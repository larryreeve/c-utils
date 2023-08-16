{------------------------------------------------------------------------------
Version History:

1.00	07/91	Larry	Initial Release for BTOS II 3.0.1 & 3.2.0

1.01	09/91	Larry	Patched run file for CTOS II (see notes below)
                        Re-linked with 12.1 Libraries

1.02    06/92	Larry	Re-linked with 12.1.4 Libraries & CTOS 3.3.4

1.03    07/92   Larry   Fixed type 9 utility bill to return any utility bill.
                        Added function 10 to read any bill type.
                        Renamed BarCodeCommServer.run to BarCodeService.run
                        Linked using 12.1.4 Libs (Latest Libs)
                        Moved all utilities to BarCodeUtilities.run &
                        changed commands to have case values (which are case
                        sensitive):
                          CL - Close Comm Port
                          DE - Deinstall Service
                          GV - Get BarCode Version
                          QE - Query Last Service Erc
                          RS - Reset Service
                          RT - Read Test

1.04    07/92   Larry   Linked Revised version with 12.1 Libs.
                        12.1.4 Libs do not work on SG2000 Comm Ports.

1.05    08/92   Larry   Added Start/Stop Characters in bills. Can still use 
                        older bills. Implemented ReadBill procedure.
                        Replaced MuSoft MovRec call with Pascal MoveSL.
                        Reset Service call also resets last error code.
                        Reset Service now clears comm buffer a byte at a time.
                        Added BarCode Close Port command for ports left in an 
                        open mode after program exit (e.g., MuSoft crash).

1.06	10/92	Larry	Cleaned up output messages.
                        Parameters can now be passed to BarCodeService.run:
                          Param 1 - debugging log file
                          Param 2 - process priority
                          Param 3 - read character delay
                        Approximate response time is 1 bill per two seconds
                        ReadBill calls ResetService if a non-numeric char
                        is read.
                        ResetService delays a few milliseconds between 
                        character reads to allow chars time to get into buffer.
------------------------------------------------------------------------------}


{---------------------
  Compiler Directives
----------------------}

(* BRAVE+   *)  {Send error messages/warnings to terminal                }
(* DEBUG-   *)  {Turn off all DEBUG checking                             }
(* ENTRY-   *)  {DO NOT generate procedure entry/exit calls for debugger }
(* INDEXCK- *)  {DO NOT check for arrays out of bounds                   }
(* INITCK-  *)  {DO NOT check for uninitialized values                   }
(* LINE-    *)  {DO NOT generate line numbers for debugger               }
(* MATHCK-  *)  {DO NOT check for overflow, divide by zero               }
(* NILCK-   *)  {DO NOT check for bad pointer values                     }
(* RANGECK- *)  {DO NOT check ranges for arrays, CASE statements, etc    }
(* SIZE+    *)  {Optimize size  of code generated                        }
(* SPEED-   *)  {Optimize speed of code generated                        }
(* STACKCK- *)  {DO NOT check for stack overflow                         }


