#ifndef  GEMBUDLP_HPP
#define  GEMBUDLP_HPP

#include <lbport.h>
#include <lbstring.hpp>
#include <mftable.hpp>
#include <gemcntrl.hpp>

#define BUDLP_MAX_CKT_LINES     128
#define BUDLP_MAX_STUB_LINES    15
#define BUDLP_MAX_VENADDR       6
#define BUDLP_CHKFILENAME       "budchk"
#define BUDLP_CKTFILENAME       "budcheck.ckt"
#define BUDLP_SETFILENAME       "gbpset.dat"

#define BUDLP_STAT_OK           1
#define BUDLP_STAT_EOC          2
#define BUDLP_STAT_EOF          3

class GemBudLP_T
  {
  public:
        GemBudLP_T();

        ~GemBudLP_T();

        LB_Bool_T               Start();

        void                    Finish();

        LB_Word_T               NextCheck(LB_String_T& pa_ChkNum);

        LB_Word_T               NextPrintLine(LB_String_T& pa_Line);

        LB_Bool_T               CktStoreFileName(LB_String_T& pa_CktFileName);

        LB_Bool_T               CktGetInfo(LB_String_T& pa_FN,
                                           LB_String_T& pa_Name,
                                           LB_String_T& pa_Desc,
                                           LB_String_T& pa_Comments);

  private:
        //--* Private Variables
        GemControl_T                    m_GemCtl;
        LB_Fio_T                        m_CheckFile;

        struct CHECK_TEMPLATE_T
          {
          LB_String_T                   Lines[BUDLP_MAX_CKT_LINES];
          LB_Word_T                     LineCnt;
          LB_Word_T                     LineCur;
          }
          m_Template;

        struct CHECK_STUB_LINES_T
          {
          LB_String_T                   Description;
          LB_String_T                   Date;
          LB_String_T                   Invoice;
          LB_String_T                   AmtGross;
          LB_String_T                   AmtDisc;
          LB_String_T                   AmtNet;
          };

        struct CHECK_STUB_T
          {
          LB_String_T                   VenNumber;
          LB_String_T                   VenName;
          struct CHECK_STUB_LINES_T     Lines[BUDLP_MAX_STUB_LINES];
          LB_Word_T                     LineCur;
          LB_String_T                   TotalGross;
          LB_String_T                   TotalDisc;
          LB_String_T                   TotalNet;
          };

        struct CHECK_BODY_T
          {
          LB_String_T                   Date;
          LB_String_T                   Number;
          LB_String_T                   Amount;
          LB_String_T                   SpellAmt[2];
          LB_String_T                   VenName;
          LB_String_T                   VenAddr[BUDLP_MAX_VENADDR];
          LB_Word_T                     VenAddrLine;
          };

        struct CHECK_T
          {
          struct CHECK_STUB_T           Stub;
          struct CHECK_BODY_T           Body;
          }
          m_Check;

        struct CKTINFO_T
          {
          LB_String_T                   FileName;
          LB_String_T                   Name;
          LB_String_T                   Desc;
          LB_String_T                   Comments;
          }
          m_CktInfo;

        struct FILE_CKTINFO_T
          {
          LB_Byte_T                     CktFileName[128];
          LB_Byte_T                     Cr;
          LB_Byte_T                     DeleteFlag;
          }
          m_CktFileInfo;

        //--- Private Methods
        void                            m_Clear();
        void                            m_ClearCheck();
        void                            m_ClearTemplate();
  };
#endif
