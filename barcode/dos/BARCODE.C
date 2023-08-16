#include <dos.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>


/****************************************************************************
                        Storage Type Definitions
****************************************************************************/
#define BYTE           unsigned char
#define WORD           unsigned int
#define DWORD          unsigned long
#define ADSMEM         void far *
#define ADSCHAR        char far *
#define ADSBYTE        BYTE far *
#define ADSWORD        WORD far *


/****************************************************************************
                        Function Call Defintions
****************************************************************************/
#define fnBarCodeRequest               57600
#define fnRESETSERVER                  65532
#define fnGETVERNUM                    65533
#define fnGETSTATUS                    65534
#define fnTERMINATE                    65535

#define ercNoSuchRequestCode              31
#define ercDeviceNotReady                300


/*******************************************************************************
                    MISCELLANEOUS DEFINES
*******************************************************************************/
#define DEFBCINT                       0x61
#define DEFDCINT                       0x60
#define DEFBAUD                        7
#define DEFPARITY                      2
#define DEFDATABITS                    2
#define DEFPORT                        1
#define DEFSTOPBITS                    0
#define VERSTRING                      "BarCode Communications Server"
#define VERMAJOR                       1
#define VERMINOR                       00
#define TRUE                           0xFF
#define FALSE                          0x00
#define CARRYFLAG                      1       /* DOS Flag                 */
#define ZEROFLAG                       64      /* DOS Flag                 */
#define WORDSIZE                       sizeof(WORD) * 8
#define qRCVSIZE                       256
#define qSNDSIZE                       256
#define ercOK                          0x00


/******************************************************************************
*
*                  DOS STRUCTURE StackFrameStruct
*
******************************************************************************/
struct StackFrameStruct
{
    ADSMEM                      Param1;
    ADSMEM                      Param2;
    ADSMEM                      Param3;
    ADSMEM                      Param4;
    ADSMEM                      Param5;
    ADSMEM                      Param6;
    ADSMEM                      Param7;
    ADSMEM                      Param8;
    ADSMEM                      Param9;
    ADSMEM                      Param10;
    ADSMEM                      Param11;
    ADSMEM                      Param12;
    ADSMEM                      Param13;
    ADSMEM                      Param14;
    ADSMEM                      Param15;
    ADSMEM                      Param16;
    ADSMEM                      Param17;
    ADSMEM                      Param18;
    ADSMEM                      Param19;
    ADSMEM                      Param20;
} ;


/*******************************************************************************
                   External Variable Settings
*******************************************************************************/
extern  WORD    _stklen             = 512;
extern  WORD    _heaplen            = 1;
extern  int     directvideo         = 0xFF;            /* 0=BIOS Output  */


/*******************************************************************************
                    Global Variable Settings
*******************************************************************************/
static  BYTE    gfHelp              = FALSE;            /* FLAG - Display Help            */
static  BYTE    gfRemove            = FALSE;            /* FLAG - Remove Server           */
static  BYTE    gfReset             = FALSE;            /* FLAG - Reset Server            */
static  BYTE    gfTest              = FALSE;            /* FLAG - Test server             */
static  BYTE    gBCIntNum           = DEFBCINT;
static  BYTE    gDCIntNum           = DEFDCINT;         /* Interrupt Number               */
static  BYTE    gPortNo             = DEFPORT;
static  BYTE    gBaud               = DEFBAUD;
static  BYTE    gParity             = DEFPARITY;
static  BYTE    gDataBits           = DEFDATABITS;
static  BYTE    gStopBits           = DEFSTOPBITS;
static  BYTE    gBarBufTemp[32];
static  BYTE    gfPortOpen          = FALSE;
static  WORD    gBarBufSize         = 0;
static  WORD    gPspSeg             = 0;                /* Save PSP Segment for Release   */
static  DWORD   gMemSize            = 0;                /* Size of server in memory       */
static  DWORD   gNbrRqServed        = 0;                /* Number of Requests Served      */
static  ADSWORD gEnvSeg;                                /* Save Env Segment for Release   */
static  ADSWORD gCritFlag;
static  void    interrupt  (*old_bc_int_func)  ();