{------------------------------------------------------------------------------
Request Block Format is:

        Field         Type  Contents Description
        ------------- ----- -------- -----------
        sCntInfo      Byte  2        Size of Control Information*
        RtCode        Byte  0        Routing Code
        nReqPbCb      Byte  1        # Request PbCb pairs
        nRespPbCb     Byte  2        # Response PbCb pairs
        userNum       Word  0        CTOS will set this
        exchResp      Word           Reponse exchange
        ercRet        Word           Error Code Returned
        rqCode        Word           See CONSTANTS section
        rqFunction    Word           Function # of Request; See table below
        pInpData      AdsMem         pbData passed in to Service
        sInpData      Word           cbData passed in to Service
        pOutData      AdsMem         pbString Of Data Returned
        sOutData      Word           Max Bytes to Read
        pOutSize      AdsWord        Count of Bytes Returned
        sOutSize      Word  2
                     ========
                     34 Bytes Total Record Size

*Note: Control information is the number of bytes after the rqCode field and
       before the PbCb pair start.

    Request Function #      Function
    ==================      =======================================
             0              Read Tax Bill:
                              responds with 300 if no data
                              responds with BTOS erc if BTOS erc
                              responds with ercRet 0 if data avail

             1              Read Water Bill
             2              Read Sewer Bill
             3              Read Trash Bill
             4              Undefined Utility Bill
             5              Undefined Utility Bill
             6              Undefined Bill
             7              Undefined Bill
             8              Undefined Bill
             9              Read Any  Utility Bill
            10              Read Any Bill
    -----------------------------------------------------------------------
           240              GetVersion:
                              Responds w/version string in pOutData.
                              If not available, responds with erc 33. 
    -----------------------------------------------------------------------
           241              GetRequestCode
                              responds with the BarCodeRequest 
                              RqCode that the service is using.
                              Does not return the other 4 RqCodes
                              used for Swapping, etc.
    -----------------------------------------------------------------------
           242              Get Last Error Code
                              responds with LastErc in pOutSize
    -----------------------------------------------------------------------
           245              ResetService
                              sets BarBufSize to 0 inside service for forcing
                              read of port; throws out internal buffer / 
                              clears comm port.
    -----------------------------------------------------------------------
           250              Open Comm Port:
                              pass name of comm port & config file in pBarCode
                              pass size of comm port & config file in sBarCode
                              responds with erc in ercRet
    -----------------------------------------------------------------------
           251              Close Comm Port:
                              pass no parameters
                              responds with erc in ercRet
------------------------------------------------------------------------------}


{-----------------------------------------------------------------------------
SPECIAL NOTES:

- Use Link V6 command
- Run File mode is GDTProtected

- IMPORTANT: Under CTOSII, the run file must be patched. The Pascal 6.02 
runtime checks for an OS version and then executes code based on the version. 
This OS checking must be NOP'd, because the call generates ERC 80s. The 
version checking lokks for OSVersion 11 or higher and then exexutes the bad 
code. If the OSVersion is less, the subroutine is exited. 

The offending routine is LCWRQQ. The offset is 1F. To patch the run file, enter from the command line,

    Debug File
      BarCodeService.run
          Yes <GO>

    In the debugger, load the symbol file:
    'BarCodeService.sym' <CODE-F>

    Now patch the routine,

    LCWRQQ+1F <MARK>
    NOP <DOWN ARROW>
    NOP <DOWN ARROW>
    <FINISH>
------------------------------------------------------------------------------}
Program BarCodeService(Output);

Const
   RqCodePrimary     = #E100;
   RqCodeSwap        = #E101;
   RqCodeTerminate   = #E102;
   RqCodeCUN         = #E103;
   RqCodeWsAbort     = #E104;
   RqCodeNone        = #0000;
   ercOK             =     0;
   ercEOF            =     1;
   ercNotImplemented =     7;
   ercNotInstalled   =    33;
   ercInstalled      =    52;
   ercPortNotOpen    =   203; 
   ercDeviceNotReady =   300;
   ercPartPrimary    =   810;          {Partition must be primary         }
   ercNoCM           = 12099;
   osTRUE            =   #FF;
   osFALSE           =   #00;
   DefPriority       =    64;          {Process Priority                  }
   DefReadDelay      =    15;          {Delay Before Clearing Comm Buffer }
   BswaSize          =   130;          {ByteStream Work Area              }
   SwaSize           =  1024;          {SAM Work Area                     }
   modeRead          = #6D72;
   modeModify        = #6D6D;
   modeWrite         = #6D77;
   chStart           = '*';
   chStop            = '%';
   BarVersionStr     = 'BarCode Service, Version 1.06';
   BarVersionSize    = 29;
   imageBinary       = 2;

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
   exchResp            [06]  : Word;
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
  ParameterRecord    = Record
    StrAddr            [00]  : AdsMem;
    Leng               [04]  : Word;
  End;
  ByteStreamWorkArea = Array [1..BswaSize] of Char;
  SamWorkArea        = Array [1..SwaSize]  of Char;

