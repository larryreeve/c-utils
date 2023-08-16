{------------------------------------------------------------------------------
Version History:

1.00	07/91	Larry	Initial Release for BTOS II 3.0.1 & 3.2.0
1.01	09/91	Larry	Patched run file for CTOS II (see notes below)
			Re-linked with 12.1 Libraries
1.02	10/91	Larry	Added code for validators
			Note: all vars & consts for validator prefixed with v.
------------------------------------------------------------------------------}


{---------------------
  Compiler Directives
----------------------}

(* BRAVE+   *)  {Send error messages/warnings to terminal}
(* DEBUG-   *)  {Turn off all DEBUG checking}
(* ENTRY-   *)  {DO NOT generate procedure entry/exit calls for debugger}
(* INDEXCK- *)  {DO NOT check for arrays out of bounds}
(* INITCK-  *)  {DO NOT check for uninitialized values}
(* LINE-    *)  {DO NOT generate line numbers for debugger}
(* MATHCK-  *)  {DO NOT check for overflow, divide by zero}
(* NILCK-   *)  {DO NOT check for bad pointer values}
(* RANGECK- *)  {DO NOT check ranges for arrays, CASE statements, etc}
(* SIZE+    *)  {Optimize size  of code generated}
(* SPEED-   *)  {Optimize speed of code generated}
(* STACKCK- *)  {DO NOT check for stack overflow}


{------------------------------------------------------------------------------
Request Block Format is:

	Field		Type	Contents	Description
	-------------	-----	--------	-----------
	sCntInfo	Byte	2 		Size of Control Information*
	RtCode      	Byte	0		Routing Code
	nReqPbCb	Byte	1		# Request PbCb pairs
	nRespPbCb	Byte	2		# Response PbCb pairs
	userNum		Word	0		CTOS will set this
	exchResp	Word	                Reponse exchange
	ercRet		Word			Error Code Returned
	rqCode		Word	                See CONSTANTS section
        rqFunction      Word			See table below
        rqFunction      Word			Function # of Request
        pInpData        AdsMem			Data passed in to Server
        sInpData        Word			cbData passed in to Server
        pOutData        AdsMem    		String Of Data Returned
        sOutData        Word      		Max Bytes to Read
        pOutSize        AdsWord   		Count of Bytes Returned
        sOutSize        Word    2
    			========
                        34 Bytes Total

*Note: Control information is the number of bytes after the rqCode field and
       before the PbCb pair start.

  	Request Function #		Function
	==================		=======================================
	     0				Read Tax Bill:
					  responds with 300 if no data
					  responds with BTOS erc if BTOS erc
					  responds with ercRet 0 if data avail
					  responds with erc in ercRet
             1				Read Water   Bill
 	     2				Read Sewer   Bill
	     3				Read Trash   Bill
             4				Read Utility Bill
             5				Read Utility Bill
	     9				Read Any     Bill
	-----------------------------------------------------------------------
	    60				Read  validator port
					  pass pointer to string
					  pass number of bytes to read
	    61				Write validator port
					  pass pointer to string
					  pass number of bytes to write
	-----------------------------------------------------------------------
	   240				Get Version:
					Responds w/version string in pOutData.
					If not available, responds with erc33. 
	-----------------------------------------------------------------------
           241				Get Request Code
					  responds with the BarCodeRequest 
					  RqCode that the server is using.
					  Does not return the other 4 RqCodes
					  used for Swapping, etc.
	-----------------------------------------------------------------------
           242				Get Last Error Code
					  responds with LastErc in pOutSize
	-----------------------------------------------------------------------
           245				Reset Server
					  sets BarBufSize to 0 inside server
					  for forcing read of port; throws
					  out internal buffer.
					  responds with ercRet = 0
	-----------------------------------------------------------------------
	   250			        Open Comm Port: (BarCode)
					  pass name of comm port & config file
					  in pBarCode
					  pass size of comm port & config file
					  in sBarCode
					  responds with erc in ercRet
	-----------------------------------------------------------------------
	   251				Close Comm Port: (BarCode)
					  pass no parameters
					  responds with erc in ercRet
	-----------------------------------------------------------------------
	   252				Open Comm Port: (Validator)
					  pass name of comm port & config file
					  in pBarCode
					  pass size of comm port & config file
					  in sBarCode
					  responds with erc in ercRet
	-----------------------------------------------------------------------
	   253				Close Comm Port: (Validator)
					  pass no parameters
					  responds with erc in ercRet
------------------------------------------------------------------------------}


