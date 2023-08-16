#ifndef  LB_GEMDDACH_HPP
#define  LB_GEMDDACH_HPP

#include <lbport.h>
#include <lbfio.hpp>
#include <gemcntrl.hpp>

//--- Index Values - File Header
#define GEMDDACH_FH_PRIORCD     1
#define GEMDDACH_FH_DESTID      2
#define GEMDDACH_FH_ORIGID      3
#define GEMDDACH_FH_TRANSDATE   4
#define GEMDDACH_FH_TRANSTIME   5
#define GEMDDACH_FH_FILEIDMOD   6
#define GEMDDACH_FH_RECSIZE     7
#define GEMDDACH_FH_BLOCKFACT   8
#define GEMDDACH_FH_FORMATCD    9
#define GEMDDACH_FH_DESTNAME    10
#define GEMDDACH_FH_ORIGNAME    11
#define GEMDDACH_FH_REFCD       12

//--- Index Values - Batch Header
#define GEMDDACH_BH_SVCCLS      1
#define GEMDDACH_BH_ORIGNAME    2
#define GEMDDACH_BH_ORIGID      3
#define GEMDDACH_BH_CLSCD       4
#define GEMDDACH_BH_DESCENTRY   5
#define GEMDDACH_BH_DESCDATE    6
#define GEMDDACH_BH_EFFDATE     7
#define GEMDDACH_BH_DESTID      8
#define GEMDDACH_BH_BATCH       9

//--- Index Values - Detail Entry
#define GEMDDACH_DT_TRANSCD     1
#define GEMDDACH_DT_BANKID      2
#define GEMDDACH_DT_BANKACCTNUM 3
#define GEMDDACH_DT_NET         4
#define GEMDDACH_DT_EMPNUM      5
#define GEMDDACH_DT_EMPNAME     6
#define GEMDDACH_DT_DISCRINFO   7
#define GEMDDACH_DT_ADDENDA     8
#define GEMDDACH_DT_TRACE       9

//--- Index Values - Batch Trailer
#define GEMDDACH_BT_SVCCLS      1
#define GEMDDACH_BT_NUMDET      2
#define GEMDDACH_BT_HASH        3
#define GEMDDACH_BT_TOTDB       4
#define GEMDDACH_BT_TOTCR       5
#define GEMDDACH_BT_ORIGID      6
#define GEMDDACH_BT_DESTID      7
#define GEMDDACH_BT_BATCH       8

//--- Index Values - File Trailer
#define GEMDDACH_FT_BATCHCNT    1
#define GEMDDACH_FT_BLOCKCNT    2
#define GEMDDACH_FT_NUMDET      3
#define GEMDDACH_FT_HASH        4
#define GEMDDACH_FT_TOTDB       5
#define GEMDDACH_FT_TOTCR       6

//--- Index Values - File Information
#define GEMDDACH_FI_PATH        1
#define GEMDDACH_FI_DATE        2
#define GEMDDACH_FI_TIME        3
#define GEMDDACH_FI_SIZE        4