static  BYTE    gDCBuffer[qRCVSIZE + 2 +
                          qSNDSIZE + 2];

        char    BaudRateTable[9][6]   =
   { "110", "150",  "300", "600", "1200", "2400", "4800", "9600", "19200" };

        char    ParityTable[5][6]     = {"No","Odd","Even","Mark","Space"};

        char    DataBitsTable[4][2]   = {"5","6","7","8"};

        char    StopBitsTable[2][2]   = {"1","2"};

static union     REGS    r;
static struct    SREGS   s;

char t1[64];
char t2[64];


void BIOS_PosCursor(WORD iLine, WORD iCol)
{
    _AH = 0x02;         /* Function Number          */
    _BH = 0;            /* BIOS Video Page          */
    _DH = iLine;        /* Starting Line            */
    _DL = iCol;         /* Starting Column          */
    geninterrupt(0x10); /* General BIOS Interrupt   */
}


void BIOS_WriteChar(BYTE ch, BYTE attr, WORD len)
{
    _AH = 0x09;         /* Function Number          */
    _BH = 0;            /* BIOS Video Page          */
    _AL = ch;           /* Character to Write       */
    _BL = attr;         /* Attribute Byte           */
    _CX = len;          /* Repeat Counter           */
    geninterrupt(0x10); /* General BIOS Interrupt   */
}


void BIOS_WriteString(WORD iLine, WORD iCol, ADSCHAR pString, WORD attr)
{
    for (; *pString != '\0'; pString++)
      {
      BIOS_PosCursor(iLine,iCol);
      BIOS_WriteChar((BYTE) *pString,attr,1);
      iCol ++;
      if (iCol > 79)
        {
        iLine ++;
        iCol = 0;
        }
      }
}


void SayLocus(char * pString)
{
    BIOS_WriteString(29,0,(ADSCHAR) "                                  ",0x07);
    BIOS_WriteString(29,0,(ADSCHAR) pString,0x07);
}


/*/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
//                     DATA COMMUNICATION SECTION
//
///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////*/

/*****************************************************************************
---------------   -------------------------------   --------    ---------
  BAUD RATE       PARITY TABLE                      DATABITS    STOP BITS
---------------   -------------------------------   ---------   ---------
  110 - 0         None                        - 0   5 - 0       0 - 1
  150 - 1         Odd                         - 1   6 - 1       1 - 2
  300 - 2         Even                        - 2   7 - 2
  600 - 3         Stick Parity Odd(Mark)      - 3   8 - 3
 1200 - 4         Stick Parity Even (Space)   - 4
 2400 - 5
 4800 - 6
 9600 - 7
19200 - 8
*****************************************************************************/
WORD DC_ClosePort(void)
{
  _AL = 1;                                   /* Close Port Function  */
  _AH = gPortNo;                             /* Com Port #           */
  geninterrupt(DEFDCINT);
  gCritFlag = (ADSMEM) NULL;

  return ((WORD) r.h.al);
}


WORD DC_OpenPort(void)
{
  _DL = 0;                                   /* Interrupt Level      */
  _DH = 0;                                   /* Reserved             */
  _ES = 0;                                   /* Reserved             */
  _DI = 0;                                   /* I/O Port Address     */
  _DS = FP_SEG((ADSCHAR) &gDCBuffer[0]);     /* I/O Buffer Address   */
  _SI = FP_OFF((ADSCHAR) &gDCBuffer[0]);     /* I/O Buffer Address   */
  _AL = 9;                                   /* Open Port Function   */
  _AH = gPortNo;                             /* Com Port #           */
  _BX = (WORD) qRCVSIZE;                     /* Size of Input Queue  */
  _CX = (WORD) qSNDSIZE;                     /* Size of Output Queue */
  geninterrupt(DEFDCINT);
/*
  if (r.h.al == 0)
    {
    r.h.al = 6;
    r.h.ah = gPortNo;
    r.x.bx = 4;
    int86x(gDCIntNum,&r,&r,&s);
    gCritFlag = (ADSWORD) MK_FP(s.es,r.x.bx);
    }
*/
  return((WORD) _AL);
}


