{$DEBUG-}
{$ENTRY-}
{$INDEXCK-}
{$INITCK-}
{$LINE-}
{$MATHCK-}
{$NILCK-}
{$RANGECK-}
{$STACKCK-}
{
  Bar Code Utilities.
    Link using BarCodeUtilities.obj, UserRqLabel.obj
}
Program BarCodeUtilities(Input,Output);

Const
   ercOK              =   0;
   ercNoService       =  33;
   ercInstalled       =  52; 
   ercDeviceNotReady  = 300;
   ercNoSuchPartition = 804;
   MAXBUFFERSIZE      = 128;
   keyF1              =  21;
   keyGO              =  27;
   keyCANCEL          =   7;
   keyFINISH          =   4; 

Type
  HexString           = LString(4);
  BufferType = Record
     Wa [00]: Array [1..MAXBUFFERSIZE] of Byte;
  End;

Var
  userNum  : Word;
  Erc      : Word;
  I        : Integer;
  Sd       : Record
    StrAddr     : AdsMem;
    Leng        : Word;
  End;
  CommCase : LString(2);
  GenStr   : LString(64);
  GenWrd   : Word;
  HexStr   : HexString;
  CfgFile  : LString(64);
  Buffer   : BufferType;
  BufLen   : Word;
  LastKey  : Word;
  fCont    : Boolean;


{CTOS-defined procedures}
Procedure CheckErc       (Erc: Word);                             Extern;
Procedure Exit;                                                   Extern;
Procedure NPrint         (pbString: AdsMem;
                          CbString: Word);                        Extern;
{CTOS-defined functions}
Function  BarCodeRequest (wFunctionNumber        : Word;
                          pInpData               : AdsMem;
                          sInpData               : Word;
                          pOutData               : AdsMem;
                          sOutData               : Word;
                          pOutSizeRet            : AdsMem;
                          sOutSizeRet            : Word)  : Word; Extern;

Function  GetPartitionHandle(pbPartitionName     : AdsMem;
                             cbPartitionName     : Word;
                             pUserNumPartition   : AdsMem): Word; Extern;

Function  ReadKbdDirect     (mode                : Word;
                             pCharRet            : AdsMem): Word; Extern;

Function  RemovePartition   (userNumPartition    : Word)  : Word; Extern;

Function  RgParam           (iParam              : Word;
                             jParam              : Word;
                             psdRet              : AdsMem): Word; Extern;

Function  VacatePartition   (userNumPartition    : Word)  : Word; Extern;


Function ConvertToHex (W: Word): HexString;
Var
  TempWord: Word;            {Temporary 16-bit holder}
  TempS   : HexString;       {Temporary Serial numvber string}
  TempC   : Char;            {Temporary hex character holder}
  HexTable: LString(16);     {Hex set}
  Quot    : Word;            {Quotient}      
  Rem     : Word;            {Remainder}
Begin
  HexTable := '0123456789ABCDEF';

  TempS:=NULL;

{Build lo-word value}
  TempWord:=W;
  Quot    :=W;
  While (Quot <> 0) Do
    Begin
      Quot:=TempWord Div 16;
      Rem :=TempWord - 16*Quot;
      TempC:=HexTable[Rem+1];
      Insert(TempC,TempS,1);
      TempWord:=Quot;
    End;
{Pad string with ZEROs}
  While (TempS.Len < 4) Do
    Insert('0',TempS,1);
  ConvertToHex:=TempS;
End;

Function FinishKeyWaiting: Boolean;
Var
  Erc: Word;
Begin
  LastKey := 0;
  Erc := ReadKbdDirect(1,Ads LastKey);
  FinishKeyWaiting := (LastKey = keyFINISH);
End;


Procedure ReadKeyboard;
Var
  Erc: Word;
Begin
  LastKey := 0;
  While (Not (Ord(LastKey) in [keyCANCEL,keyGO,keyFINISH,keyF1])) Do
    Erc := ReadKbdDirect(0,Ads LastKey);
End;


Begin {Main}
  Writeln('BarCodeService Utilities 1.06');
  Writeln('A service of United Computer Sales & Service, Inc.');
  Writeln;

  Erc := RgParam(0,1,Ads Sd);
  If (Erc = ercOk) Then
    Begin
      MoveSL(Sd.StrAddr,Ads CommCase[1],Sd.Leng);
      CommCase[0] := Chr(Ord(Sd.Leng));
    End
  Else
    CommCase := NULL;

{-----------------}
{ Invalid Command }
{-----------------}
  If (CommCase = NULL) Then
    Begin
      Writeln('-Command error: No case specified. Re-load software.');
      Exit;
    End;

{-----------------}
{ Close Comm Port }
{-----------------}
  If (CommCase = 'CL') Then
   Begin
     CheckErc(BarCodeRequest(251, Ads NULL,0, Ads NULL,0, Ads NULL,0));
     Writeln('Bar Code Service communications port was closed.');
     Exit;
   End;