class GemDDACH_T
  {
  public:
        GemDDACH_T();

        ~GemDDACH_T();

        LB_Bool_T   Open            (LB_Word_T pa_OrgID);
        LB_Bool_T   Create          (LB_Word_T pa_OrgID);
        void        Close           ();

        LB_Bool_T   GetBHInfo       (LB_Word_T    pa_Index, LB_String_T& pa_Buf);
        LB_Bool_T   GetBTInfo       (LB_Word_T    pa_Index, LB_String_T& pa_Buf);
        LB_Bool_T   GetDTInfo       (LB_Word_T    pa_Index, LB_String_T& pa_Buf);
        LB_Bool_T   GetFHInfo       (LB_Word_T    pa_Index, LB_String_T& pa_Buf);
        LB_Bool_T   GetFIInfo       (LB_Word_T    pa_Index, LB_String_T& pa_Buf);
        LB_Bool_T   GetFTInfo       (LB_Word_T    pa_Index, LB_String_T& pa_Buf);

        LB_Bool_T   SetBHInfo       (LB_Word_T    pa_Index, LB_String_T& pa_Buf);
        LB_Bool_T   SetBTInfo       (LB_Word_T    pa_Index, LB_String_T& pa_Buf);
        LB_Bool_T   SetDTInfo       (LB_Word_T    pa_Index, LB_String_T& pa_Buf);
        LB_Bool_T   SetFHInfo       (LB_Word_T    pa_Index, LB_String_T& pa_Buf);
        LB_Bool_T   SetFTInfo       (LB_Word_T    pa_Index, LB_String_T& pa_Buf);

        LB_Bool_T   ReadFH          ();
        LB_Bool_T   ReadBH          ();
        LB_Bool_T   ReadDT          (LB_Word_T *pa_NextEntry, LB_Bool_T pa_MoveNext);
        LB_Bool_T   ReadBT          ();
        LB_Bool_T   ReadFT          ();
        LB_Bool_T   ReadFI          ();

        LB_Bool_T   WriteFH         ();
        LB_Bool_T   WriteBH         ();
        LB_Bool_T   WriteDT         ();
        LB_Bool_T   WriteBT         ();
        LB_Bool_T   WriteFT         ();
        LB_Bool_T   WriteFiller     ();

  private:
        //--- Variables
        LB_Word_T           m_OrgID;
        LB_Fio_T            m_File;
        GemControl_T        m_GemCtl;

        union ACH_INFO_T
          {
          //------------------------------------------------------------------
          //
          //--- The following is from the tables 3.1.1 - 3.1.10 of the
          //--- 1992 ACH Rules book, Appendix Three, pages OR58-OR68.
          //---
          //--- Alpha   fields must be left-justified and blank-filled.
          //--- Numeric fields must be right-justified, unsigned, zero-filled.
          //---
          //--- The trailing CR/LF is for United historical purposes only
          //--- and should be phased out at some point.
          //
          //--- Automated Deposit Transaction Codes:
          //        22 - Credit Checking
          //        23 - Credit Checking,   Prenotification
          //        32 - Credit Savings
          //        33 - Credit Savings,    Prenotification
          //        27 - Debit Checking
          //        28 - Debit Checking,    Prenotification
          //        37 - Debit Savings
          //        38 - Debit Savings,     Prenotification
          //
          //------------------------------------------------------------------
          struct FILE_HEADER_T
            {
            LB_Byte_T       RecType;                // '1'
            LB_Byte_T       PriorCD[2];             // Numeric
            LB_Byte_T       DestID[10];             // bTTTTAAAAC
            LB_Byte_T       OrigID[10];             // bTTTTAAAAC
            LB_Byte_T       TransDate[6];           // YYMMDD
            LB_Byte_T       TransTime[4];           // HHMM
            LB_Byte_T       FileIDMod;              // A-Z,0-9
            LB_Byte_T       RecSize[3];             // '094'
            LB_Byte_T       BlockFact[2];           // '10'
            LB_Byte_T       FormatCD;               // '1'
            LB_Byte_T       DestName[23];           // Alphanumeric
            LB_Byte_T       OrigName[23];           // Alphanumeric
            LB_Byte_T       RefCD[8];               // Alphanumeric
            LB_Byte_T       Cr;
            LB_Byte_T       DeleteFlag;
            } FH;

          struct BATCH_HEADER_T
            {
            LB_Byte_T       RecType;                // '5'
            LB_Byte_T       SvcCls[3];              // Numeric
            LB_Byte_T       OrigName[16];           // Alphanumeric
            LB_Byte_T       OrigDesc[20];           // Alphanumeric
            LB_Byte_T       OrigID[10];             // Alphanumeric
            LB_Byte_T       ClsCD[3];               // Alphanumeric
            LB_Byte_T       DescEntry[10];          // Alphanumeric
            LB_Byte_T       DescDate[6];            // YYMMDD
            LB_Byte_T       EffDate[6];             // YYMMDD
            LB_Byte_T       SettleDate[3];          // Julian, Numeric
            LB_Byte_T       OrigStatusCode;         // Alphanumeric
            LB_Byte_T       DestID[8];              // TTTTAAAA
            LB_Byte_T       Batch[7];               // Numeric
            LB_Byte_T       Cr;
            LB_Byte_T       DeleteFlag;
            } BH;

          struct ENTRY_DETAIL_T
            {
            LB_Byte_T       RecType;                // '6'
            LB_Byte_T       TransCD[2];             // Numeric
            LB_Byte_T       BankID[9];              // TTTTAAAAC
            LB_Byte_T       BankAcctNum[17];        // Alphanumeric
            LB_Byte_T       Net[10];                // $$$$$$$$cc
            LB_Byte_T       EmpNum[15];             // Alphanumeric
            LB_Byte_T       EmpName[22];            // Alphanumeric
            LB_Byte_T       DiscrInfo[2];           // Alphanumeric
            LB_Byte_T       Addenda;                // Numeric
            LB_Byte_T       Trace[15];              // Numeric
            LB_Byte_T       Cr;
            LB_Byte_T       DeleteFlag;
            } DT;

          struct BATCH_TRAILER_T
            {
            LB_Byte_T       RecType;                // '8'
            LB_Byte_T       SvcCls[3];              // Numeric
            LB_Byte_T       NumDet[6];              // Numeric
            LB_Byte_T       HashValue[10];          // Numeric
            LB_Byte_T       TotDB[12];              // $$$$$$$$$$cc
            LB_Byte_T       TotCR[12];              // $$$$$$$$$$cc
            LB_Byte_T       OrigID[10];             // Alphanumeric
            LB_Byte_T       Filler[25];             // Alphanumeric
            LB_Byte_T       DestID[8];              // TTTTAAAA
            LB_Byte_T       Batch[7];               // Numeric
            LB_Byte_T       Cr;
            LB_Byte_T       DeleteFlag;
            } BT;

          struct FILE_TRAILER_T
            {
            LB_Byte_T       RecType;                // '9'
            LB_Byte_T       BatchCnt[6];            // Numeric
            LB_Byte_T       BlockCnt[6];            // Numeric
            LB_Byte_T       NumDet[8];              // Numeric
            LB_Byte_T       HashValue[10];          // Numeric
            LB_Byte_T       TotDB[12];              // $$$$$$$$$$cc
            LB_Byte_T       TotCR[12];              // $$$$$$$$$$cc
            LB_Byte_T       Filler[39];
            LB_Byte_T       Cr;
            LB_Byte_T       DeleteFlag;
            } FT;

          } m_AchInfo;

        //--- Functions
        void                m_ClearRecord();
  };
#endif