WORD DC_QueueSize(void)
{
/*
  if (*gCritFlag != 0)
    {
    SayLocus("DataComm Server Busy");
    return (0);
    }
  else
    SayLocus("DataCommServer NOT BUSY");
  delay(300);
*/
  _AL = 3;                                   /* Read Port Function   */
  _AH = gPortNo;                             /* Com Port #           */
  _BX = 0;                                   /* Terminal Character   */
  _CX = 0;                                   /* Read Counter         */
  geninterrupt(DEFDCINT);
/*
  if (r.h.al == 0)
    {
    SayLocus("NO DATACOMM ERROR");
    delay(300);
    if (r.x.bx <= 0)
      SayLocus("Q is <= ZERO");
    else
      SayLocus("Q is > ZERO");
    delay(300);
    return (r.x.bx);
    }
  else
    {
    SayLocus("DATACOMM ERROR");
    delay(300);
    return (r.h.al);
    }
*/
    return ((WORD) _BX);
}


WORD DC_ReadPort(ADSBYTE ch)
{

  if (*gCritFlag != 0)
    return (ercDeviceNotReady);
  r.h.al = 3;                                   /* Read Port Function   */
  r.h.ah = gPortNo;                             /* Com Port #           */
  r.h.bl = 0;                                   /* Terminal Character   */
  r.h.bh = 1;                                   /* Read Byte Option     */
  int86x(gDCIntNum,&r,&r,&s);
  if (r.h.al == 0)
    *ch = r.h.ah;                               /* Character Read       */
  else
    *ch = 0;

  return ((WORD) r.h.al);
}


WORD DC_ReadString(ADSCHAR pString, WORD wReadMax)
{
/*
  if (*gCritFlag != 0)
    return (ercDeviceNotReady);
*/

  _DS = FP_SEG(pString);                     /* Address of Transfer  */
  _SI = FP_OFF(pString);                     /* Address of Transfer  */
  _AL = 3;                                   /* Read Port Function   */
  _AH = gPortNo;                             /* Com Port #           */
  _BL = 0;                                   /* Terminal Character   */
  _BH = 0;                                   /* Read String Option   */
  _CX = wReadMax;                            /* Max Chars to Read    */
  geninterrupt(DEFDCINT);
  return ((WORD) _AL);
}


WORD DC_SetOptions(void)
{
/*
  if (*gCritFlag != 0)
    return (ercDeviceNotReady);
*/
  /*************/
  /* Baud Rate */
  /*************/
  _AL = 4;
  _AH = gPortNo;
  _BL = 1;
  _BH = gBaud;
  geninterrupt(DEFDCINT);
  if (_AL != 0)
    return ((WORD) _AL);

  /**********/
  /* Parity */
  /*********/
  _AL = 4;
  _AH = gPortNo;
  _BL = 2;
  _BH = gParity;
  geninterrupt(DEFDCINT);
  if (_AL != 0)
    return ((WORD) _AL);

  /************/
  /* DataBits */
  /************/
  _AL = 4;
  _AH = gPortNo;
  _BL = 3;
  _BH = gDataBits;
  geninterrupt(DEFDCINT);
  if (_AL != 0)
    return ((WORD) _AL);

  /************/
  /* StopBits */
  /************/
  _AL = 4;
  _AH = gPortNo;
  _BL = 4;
  _BH = gStopBits;
  geninterrupt(DEFDCINT);
  if (_AL != 0)
    return ((WORD) _AL);

  return (ercOK);
}


void DC_DrainInputQueue(void)
{
    WORD erc;
    BYTE ch;

    for (erc=0; (erc == 0); )
      erc = DC_ReadPort((ADSBYTE) &ch);
}


/*/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
//                     MS-DOS TSR SECTION
//
///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////*/


/****************************************************************************
*
*   Convert Word Function
*
****************************************************************************/
WORD CW(WORD w)
{
  return (w << 8) | (w >> WORDSIZE - 8);  /* rol */
}


/****************************************************************************
*
*   Memory Copy Function
*
****************************************************************************/
void memcopy(ADSCHAR Source, ADSCHAR Dest, WORD count)
{
    register int i;

    for (i=count; i > 0; i--)
      *Dest++ = *Source++;
}