{-----------------------------------------------------------------------------
SPECIAL NOTES:

-Bind using BarCodeCommServer.obj CmRqLabl.obj CMCalls.obj MuMovRec.obj
-Run File mode is GDTProtected

-IMPORTANT: Under CTOSII, the run file must be patched. The Pascal runtime 
checks for an OS version and then executes code based on the version. This
OS checking must be NOP'd, because the call generates ERC 80s. The version 
checking lokks for OSVersion 11 or higher and then exexutes the bad code.
If the OSVersion is less, the subroutine is exited. 

The offending routine is LCWRQQ. The offset is 1F. To patch the run file, enter:

	From the command line,

	Debug File
	  BarCodeCommServer.run
          Yes <GO>

	In the debugger, load the symbol file:
	'BarCodeCommServer.sym' <CODE-F>

	Now patch the routine,

	LCWRQQ+1F <MARK>
	NOP <DOWN ARROW>
	NOP <DOWN ARROW>
	<FINISH>
------------------------------------------------------------------------------}


Program BarCodeCommServer;

Const
   RequestCode       = #E100;
   RqCodeSwap        = #E101;
   RqCodeTerminate   = #E102;
   RqCodeCUN         = #E103;
   RqCodeWsAbort     = #E104;
   ercOK             =     0;
   ercNotImplemented =     7;
   ercNotInstalled   =    33;
   ercInstalled      =    52; 
   ercDeviceNotReady =   300;
   ercPartPrimary    =   810;          {Partition must be primary}
   ercNoCM           = 12099;
   osTRUE            =   #FF;
   osFALSE           =   #00;
   priorityLevel     =    64;          {Process Priority}
   BswaSize          =   130;          {ByteStream Work Area}
   SwaSize           =  1024;          {SAM Work Area}
   modeRead          = #6D72;
   modeModify        = #6D6D;
   modeWrite         = #6D77;
   Skip1             = Chr(10);
   Skip2             = Skip1*Skip1;
   BarVersionStr     = 'BarCode Communications Server, Version 1.02 (CTOS II)';
   BarVersionSize    = 53;

Type
  AdsWord            = Ads of Word;
  RequestInfo        = Record
    ServiceExchange    [00]  : Word;
    LocalServiceCode   [02]  : Word;
    SRProcessor        [04]  : Byte;
    NetRouting         [05]  : Byte;
  End;
  RequestBlock       = Record
   sCntInfo            [00]  : Byte;
   RtCode              [01]  : Byte;
   nReqPbCb            [02]  : Byte;
   nRespPbCb           [03]  : Byte;
   userNum             [04]  : Word;
   exchResp	       [06]  : Word;
   ercRet              [08]  : Word;
   rqCode              [10]  : Word;
   rqFunction          [12]  : Word;
   pInpData            [14]  : AdsMem;
   sInpData            [18]  : Word;
   pOutData            [20]  : AdsMem;    {String Of Data}
   sOutData            [24]  : Word;      {Max Bytes to Read}
   pOutSize            [26]  : AdsWord;   {Count of Bytes Returned}
   sOutSize            [30]  : Word;
  End;
  ByteStreamWorkArea = Array [1..BswaSize] of Char;
  SamWorkArea        = Array [1..SwaSize]  of Char;

Var
  Erc       : Word;                {BTOS error code}
  LastErc   : Word;                {Last error code that happened}
  UserNum   : Word;                {Our user number}
  Exchange  : Word;                {Mailbox for all of our requests}
  fTerminate: Boolean;             {Quit server when TRUE}
  fPortOpen : Boolean;             {TRUE if BarCode CommPort is open}
  vfPortOpen: Boolean;             {TRUE if Validator CommPort is open}
  BarBufTemp: Array [1..32] of Byte;
  BarBufSize: Word;
  vBWritten : Word;                {Count of bytes written to validator}
  AddressX  : AdsMem;              {Dummy address for copyinf BarCode data}
  Bswa      : ByteStreamWorkArea;
  vBswa     : ByteStreamWorkArea;
  Swa       : SamWorkArea;         
  vSwa      : SamWorkArea;         
  RqInfo    : RequestInfo;         {Who is currently serving our requests?}
  RqMsg     : Ads Of RequestBlock; {Request Block from Application}
  RqMsgDone : Ads Of RequestBlock; {To serve all outstanding requests on
                                    BarCodeServer terminate request.}
  cmVersion : Byte;                {Context Manager Version}
  cmRevision: Byte;                {Context Manager Revision}

{-----------------------}
{CTOS-defined procedures}
{-----------------------}
Procedure CheckErc          (Erc                : Word);          Extern;
Procedure Exit;                                                   Extern;
Procedure NPrint            (pbString           : AdsMem;
                             cbString           : Word);          Extern;