Var
  Erc       : Word;                {CTOS error code                       }
  LastErc   : Word;                {Last CTOS error code                  }
  UserNum   : Word;                {Our user number                       }
  Exchange  : Word;                {Mailbox for all of our requests       }
  Priority  : Word;                {Process Priority                      }
  ReadDelay : Word;                {Delay time between readings chars     }
  fTerminate: Boolean;             {Quit service when TRUE                }
  fPortOpen : Boolean;             {TRUE if CommPort is open              }
  fSendBill : Boolean;             {TRUE if bill type is available        }
  fDebug    : Boolean;             {TRUE if log file turned on.           }
  BarBufTemp: Array [1..32] of Byte;
  BarBufSize: Word;
  TempStr   : LString(128);
  LogFH     : Text;                {Log File Handle                       }
  AddressX  : AdsMem;              {Dummy address for copying BarCode data}
  Bswa      : ByteStreamWorkArea;
  Swa       : SamWorkArea;         
  RqInfo    : RequestInfo;         {Who is currently serving our requests?}
  RqMsg     : Ads Of RequestBlock; {Request Block from Application        }
  RqMsgDone : Ads Of RequestBlock; {To serve all outstanding requests on
                                    BarCodeService terminate request.     }
  cmVersion : Byte;                {Context Manager Version               }
  cmRevision: Byte;                {Context Manager Revision              }
  Sd        : ParameterRecord;     {Command Line Parameters               }

{-----------------------}
{CTOS-defined procedures}
{-----------------------}
Procedure CheckErc          (Erc                : Word);          Extern;

Procedure Exit;                                                   Extern;

{----------------------}
{CTOS-defined functions}
{----------------------}
Function  AllocExch         (pExchRet            : AdsMem): Word; Extern;

Function  Beep                                            : Word; Extern;

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

Function  GetPartitionHandle(pbPartitionName     : AdsMem;
                             cbPartitionName     : Word;
                             pUserNumPartitionRet: Adsmem): Word; Extern;

Function  GetUserNumber     (pUserNumberRet      : AdsMem): Word; Extern;