/****************************************************************************
*
*   Function to Free a DOS Segment, specifically: the DOS environment space
*   for the server.
*
****************************************************************************/
WORD DOSFreeSegment(WORD SegToFree)
{
  _ES = SegToFree;
  _AH = 0x49;
  geninterrupt(0x21);
  if (_FLAGS & CARRYFLAG)
    return (_AX);
  else
    return (ercOK);
}

/****************************************************************************
*
*   Function to Test the Server
*
*
****************************************************************************/
void TestServer(void)
{
  union     REGS    r;
  struct    SREGS   s;
  WORD erc,q;
  int  i;

  printf("\nOpening port ... ");
  erc = DC_ClosePort();
  erc = DC_OpenPort();
  if (erc != ercOK)
    {
    printf("\nError #%u opening port #%u",erc,gPortNo);
    exit(EXIT_FAILURE);
    }
  printf("setting options ... ");
  erc = DC_SetOptions();
  if (erc != ercOK)
    {
    printf("\nError #%u setting options on port #%u",erc,gPortNo);
    exit(EXIT_FAILURE);
    }
  printf("done.");
  for (; !kbhit(); )
    {
    q = DC_QueueSize();
    printf("\nQueueSize is %u bytes in size.",q);
    if (q >= 14)
      {
      DC_ReadString((ADSMEM) gBarBufTemp,14);
      printf("\nRead BarCode Data: [");
      for (i=0; i < 14; i++)
        printf("%c",gBarBufTemp[i]);
      printf("]\n");
      for (; !kbhit(); ) ;
      if (getch() == 0);
      }
    }
  if (getch() == 0);
  erc = DC_ClosePort();
  printf("\n\n-Server test completed.\n");
}



/****************************************************************************
*
*   Function to Reset the Server
*
****************************************************************************/
void ResetServer(void)
{
  union     REGS    r;
  struct    SREGS   s;

  r.x.ax = fnRESETSERVER;
  int86x(gBCIntNum,&r,&r,&s);

  if (r.x.ax == ercOK)
    printf("\n-Server reset.");
  else
    printf("\n-Error #%u resetting server.",r.x.ax);
}


/****************************************************************************
*
*   Function to Terminate the Server
*
****************************************************************************/
void TerminateServer(void)
{
  union     REGS    r;
  struct    SREGS   s;
  WORD              erc;

  r.x.ax = fnTERMINATE;
  int86x(gBCIntNum,&r,&r,&s);

  erc = DOSFreeSegment(r.x.bx);
  if (erc != ercOK)
    {
    printf("\n-ERROR:");
    printf(" Could not free memory. (MS-DOS ERC %u)\n",erc);
    }
  else
    {
    gMemSize        = ((s.ds   * 256L) + r.x.dx) * 16L;
    gNbrRqServed    = ((r.x.ax * 256L) + r.x.cx);
    printf("\n-Server unloaded. (Responded to %lu requests, %lu bytes released)",gNbrRqServed,gMemSize);
    }
}


/****************************************************************************
*
*   Function to Determine If An Interrupt Is Already In Use
*
****************************************************************************/
WORD InterruptIsInUse(void)
{
  if (FP_SEG(old_bc_int_func) + (FP_OFF(old_bc_int_func) / 16) == 0)
    return(FALSE);
  else
    return (TRUE);
}


/****************************************************************************
*
*   Function to Ask Server for Its Version Number
*
****************************************************************************/
void GetServerVersion(ADSBYTE major, ADSBYTE minor)
{
  union     REGS    r;
  struct    SREGS   s;

  r.x.ax = fnGETVERNUM;
  int86x(gBCIntNum,&r,&r,&s);

  *major = r.h.bh;
  *minor = r.h.bl;
}


/****************************************************************************
*
*   Function to Determine If A Copy of the Server is Already in Memory
*
****************************************************************************/
WORD IsServerInstalled(void)
{
  BYTE major,minor;
  WORD erc;

  if (InterruptIsInUse() == FALSE)
    return(FALSE);
  else
    {
    GetServerVersion((ADSBYTE) &major, (ADSBYTE) &minor);
    if ((major == VERMAJOR) && (minor == VERMINOR))
      return(TRUE);
    else
      return(FALSE);
    }
}