{----------------------}
{CTOS-defined functions}
{----------------------}
Function  AllocExch         (pExchRet            : AdsMem): Word; Extern;
Function  ChangePriority    (priority            : Word)  : Word; Extern;
Function  Check             (exchange            : Word;
                             ppMsgRet            : AdsMem): Word; Extern;
Function  CMCurrentVersion  (pbVersion           : AdsMem;
                             pbRevision          : AdsMem): Word; Extern;
Function  ConvertToSys:                                     Word; Extern;
Function  DeallocExch       (exchange            : Word)  : Word; Extern;
Function  FillBufferAsyncC  (pBs                 : AdsMem;
                             cbMin               : Word;
                             cbMax               : Word;
                             pPbRet              : AdsMem;
                             pCbRet              : AdsWord;
                             idExch              : Word;
                             pMsg                : AdsMem;
                             rqCode              : Word)  : Word; Extern;
Function  FlushBufferAsyncC (pBs                 : AdsMem;
                             pbTransmitData      : AdsMem;
                             cbMin               : Word;
                             cbMax               : Word;
                             pCbRet              : AdsMem;
                             idExch              : Word;
                             pMsg                : AdsMem;
                             rqCode              : Word)  : Word; Extern;
Function  GetPartitionHandle(pbPartitionName     : AdsMem;
                             cbPartitionName     : Word;
                             pUserNumPartitionRet: Adsmem): Word; Extern;
Function  GetUserNumber     (pUserNumberRet      : AdsMem): Word; Extern;
Function  MoveRec           (pDestination        : AdsMem;
                             pSource             : AdsMem;
                             CountOfBytes        : Word)  : Word; Extern;
Function  OpenByteStreamC   (pBs                 : AdsMem;
                             pbFileSpec          : AdsMem;
                             cbFileSpec          : Word;
                             pbPassword          : AdsMem;
                             cbPassword          : Word;
                             mode                : Word;
                             pBufferArea         : AdsMem;
                             sBufferArea         : Word)  : Word; Extern;
Function  QueryRequestInfo  (RequestNumber       : Word;
                             pStatusRet          : AdsMem;
                             sStatusMax          : Word)  : Word; Extern;
Function  ReleaseByteStreamC(pBswa               : Adsmem): Word; Extern;
Function  Respond           (pRq: AdsMem):                  Word; Extern;
Function  ServeRq           (requestCode         : Word;
                             exchange            : Word)  : Word; Extern;
Function  SetPartitionLock  (fLock               : Byte)  : Word; Extern;
Function  SetPartitionName  (userNumPartition    : Word;
                             pbName              : AdsMem;
                             cbName              : Word)  : Word; Extern;
Function  Wait              (exchange            : Word;
                             ppMsgRet            : AdsMem): Word; Extern;


Begin {BarCode Comm Server}

  NPrint(Ads Skip1*BarVersionStr,BarVersionSize+1);
  NPrint(Ads Skip1*'  A service of Colonial Computer Associates',44);

  Erc := CMCurrentVersion(Ads cmVersion, Ads cmRevision);
  If (Erc <> ercNoCM) Then
    Begin
      NPrint(Ads Skip2*'Cannot install server inside Context Manager!',47);
      CheckErc(ercPartPrimary);
    End;

{Check to see if BarCode Comm Server is already installed.}
  Erc := GetPartitionHandle(Ads 'BarCode', 7, Ads UserNum);
  If (Erc = ercOK) Then
    Begin
      NPrint(Ads Skip2*'BarCode Comm Server already installed!',40);
      CheckErc(ercInstalled);
    End;

{Allocate Exchange Resources}
  CheckErc(QueryRequestInfo(RequestCode,Ads RqInfo,SizeOf(RqInfo)));
  If (RqInfo.ServiceExchange <> 0) Then
    Begin
      NPrint(Ads Skip2*'Request Code in use!',22);
      NPrint(Ads Skip1*'Subsystem error is Request Code.',33);
      CheckErc(RequestCode);
    End
  Else
    CheckErc(AllocExch(Ads Exchange));