Function  OpenByteStreamC   (pBswa               : AdsMem;
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

Function  RgParam           (iParam              : Word;
                             jParam              : Word;
                             psdRet              : AdsMem): Word; Extern;

Function  ServeRq           (requestCode         : Word;
                             exchange            : Word)  : Word; Extern;

Function  SetImageModeC     (pBswa               : AdsMem;
                             mode                : Word)  : Word; Extern;

Function  SetPartitionLock  (fLock               : Byte)  : Word; Extern;

Function  SetPartitionName  (userNumPartition    : Word;
                             pbName              : AdsMem;
                             cbName              : Word)  : Word; Extern;

Function  ShortDelay        (milliseconds        : Word)  : Word; Extern;

Function  Wait              (exchange            : Word;
                             ppMsgRet            : AdsMem): Word; Extern;


{----------------------------}
{ Check Check-Digits of Bill }
{----------------------------}
Function ValidBill: Boolean;
Begin
  ValidBill := TRUE;
End;


{--------------------}
{ Clear Comm Buffers }
{--------------------}
Procedure ResetService;
Var
  CbRet : Word;
  Ch    : Byte;
Begin

  If (fDebug) Then
    Begin
      MoveSL(Ads BarBufTemp,Ads TempStr[1],BarBufSize);
      TempStr[0] := Chr(Ord(BarBufSize));
      Writeln(LogFH,'Service Reset Begin, BarBufTemp is "',TempStr,'"');
      Write  (LogFH,'  Data remaining is: "');
    End;

  {--------------------------------}
  { Clear Temporary Buffer to Zero }
  {--------------------------------}
  BarBufSize := 0;
  FillSC(Ads BarBufTemp, SizeOf(BarBufTemp), Chr(0));

  {-------------------}
  { Clear Comm Buffer }
  {-------------------}
  RqMsg^.ercRet := ercOK;
  While (RqMsg^.ercRet = ercOK) Do
    Begin
      CheckErc(ShortDelay(ReadDelay));
      RqMsg^.ercRet := FillBufferAsyncC(Ads Bswa,1,1,Ads AddressX,Ads CbRet,
                                        0,Ads NULL,0);
      If (fDebug) AND (RqMsg^.ercRet = ercOK) Then 
        Begin
          MoveSL(AddressX,Ads Ch,1);
          Write(LogFH,Chr(Ch));
        End;
    End;

  If (fDebug) Then
    Begin
      Writeln(LogFH,'"');
      Writeln(LogFH,'  Service Reset End, Erc #',RqMsg^.ercRet:-5);
      CheckErc(Beep);
      CheckErc(Beep);
    End;

  RqMsg^.ercRet := ercOK;
End;


Procedure ReadBill;
Var
  Ch        : Byte;
  CbRet     : Word;
  Erc       : Word;
  fTerminate: Boolean;
  I         : Integer;
Begin
  {----------------}
  { Read Bill Loop }
  {----------------}
  fTerminate := FALSE;
  While (Not fTerminate) Do
    Begin
      If (NOT fPortOpen) Then
        Erc := ercPortNotOpen
      Else
        Erc := FillBufferAsyncC(Ads Bswa,1,1,Ads AddressX,Ads CbRet,
                                0,Ads NULL,0);
      If (Erc = ercOk) Then
        Begin
          MoveSL(AddressX,Ads Ch,1);
          Case Ch Of
            Ord(chStart): BarBufSize := 0;

            Ord(chStop) : Begin
                            RqMsg^.ercRet := ercOk;
                            fTerminate    := TRUE;
                            BarBufSize    := RqMsg^.sOutData;
                          End;

            Otherwise     Begin
                            If (NOT (Chr(Ch) in ['0'..'9'])) Then
                              Begin
                                If (fDebug) Then
                                  Writeln(LogFH,'Invalid char: ',Chr(Ch));
                                ResetService;
                                Erc           := ercDeviceNotReady;
                                fTerminate    := TRUE;
                              End
                            Else
                              Begin
                                BarBufSize := BarBufSize + 1;
                                BarBufTemp[Ord(BarBufSize)] := Ch;
                                If (BarBufSize >= RqMsg^.sOutData) Then 
                                  fTerminate := TRUE;
                              End;
                          End;
          End; {Case}
        End
      Else fTerminate := TRUE;
    End;
  RqMsg^.ercRet := Erc;
End;



Begin {BarCode Service}

  {--------------}
  { Title Screen }
  {--------------}
  Writeln(BarVersionStr);
  Writeln('A service of United Computer Sales & Service, Inc.');
  Writeln;


  {---------------------------}
  { Check for Context Manager }
  {---------------------------}
  Erc := CMCurrentVersion(Ads cmVersion, Ads cmRevision);
  If (Erc <> ercNoCM) Then
    Begin
      Writeln('Service cannot be installed inside of Context Manager!');
      Exit;
    End;


  {-------------------------------------------------------}
  { Check to see if BarCode Service is already installed. }
  {-------------------------------------------------------}
  Erc := GetPartitionHandle(Ads 'BarCode', 7, Ads UserNum);
  If (Erc = ercOK) Then
    Begin
      Writeln('BarCode Service already installed!');
      Exit;
    End;


  {---------------------------}
  { Check For & Open Log File }
  {---------------------------}
  Erc := RgParam(1,0,Ads Sd);
  If (Erc = ercOk) Then
    Begin
      MoveSL(Sd.StrAddr,Ads TempStr[1],Sd.Leng);
      TempStr[0] := Chr(Ord(Sd.Leng));
    End
  Else
    TempStr := NULL;

  fDebug := (NOT (TempStr = NULL));

  If (fDebug) Then
    Begin
      Writeln('Debugging log file is ',TempStr);
      Assign(LogFH,TempStr);
      Rewrite(LogFH);
      Writeln(LogFH,'*----- START LOGGING -----*');
    End;


  {----------------------------------------}
  { Check For Non-Default Process Priority }
  {----------------------------------------}
  Priority := DefPriority;
  Erc := RgParam(2,0,Ads Sd);
  If (Erc = ercOk) Then
    Begin
      MoveSL(Sd.StrAddr,Ads TempStr[1],Sd.Leng);
      TempStr[0] := Chr(Ord(Sd.Leng));
      If Decode(TempStr,Priority) Then
        If ((Priority < 0) OR (Priority > 255)) Then
          Priority := DefPriority;
      If (fDebug) Then
        Writeln(LogFH,'Process priority set to ',Priority:-3);
      Writeln('Process priority set to ',Priority:-3);
    End;


  {--------------------------------------------}
  { Check For Non-Default Read Character Delay }
  {--------------------------------------------}
  ReadDelay := DefReadDelay;
  Erc := RgParam(3,0,Ads Sd);
  If (Erc = ercOk) Then
    Begin
      MoveSL(Sd.StrAddr,Ads TempStr[1],Sd.Leng);
      TempStr[0] := Chr(Ord(Sd.Leng));
      If Decode(TempStr,ReadDelay) Then
        If (ReadDelay < 0) Then
          ReadDelay := DefReadDelay;
      If (fDebug) Then
        Writeln(LogFH,'Read delay set to ',ReadDelay:3,' milli-seconds.');
      Writeln('Read delay set to ',ReadDelay:3,' milli-seconds.');
    End;


  {-----------------------------}
  { Allocate Exchange Resources }
  {-----------------------------}
  CheckErc(QueryRequestInfo(RqCodePrimary,Ads RqInfo,SizeOf(RqInfo)));
  If (RqInfo.ServiceExchange <> 0) Then
    Begin
      Writeln('Request Code in use!');
      Writeln('Subsystem error is Request Code.');
      CheckErc(RqCodePrimary);
    End
  Else
    CheckErc(AllocExch(Ads Exchange));


  {-----------------------------}
  { Convert into system service }
  {-----------------------------}
  Writeln('BarCode Service installed.');
  CheckErc(ConvertToSys);
  Exit;
  CheckErc(ChangePriority(Priority));
  CheckErc(SetPartitionName(0, Ads 'BarCode', 7));
  CheckErc(SetPartitionLock(osTRUE));

  CheckErc(ServeRq(RqCodePrimary,  Exchange));  {BarCode Interface   Request}
  CheckErc(ServeRq(RqCodeSwap,     Exchange));  {Swapping            Request}
  CheckErc(ServeRq(RqCodeTerminate,Exchange));  {Terminate           Request}
  CheckErc(ServeRq(RqCodeCUN,      Exchange));  {Change User Number  Request}
  CheckErc(ServeRq(RqCodeWsAbort,  Exchange));  {Workstation Aborted Request}


  {-----------}
  { Main Loop } 
  {-----------}
  BarBufSize:= 0;
  LastErc   := ercOk;
  fPortOpen := False;
  fTerminate:= False;
  fSendBill := False;

  While (Not fTerminate) Do
    Begin
      Erc := Wait(Exchange,Ads RqMsg);
      If (fDebug) AND (Erc <> ercOK) Then
        Begin
          Writeln(LogFH,'WAIT call generated erc #',Erc:-5);
          RqMsg^.rqCode := RqCodeNone;
        End;

      Case RqMsg^.rqCode of

              RqCodeSwap: Begin {Handle application Swapping request}
                            RqMsg^.ercRet := ercOK;
                          End;

         RqCodeTerminate: Begin {Handle application Termination request}
                            RqMsg^.ercRet := ercOK;
                          End;

               RqCodeCUN: Begin {Handle application ChangeUserNumber request}
                            RqMsg^.ercRet := ercOK;
                          End;

           RqCodeWsAbort: Begin {Handle application Abort request}
                            RqMsg^.ercRet := ercOK;
                          End;

           RqCodePrimary:
      Begin {Application called BarCodeRequest procedure}
        Case RqMsg^.rqFunction Of

           0..10:
               Begin {Read Bills}

                 fSendBill := False;

                 {------------------------------------}
                 { Fill internal buffer, if necessary }
                 {------------------------------------}
                 If (BarBufSize < RqMsg^.sOutData) Then ReadBill;

                 {---------------------------------}
                 { Check if bill type is available }
                 {---------------------------------}
                 If (BarBufSize >= RqMsg^.sOutData) Then
                   Begin
                     If (NOT ValidBill) Then 
                       Begin 
                         ResetService;
                       End
                     Else
                       Begin
                         Case RqMsg^.rqFunction Of
                         {----------}
                         { Tax Bill }
                         {----------}
                         0: If (Chr(BarBufTemp[1]) = '0') Then 
                              fSendBill := TRUE;

                         {-----------------------------}
                         { Check Specific Utility Bill }
                         {-----------------------------}
                         1: If (Chr(BarBufTemp[1]) = '1') Then fSendBill:=TRUE;
                         2: If (Chr(BarBufTemp[1]) = '2') Then fSendBill:=TRUE;
                         3: If (Chr(BarBufTemp[1]) = '3') Then fSendBill:=TRUE;
                         4: If (Chr(BarBufTemp[1]) = '4') Then fSendBill:=TRUE;
                         5: If (Chr(BarBufTemp[1]) = '5') Then fSendBill:=TRUE;

                         {-------------------}
                         { Check Other Bills }
                         {-------------------}
                         6: If (Chr(BarBufTemp[1]) = '6') Then fSendBill:=TRUE;
                         7: If (Chr(BarBufTemp[1]) = '7') Then fSendBill:=TRUE;
                         8: If (Chr(BarBufTemp[1]) = '8') Then fSendBill:=TRUE;
 
                         {----------------------------}
                         { Check General Utility Bill }
                         {----------------------------}
                         9: If (Chr(BarBufTemp[1]) in ['1'..'5','9']) Then
                              fSendBill := TRUE;

                         {---------------}
                         { Send Any Bill }
                         {---------------}
                        10: fSendBill := TRUE;

                         End; {Case}

                         {-------}
                         { Debug }
                         {-------}
                         If (fDebug) Then
                           Begin
                             MoveSL(Ads BarBufTemp,Ads TempStr[1],BarBufSize);
                             TempStr[0] := Chr(Ord(BarBufSize));
                             Write  (LogFH,'ReadBill #');
                             Write  (LogFH,RqMsg^.rqFunction:3,', data is: "');
                             Write  (LogFH,TempStr,'"');
                             Write(LogFH,' Bill ');
                             If (fSendBill) Then
                               Writeln(LogFH,'was sent.')
                             Else
                               Writeln(LogFH,'was not sent.');
                           End;
                       End;
                   End;

                 {-----------------------------------------}
                 { If Bill Type Is Available, Then Send It }
                 {-----------------------------------------}
                 If (fSendBill) Then
                   Begin
                     MoveSL(Ads BarBufTemp,
                            RqMsg^.pOutData,
                            BarBufSize);
                     RqMsg^.pOutSize^ := BarBufSize;
                     BarBufSize       := 0;
                     RqMsg^.ercRet    := ercOK;
                   End
                 Else
                   Begin
                     RqMsg^.pOutSize^ := 0;
                     RqMsg^.ercRet    := ercDeviceNotReady;
                   End;

               End;  {Read Bills}

          240: Begin {Get Version}
                 MoveSL(Ads BarVersionStr,
                        RqMsg^.pOutData,
                        BarVersionSize);
                 RqMsg^.pOutSize^ := BarVersionSize;
                 RqMsg^.ercRet    := ercOK;
                 If (fDebug) Then
                   Writeln(LogFH,'GetVersion function, returned ',
                                 BarVersionStr);
               End; {Get Version}

          241: Begin {Get Request Code}
                 RqMsg^.pOutSize^ := RqCodePrimary;
                 RqMsg^.ercRet    := 0;
                 If (fDebug) Then
                   Writeln(LogFH,'GetRequestCode function, returned erc #',
                                 RqCodePrimary:-5);
               End;  {Get Request Code}

          242: Begin {Get Last Error Code}
                 RqMsg^.pOutSize^ := LastErc;
                 RqMsg^.ercRet    := 0;
                 LastErc          := ercOk;
                 If (fDebug) Then
                   Writeln(LogFH,'GetLastErrorCode function, returned erc #',
                                 RqMsg^.ercRet:-5);
               End;  {Get Request Code}

          245: ResetService;

          250: Begin {Open Port}
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

                 fPortOpen:= (RqMsg^.ercRet = ercOK);

                 If (fPortOpen) Then
                   RqMsg^.ercRet := SetImageModeC(Ads Bswa, imageBinary);

                 If (fDebug) Then
                   Begin
                     MoveSL(RqMsg^.pInpData,Ads TempStr[1],RqMsg^.sInpData);
                     TempStr[0] := Chr(Ord(RqMsg^.sInpData));
                     Writeln(LogFH,'OpenPort requested for "',TempStr,'"');
                     Writeln(LogFH,'OpenPort function, returned erc #',
                             RqMsg^.ercRet:5);
                   End;

               End;  {Open Port}

          251: Begin {Close Port}
                 If (fPortOpen) Then
                   RqMsg^.ercRet:= ReleaseByteStreamC(Ads Bswa)
                 Else
                   Begin
                     RqMsg^.ercRet:= ercOK;
                     If (fDebug) Then
                       Writeln(LogFH,'ClosePort function, port was not open.');
                   End;

                 FillSC(Ads Bswa, SizeOf(Bswa), Chr(0));
                 FillSC(Ads Swa,  SizeOf(Swa),  Chr(0));

                 fPortOpen := FALSE;

                 If (fDebug) Then 
                   Writeln(LogFH,'ClosePort function, returned erc #',
                                  RqMsg^.ercRet:5);
               End;  {Close Port}

          255: Begin {Terminate Service}
                 While (Check(Exchange, Ads RqMsgDone) = ercOK) Do
                   Begin
                     RqMsgDone^.pOutSize^ := 0;
                     RqMsgDone^.ercRet    := ercNotInstalled;
                     CheckErc(Respond(RqMsgDone))
                   End;

                 CheckErc(ServeRq(RqCodePrimary,  0));
                 CheckErc(ServeRq(RqCodeSwap,     0));
                 CheckErc(ServeRq(RqCodeTerminate,0));
                 CheckErc(ServeRq(RqCodeCUN,      0));
                 CheckErc(ServeRq(RqCodeWsAbort,  0));

                 If (fPortOpen) Then
                   RqMsg^.ercRet:= ReleaseByteStreamC(Ads Bswa);

                 fTerminate := TRUE;

                 RqMsg^.ercRet     := GetUserNumber(Ads UserNum);
                 RqMsg^.pOutSize^  := UserNum;

                 If (fDebug) Then
                     Writeln(LogFH,'TERMINATE returned erc #',
                                    RqMsg^.ercRet:-5);

               End; {Terminate Service}

          Otherwise {Unknown Function Passed}
               Begin
                 RqMsg^.ercRet    := ercNotImplemented;
                 RqMsg^.pOutSize^ := 0;
                 If (fDebug) Then
                   Writeln(LogFH,'Unknown function "',RqMsg^.rqFunction:5,'"');
               End;
        End; 
      End; {Request Code Block}
      End; {Case -> Request}
      If (RqMsg^.ercRet <> ercOK) Then
        Begin
          LastErc := RqMsg^.ercRet;
          If (LastErc <> ercDeviceNotReady) Then
            Begin
              If (fDebug) Then
                Writeln(LogFH,'Erc #',LastErc:-5,' on function #',
                               RqMsg^.rqFunction);
            End;
        End;
      Erc := Respond(RqMsg);
      If (fDebug) AND (Erc <> ercOK) Then
        Writeln(LogFH,'RESPOND call generated erc #',Erc:-5);
    End;
  If (fDebug) Then 
    Begin
      Writeln(LogFH,'*----- STOP LOGGING -----*');
      Close(LogFH);
    End;
  Erc := SetPartitionLock(osFALSE);
  Erc := DeallocExch(Exchange);
End.