#pragma argsused
WORD BarCodeRequest(WORD    wFunctionNumber,
                    ADSMEM  pInpData,
                    WORD    sInpData,
                    ADSMEM  pOutData,
                    WORD    sOutData,
                    ADSWORD pOutSize,
                    WORD    sOutSize)
{
    WORD erc;
    WORD q;

    erc = ercOK;

    switch (wFunctionNumber)
      {
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 9: if (gBarBufSize <= 0)
                {
                if (DC_QueueSize() >= sOutData)
                  DC_ReadString((ADSCHAR) &gBarBufTemp[0],sOutData);
                else
                  SayLocus("Q size <> 14");
                }

              if (gBarBufSize > 0)
                {
                if ((*gBarBufTemp >= '0') || (wFunctionNumber == 9))
                  {
                  memcopy((ADSCHAR) gBarBufTemp,pOutData,gBarBufSize);
                  *pOutSize   = gBarBufSize;
                  gBarBufSize = 0;
                  SayLocus("Bill Read!");
                  }
                else
                  {
                  *pOutSize = 0;
                  erc = ercDeviceNotReady;
                  }
                }
              else
                {
                *pOutSize = 0;
                erc = ercDeviceNotReady;
                }
              break;

      case 250: SayLocus("BarCode Request: Open Port");
                if (gfPortOpen == TRUE)
                  if ( DC_ClosePort() );

                SayLocus("Opening port");
                erc = DC_OpenPort();
                if (erc == ercOK)
                  {
                  SayLocus("Setting options");
                  erc = DC_SetOptions();
                  }
                if (erc == ercOK)
                  {
                  SayLocus("PORT OPENED");
                  gfPortOpen = TRUE;
                  }
                else
                  {
                  SayLocus("Port NOT Opened");
                  gfPortOpen = FALSE;
                  }
                break;

      case 251: SayLocus("BarCode Request: Close Port");
                if (gfPortOpen == TRUE)
                  if ( DC_ClosePort() );
                gfPortOpen = FALSE;
                break;

      default : SayLocus("BarCode Request: Unknown");
                erc = ercDeviceNotReady;
                break;
      }
    return (erc);
}


/****************************************************************************
*
*   Function to Handle Our Interrupt
*
****************************************************************************/
#pragma argsused
void interrupt BarCodeTask(bp, di, si, ds, es, dx, cx,
                           bx, ax, ip, cs, flags)
{
    int       i;
    WORD      SaveAX;
    struct    StackFrameStruct far * SF;

    SF = (struct StackFrameStruct far *) MK_FP(es,si);

    enable();

    gNbrRqServed++ ;

    SaveAX  = ax;

    switch( SaveAX )
      {

      /***************************/
      /*  MS-DOS Misc TSR Calls  */
      /***************************/

      case fnGETVERNUM      :   ax = ercOK;
                                bx = (VERMAJOR << 8) + VERMINOR;
                                break;

      case fnRESETSERVER    :   DC_DrainInputQueue();
                                DC_ClosePort();
                                ax = DC_OpenPort();
                                ax = DC_SetOptions();
                                DC_DrainInputQueue();
                                break;

      case fnTERMINATE      :   bx = gPspSeg;
                                ax = (gNbrRqServed  / 256L);
                                cx = (gNbrRqServed  % 256L);
                                ds = (gMemSize      / 256L);
                                dx = (gMemSize      % 256L);
                                setvect(gBCIntNum,old_bc_int_func);
                                DC_DrainInputQueue();
                                ax = DC_ClosePort();
                                break;


      /**************************/
      /* BarCode Function Calls */
      /**************************/
      case fnBarCodeRequest :   ax = BarCodeRequest(CW(* (ADSWORD) SF->Param2),
                                                         (ADSMEM)  SF->Param3,
                                                    CW(* (ADSWORD) SF->Param4),
                                                         (ADSMEM)  SF->Param5,
                                                    CW(* (ADSWORD) SF->Param6),
                                                         (ADSWORD) SF->Param7,
                                                    CW(* (ADSWORD) SF->Param8));
                                * (ADSWORD) SF->Param1 = CW(ax);
                                break;


      /***************************/
      /*  Invalid Function Call  */
      /***************************/
      default               :   SayLocus("BarCode: No such request code.");
                                ax = ercNoSuchRequestCode;
                                * (ADSWORD) SF->Param1 = CW(ax);
                                break;
      }
}