{Convert into system service}
  NPrint(Ads Skip2*'BarCode Comm Server installed.',32);
  CheckErc(ConvertToSys);
  Exit;

  CheckErc(ChangePriority(priorityLevel));
  CheckErc(SetPartitionName(0, Ads 'BarCode', 7));
  CheckErc(SetPartitionLock(osTRUE));
 
  CheckErc(ServeRq(RequestCode,    Exchange));  {BarCode Interface   Request}
  CheckErc(ServeRq(RqCodeSwap,     Exchange));  {Swapping            Request}
  CheckErc(ServeRq(RqCodeTerminate,Exchange));  {Terminate           Request}
  CheckErc(ServeRq(RqCodeCUN,      Exchange));  {Change User Number  Request}
  CheckErc(ServeRq(RqCodeWsAbort,  Exchange));  {Workstation Aborted Request}
 
  BarBufSize := 0;
  LastErc    := 0;
  fPortOpen  := False;
  vfPortOpen := False;
  fTerminate := False;
  While (Not fTerminate) Do
    Begin
      CheckErc(Wait(Exchange,Ads RqMsg));

      Case RqMsg^.rqCode of

              RqCodeSwap: Begin {Handle application swapping request}
                            RqMsg^.ercRet := ercOK;
                          End;

         RqCodeTerminate: Begin {Handle application termination request}
                            RqMsg^.ercRet := ercOK;
                          End;

               RqCodeCUN: Begin {Handle application ChangeUserNumber request}
                            RqMsg^.ercRet := ercOK;
                          End;

           RqCodeWsAbort: Begin {Handle application Abort request}
                            RqMsg^.ercRet := ercOK;
                          End;

             RequestCode:
      Begin {Application called BarCodeRequest procedure}
        Case RqMsg^.rqFunction Of

           0,1,2,3,4,5,9: 
               Begin {Read Bills}

{----------------------------------}
{Fill internal buffer, if necessary}
{----------------------------------}
                 If (BarBufSize = 0) Then
                   Begin
                     RqMsg^.ercRet:= FillBufferAsyncC(Ads Bswa,
                                                      RqMsg^.sOutData,
                                                      RqMsg^.sOutData,
                                                      Ads AddressX,
                                                      Ads BarBufSize,
                                                      0,
                                                      Ads NULL,
                                                      0);

                     If (RqMsg^.ercRet <> ercOK) Then
                       BarBufSize := 0
                     Else
                       CheckErc(MoveRec(Ads BarBufTemp,
                                        AddressX,
                                        BarBufSize));
                   End;