{-----------}
{ Deinstall }
{-----------}
  If (CommCase = 'DE') Then
    Begin
      Erc := GetPartitionHandle(Ads 'BarCode', 7, Ads UserNum);
      If (Erc = ercNoSuchPartition) Then
        Begin
          Writeln('BarCode Service not installed!');
          Exit;
        End
      Else
        Begin
          Write('Terminating ... ');
          {Issue terminate request}
          CheckErc(BarCodeRequest(255,Ads NULL,0,Ads Null,0,Ads userNum,2));
          Erc := VacatePartition(userNum);
          Erc := RemovePartition(userNum);
          Writeln('done.');
        End;
      Exit;
    End;

{-------------}
{ Get Version }
{-------------}
  If (CommCase = 'GV') Then
    Begin
      CheckErc(BarCodeRequest(240,
                              Ads NULL,
                              0,
                              Ads GenStr[1],
                              64,
                              Ads GenWrd,
                              2));
      GenStr[0]:=Chr(GenWrd);
      Writeln('-> Version is: "',GenStr,'"');
      {Get Primary Request Code}
      CheckErc(BarCodeRequest(241,
                              Ads NULL,
                              0,
                              Ads NULL,
                              0,
                              Ads GenWrd,
                              2));
       HexStr := ConvertToHex(GenWrd);
       Write  ('-> Primary request code is hex ',HexStr);
       Writeln(' (Decimal ',GenWrd:-5,')');
       Exit;
    End;

{----------------}
{ Query Last Erc }
{----------------}
  If (CommCase = 'QE') Then
   Begin
     CheckErc(BarCodeRequest(242, Ads NULL,0, Ads NULL,0, Ads GenWrd,2));
     Writeln('Last error code from service was ',GenWrd:-5);
     Exit;
   End;


{---------------}
{ Reset Service }
{---------------}
  If (CommCase = 'RS') Then
   Begin
     CheckErc(BarCodeRequest(245, Ads NULL,0, Ads NULL,0, Ads NULL,0));
     Writeln('Bar Code Service was reset.');
     Exit;
   End;

{-----------}
{ Read Test }
{-----------}
  If (CommCase = 'RT') Then
   Begin
     CfgFile := NULL;

     If (CfgFile = NULL) Then
       Begin
         Writeln('Do you want to use [Comm]A&[Sys]<Sys>MuBar?');
         Writeln('  Press <GO> for yes, <CANCEL> for no, FINISH to quit.');
         Writeln;
         ReadKeyboard;
         If (LastKey = keyFINISH) Then Exit;
         If (LastKey = keyGO) Then CfgFile := '[Comm]A&[Sys]<Sys>MuBar';
       End;

     If (CfgFile = NULL) Then
       Begin
         Writeln('Do you want to use [Comm]B&[Sys]<Sys>MuBar?');
         Writeln('  Press <GO> for yes, <CANCEL> for no, FINISH to quit.');
         Writeln;
         ReadKeyboard;
         If (LastKey = keyFINISH) Then Exit;
         If (LastKey = keyGO) Then CfgFile := '[Comm]B&[Sys]<Sys>MuBar';
       End;

     If (CfgFile = NULL) Then
       Begin
         Write('Enter the config file to use: ');
         Readln(CfgFile);
         If (CfgFile = NULL) Then Exit;
       End;

     Writeln(Chr(255)*'X'*Chr(1));
     Writeln(Chr(255)*'PF'*Chr(12));
     Writeln(Chr(255)*'X'*Chr(0));
     Writeln(Chr(255)*'PF'*Chr(12));

     Write('Opening ',CfgFile,' ... ');
     CheckErc(BarCodeRequest(250,
                             Ads CfgFile[1], 
                             CfgFile.Len,
                             Ads NULL,
                             0,
                             Ads NULL,
                             0));
     Writeln('done.');
     Writeln;

     Writeln(Chr(255)*'X'*Chr(1));
     Writeln;
     Writeln('BarCode Utilities Read Test - Press <FINISH> to quit.');
     Writeln(Chr(255)*'X'*Chr(0));

     fCont := FALSE;
     While (TRUE) Do
       Begin
         Write('Waiting for BarCode Data . . . ');
         BufLen := 0;
         Erc    := ercDeviceNotReady;
         While (Erc = ercDeviceNotReady) Do
           Begin
             If (FinishKeyWaiting = TRUE) Then Exit;
             Erc := BarCodeRequest(10,
                                   Ads Null, 
                                   0,
                                   Ads Buffer,
                                   14,
                                   Ads BufLen,
                                   2);
           End;
         If (Erc <> ercOK) Then 
           Writeln('done. CTOS Error # ',Erc:4);

         If (Erc = ercOK) Then
           Begin
             Write('"');
             For I:=1 to Ord(BufLen) Do
               Write(Chr(Buffer.Wa[I]));
             Writeln('"');
             If (NOT fCont) Then
               Begin
                 Writeln('  Press <GO> to resume, <FINISH> to quit, <F1> for Non-Stop.');
                 Writeln;
                 ReadKeyboard;
                 If (LastKey = keyFINISH) Then Exit;
                 fCont := (LastKey = keyF1);
               End;
           End;
         Writeln;
       End;
   End;

{-----------------}
{ Unknown Command }
{-----------------}
  Writeln('-Error: Unknown command case "',CommCase,'"');

End.