/*****************************************************************************
*
*                               MAIN
*
*****************************************************************************/
void main(int argc, char * argv[])
{
  int           i;                /* Counter Variable                     */
  int           q;
  char          * lparam;         /* Command Line Parameter               */
  WORD          erc;


  printf("\n%s, version %u.%u",VERSTRING,VERMAJOR,VERMINOR);
  printf("\n  Copyright 1992 by United Computer Sales & Service, Inc.\n");


  gDCIntNum = DEFDCINT;
  gBCIntNum = DEFBCINT;

  /*******************************/
  /* Get Command Line Parameters */
  /*******************************/
  for (i=1; i < argc; i++)
    {
    lparam = argv[i];
    if ((*lparam == '/') || (*lparam == '-'))
      lparam++ ;
    switch (*lparam)
      {
      /**********************/
      /* Baud Rate Setting  */
      /**********************/
      case 'b':
      case 'B': lparam++ ;
                gBaud = atoi(lparam);
                if ((gBaud < 0) || (gBaud > 8))
                  gBaud = DEFBAUD;
                break;

      /**********************/
      /* Serial Port Number */
      /**********************/
      case 'c':
      case 'C': lparam++ ;
                gPortNo = atoi(lparam);
                if ((gPortNo < 1) || (gPortNo > 4))
                  gPortNo = DEFPORT;
                break;

      /**********************/
      /* Data Bits Setting  */
      /**********************/
      case 'd':
      case 'D': lparam++ ;
                gDataBits = atoi(lparam);
                if ((gDataBits < 0) || (gDataBits > 3))
                  gDataBits = DEFDATABITS;
                break;

      /****************************/
      /* BarCode Interrupt Number */
      /****************************/
      case 'i':
      case 'I': lparam++ ;
                gBCIntNum = atoi(lparam);
                if ((gBCIntNum < 0) || (gBCIntNum > 255))
                  gBCIntNum = DEFBCINT;
                break;

      /****************************/
      /* DataComm Interrupt Number */
      /****************************/
      case 'j':
      case 'J': lparam++ ;
                gDCIntNum = atoi(lparam);
                if ((gDCIntNum < 0) || (gDCIntNum > 255))
                  gDCIntNum = DEFDCINT;
                break;

      /**********************/
      /* Parity Setting     */
      /**********************/
      case 'p':
      case 'P': lparam++ ;
                gParity = atoi(lparam);
                if ((gParity < 0) || (gParity > 4))
                  gParity = DEFPARITY;
                break;

      /****************/
      /* Reset Server */
      /****************/
      case 'r':
      case 'R': gfReset = TRUE;
                break;

      /**********************/
      /* Stop Bits Setting  */
      /**********************/
      case 's':
      case 'S': lparam++ ;
                gStopBits = atoi(lparam);
                if ((gStopBits < 0) || (gStopBits > 1))
                  gDataBits = DEFSTOPBITS;
                break;

     /****************/
      /* Test Server */
      /***************/
      case 't':
      case 'T': gfTest = TRUE;
                TestServer();
                exit(EXIT_SUCCESS);
                break;

      /***********************/
      /* Terminate Server    */
      /***********************/
      case 'u':
      case 'U': gfRemove = TRUE;
                break;

      /***********************/
      /* Display Help        */
      /***********************/
      case '?':
      case 'h':
      case 'H': gfHelp = TRUE;
                break;

      default:  printf("\n*Note: ignored unknown parameter \"%s\"\n",lparam);
                break;
      }
    }

  
  old_bc_int_func  = getvect(gBCIntNum);   /* Save Old Vector */


  /**************************/
  /* Show Help Screen       */
  /**************************/
  if (gfHelp == TRUE)
    {
    printf("\nOptional parameters are:");
    printf("\n\t/b              \tGive baud rate (see table below).");
    printf("\n\t/c<portno>      \tSerial port Number (1-4).");
    printf("\n\t/d              \tGive data bits (see table below).");
    printf("\n\t/i<intnum>      \tGive BarCode interrupt number (96-102).");
    printf("\n\t/j<intnum>      \tGive DataComm interrupt number (96-102).");
    printf("\n\t/p              \tGive parity  (see table below).");
    printf("\n\t/r              \tReset server.");
    printf("\n\t/s              \tGive stop bits (see table below).");
    printf("\n\t/u              \tUnload server from memory.");
    printf("\n");
    printf("\nCommunication parameters are numeric, as follows:");
    printf("\n\t BaudRate: ");
      for (i=0; i < 9; i++)
        printf("%u=%s ",i,BaudRateTable[i]);
    printf("\n\t   Parity: ");
      for (i=0; i < 5; i++)
        printf("%u=%s ",i,ParityTable[i]);
    printf("\n\tData Bits: ");
      for (i=0; i < 4; i++)
        printf("%u=%s ",i,DataBitsTable[i]);
    printf("\n\tStop Bits: ");
      for (i=0; i < 2; i++)
        printf("%u=%s ",i,StopBitsTable[i]);
    printf("\n");
    exit(EXIT_SUCCESS);
    }


  /****************/
  /* Reset Server */
  /****************/
  if (gfReset == TRUE)
    if (IsServerInstalled() == TRUE)
      {
      ResetServer();
      exit(EXIT_SUCCESS);
      }
    else
      {
      printf("\n-Server is not installed!\n");
      exit(EXIT_FAILURE);
      }


  /********************/
  /* Terminate Server */
  /********************/
  if (gfRemove == TRUE)
    if (IsServerInstalled() == TRUE)
      {
      TerminateServer();
      exit(EXIT_SUCCESS);
      }
    else
      {
      printf("\n-Server is not installed!\n");
      exit(EXIT_FAILURE);
      }


  /*************************************/
  /* Check If Server Already Installed */
  /*************************************/
  if (IsServerInstalled() == TRUE)
    {
    printf("\n-Server is already installed!");
    printf("\n-Use parameter /u to terminate server, then install.\n");
    exit(EXIT_FAILURE);
    }


  /****************************************/
  /* Check If Interrupt Is Already In Use */
  /****************************************/
  if (InterruptIsInUse() == TRUE)
    {
    printf("\n-Interrupt #%u already in use!",gBCIntNum);
    printf("\n-Use parameter /i to specify another interrupt number.\n");
    exit(EXIT_FAILURE);
    }

  /****************************/
  /* Open Communications Port */
  /****************************/
  setvect(gBCIntNum,BarCodeTask);
  gCritFlag = (ADSMEM) NULL;
/*
  erc = DC_ClosePort();
  erc = DC_OpenPort();
  if (erc != ercOK)
    {
    printf("\nError #%u opening port #%u",erc,gPortNo);
    exit(EXIT_FAILURE);
    }
  erc = DC_SetOptions();
  if (erc != ercOK)
    {
    printf("\nError #%u setting options on port #%u",erc,gPortNo);
    exit(EXIT_FAILURE);
    }
  DC_DrainInputQueue();
*/

  /*******************/
  /* Install Server  */
  /*******************/
  gPspSeg  = _psp;
  gEnvSeg  = (ADSWORD) MK_FP(_psp,0x2C);
  gMemSize = (_SS + ((_SP + _stklen) / 16)) - _psp;

  printf("\n-Server Installed, using %lu bytes of ",(gMemSize * 16L));
  if (_psp > 40000) /* psp segment is greater than 640k */
    printf("high memory");
  else
    printf("conventional memory");
  printf("\n-%s is using interrupt #%u","DataComm",gDCIntNum);
  printf("\n-%s is using interrupt #%u","BarCode ",gBCIntNum);
  printf("\n-Com%u: is set for %s baud, %s parity, %s data bits, %s stop bits",
          gPortNo,
          BaudRateTable[gBaud],
          ParityTable[gParity],
          DataBitsTable[gDataBits],
          StopBitsTable[gStopBits]);
  printf("\n");

  if (DOSFreeSegment(*gEnvSeg) == ercOK)
    keep(0,gMemSize);
  else
    printf("\n-ERROR: Could not free the DOS environment space.\n");
}