{-------------------------------}
{Check if bill type is available}
{-------------------------------}
                 If (BarBufSize > 0) Then
                   Begin
                     If (Chr(BarBufTemp[1]) >= '0') or 
                        (RqMsg^.rqFunction = 9) Then
                       Begin
{----------------------------------------------------}
{ If the bill type is a Tax Bill '0',             or }
{ if the bill type is any Utility Bill '1' - '5', or }
{ if the user wants any type of bill '9'             }
{----------------------------------------------------}
                         If ((Chr(BarBufTemp[1]) = '0')          OR
                            (Chr(BarBufTemp[1]) in ['1'..'5']))  OR
                            (RqMsg^.rqFunction = 9) Then
                           Begin
                             CheckErc(MoveRec(RqMsg^.pOutData,
                                              Ads BarBufTemp,
                                              BarBufSize));
                             RqMsg^.pOutSize^ := BarBufSize;
                             RqMsg^.ercRet    := ercOK;
                             BarBufSize       := 0;
                           End
                         Else
                           Begin
                             RqMsg^.pOutSize^ := 0;
                             RqMsg^.ercRet    := ercDeviceNotReady;
                           End;
                       End
                     Else
                       Begin
                         RqMsg^.pOutSize^ := 0;
                         RqMsg^.ercRet    := ercDeviceNotReady;
                       End;
                   End
                 Else
                   Begin
                     RqMsg^.pOutSize^ := 0;
                     RqMsg^.ercRet    := ercDeviceNotReady;
                   End;
               End;  {Read Bill}

           60: Begin {Read Validator String}
                 RqMsg^.ercRet:= FillBufferAsyncC(Ads vBswa,
                                                  RqMsg^.sOutData,
                                                  RqMsg^.sOutData,
                                                  Ads AddressX,
                                                  Ads BarBufSize,
                                                  0,
                                                  Ads NULL,
                                                  0);

                 If (RqMsg^.ercRet = ercOK) Then
                   Begin
                     CheckErc(MoveRec(RqMsg^.pOutData,
                                      AddressX,
                                      BarBufSize));
                     RqMsg^.pOutSize^ := BarBufSize;
                     RqMsg^.ercRet    := ercOK;
                     BarBufSize       := 0;
                   End
                 Else
                   Begin
                     RqMsg^.pOutSize^ := 0;
                     RqMsg^.ercRet    := ercDeviceNotReady;
                   End;
               End;

           61: Begin {Write Validator String}
                 RqMsg^.ercRet:= FlushBufferAsyncC(Ads vBswa,
                                                   RqMsg^.pInpData,
                                                   RqMsg^.sInpData,
                                                   RqMsg^.sInpData,
                                                   Ads vBWritten,
                                                   0,
                                                   Ads NULL,
                                                   0);
                 RqMsg^.pOutSize^ := vBWritten;
                 RqMsg^.soutSize := 2;
               End;

          240: Begin {Get Version}
                 CheckErc(MoveRec(RqMsg^.pOutData,
                                  Ads BarVersionStr,
                                  BarVersionSize));
                 RqMsg^.pOutSize^ := BarVersionSize;
                 RqMsg^.ercRet    := 0;
               End; {Get Version}

          241: Begin {Get Request Code}
                 RqMsg^.pOutSize^ := RequestCode;
                 RqMsg^.ercRet    := 0;
               End;  {Get Request Code}

          242: Begin {Get Last Error Code}
                 RqMsg^.pOutSize^ := LastErc;
                 RqMsg^.ercRet    := 0;
                 LastErc          := 0;
               End;  {Get Request Code}

          245: Begin {Reset Server}
                 BarBufSize    := 0;
                 RqMsg^.ercRet := ercOk;
               End;  {Reset Server}

          250: Begin {Open BarCode Port}
                 If (fPortOpen) Then 
                   RqMsg^.ercRet:= ReleaseByteStreamC(Ads Bswa);
                 RqMsg^.ercRet:= OpenByteStreamC(Ads Bswa,
                                                 RqMsg^.pInpData,
                                                 RqMsg^.sInpData,
                                                 Ads NULL,
                                                 0,
                                                 modeRead,
                                                 Ads Swa,
                                                 SwaSize);
                 fPortOpen:= (RqMsg^.ercRet = 0)
               End;  {Open Port}

          251: Begin {Close BarCode Port}
                 If (fPortOpen) Then
                   RqMsg^.ercRet:= ReleaseByteStreamC(Ads Bswa)
                 Else
                   RqMsg^.ercRet:= ercOK;
                 fPortOpen := FALSE;
               End;  {Close Port}

          252: Begin {Open Validator Port}
                 If (vfPortOpen) Then 
                   RqMsg^.ercRet:= ReleaseByteStreamC(Ads vBswa);
                 RqMsg^.ercRet:= OpenByteStreamC(Ads vBswa,
                                                 RqMsg^.pInpData,
                                                 RqMsg^.sInpData,
                                                 Ads NULL,
                                                 0,
                                                 modeModify,
                                                 Ads vSwa,
                                                 SwaSize);
                 vfPortOpen:= (RqMsg^.ercRet = 0)
               End;  {Open Port}

          253: Begin {Close Validator Port}
                 If (vfPortOpen) Then
                   RqMsg^.ercRet:= ReleaseByteStreamC(Ads vBswa)
                 Else
                   RqMsg^.ercRet:= ercOK;
                 vfPortOpen := FALSE;
               End;  {Close Port}

          255: Begin {Terminate Server}
                 While (Check(Exchange, Ads RqMsgDone) = 0) Do
                   Begin
                     RqMsgDone^.pOutSize^ := 0;
                     RqMsgDone^.ercRet    := ercNotInstalled;
                     CheckErc(Respond(RqMsgDone))
                   End;

                 CheckErc(ServeRq(RequestCode,    0));
                 CheckErc(ServeRq(RqCodeSwap,     0));
                 CheckErc(ServeRq(RqCodeTerminate,0));
                 CheckErc(ServeRq(RqCodeCUN,      0));
                 CheckErc(ServeRq(RqCodeWsAbort,  0));

                 If (fPortOpen) Then
                   CheckErc(ReleaseByteStreamC(Ads Bswa));

                 fTerminate := TRUE;

                 CheckErc(GetUserNumber(Ads UserNum));
                 RqMsg^.pOutSize^  := userNum;

                 RqMsg^.ercRet     := 0;
               End; {Terminate Server}

          Otherwise {Unknown Function Passed}
               Begin
                 RqMsg^.ercRet    := ercNotImplemented;
                 RqMsg^.pOutSize^ := 0; 
               End;
        End; 
      End; {Request Code Block}
      End; {Case -> Request}
      If (RqMsg^.ercRet > 0) Then
        LastErc := RqMsg^.ercRet;
      CheckErc(Respond(RqMsg));
    End;
  CheckErc(DeallocExch(Exchange));
  CheckErc(SetPartitionLock(osFALSE));
End.
   := ercOK;
