#ifndef  GEMPAYLP_HPP
#define  GEMPAYLP_HPP

#include <lbport.h>
#include <lbstring.hpp>
#include <mftable.hpp>
#include <xbtable.hpp>
#include <gemcntrl.hpp>

#define PAYLP_MAX_CKT_LINES     256
#define PAYLP_MAX_EARN_LINES     12
#define PAYLP_MAX_DED_LINES      20
#define PAYLP_MAX_ADDR_LINES      5
#define PAYLP_MAX_1099_PER_PAGE   3  
#define PAYLP_MAX_W2_PER_PAGE	  4
  
#define PAYLP_CHKFILENAME       "paychk"
#define PAYLP_NODFILENAME       "paynod"
#define PAYLP_W2FILENAME        "payw2"
#define PAYLP_1099FILENAME      "pay199"
#define PAYLP_CKTFILENAME       "paycheck.ckt"
#define PAYLP_SETFILENAME       "payset.dat"

#define PAYLP_STAT_OK           1
#define PAYLP_STAT_EOC          2
#define PAYLP_STAT_EOF          3


class GemPayLP_T
{
    public:
        GemPayLP_T();

        ~GemPayLP_T();

        LB_Bool_T       PrtGenStart();
        void            PrtGenFinish();
        LB_Word_T       PrtGenLoadNextItem(LB_String_T& pa_DisplayInfo);
        LB_Word_T       PrtGenBuildNextLine(LB_String_T& pa_Line);

        LB_Bool_T       CktStoreFileName(LB_String_T& pa_CktFileName);
        LB_Bool_T       CktGetAttrValue(LB_String_T  pa_AttrName,
                                        LB_String_T& pa_AttrValue);
        LB_Bool_T       CktLoad();
        LB_Bool_T       CktGetInfo(LB_String_T& pa_FN,
                                   LB_String_T& pa_Name,
                                   LB_String_T& pa_Desc,
                                   LB_String_T& pa_Comments);
        LB_Bool_T       CktGetCrpeRptName(LB_String_T& pa_CrpeRptName);

        LB_Bool_T       CrpeGenerateDataFile(LB_String_T& pa_OutputFN);
        void            CrpeCleanup();

    private:
        //--- Private Variables
        GemControl_T            m_GemCtl;
        LB_Fio_T                m_InpFile;
        XB_Table_T              m_ChkTable;
        XB_Table_T              m_DedTable;
        XB_Table_T              m_ErnTable;

        struct CHK_TEMPLATE_T
        {
            LB_Bool_T           LoadedFlag;
            LB_String_T         FileName;
            LB_String_T         AttrTitle;
            LB_String_T         AttrDesc;
            LB_String_T         AttrComments;
            LB_String_T         AttrCrpeRptName;
            LB_String_T         AttrRptType;
            LB_String_T         AttrRptFontName;
            LB_String_T         AttrRptFontSize;
			LB_String_T			AttrFilter1099;
            LB_String_T			AttrSingleEntry;
			LB_String_T			AttrDoubleEntry;
            LB_String_T         Lines[PAYLP_MAX_CKT_LINES];
            LB_Word_T           LineCnt;
            LB_Word_T           LineCur;
        }
        m_Template;

        struct FILE_CKTINFO_T
        {
            LB_Byte_T           CktFileName[128];
            LB_Byte_T           Cr;
            LB_Byte_T           DeleteFlag;
        }
        m_CktFileInfo;

        typedef struct CHK_STUB_EARN_T
        {
            LB_String_T         Code;
            LB_String_T         Hours;
            LB_String_T         Rate;
            LB_String_T         Amt;
        };

        typedef struct CHK_STUB_DED_T
        {
            LB_String_T         Code;
            LB_String_T         Amt;
            LB_String_T         AmtYTD;
            LB_String_T         Balance;
        };

        typedef struct CHK_STUB_T
        {
            LB_String_T         EmpNum;
            LB_String_T         EmpName;
            LB_String_T         SocSecNum;
            LB_String_T         PeriodEnd;
            LB_String_T         DateIssued;
            LB_String_T         ChkNum;
            CHK_STUB_EARN_T     Earn[PAYLP_MAX_EARN_LINES];
            LB_String_T         FedTax;
            LB_String_T         FedTaxYTD;
            LB_String_T         StateTax;
            LB_String_T         StateTaxYTD;
            LB_String_T         Fica;
            LB_String_T         FicaYTD;
            LB_String_T         Medicare;
            LB_String_T         MedicareYTD;
            LB_String_T         VacationTime;
            LB_String_T         VacationRemain;
            LB_String_T         SickTime;
            LB_String_T         SickRemain;
            LB_String_T         PersonalTime;
            LB_String_T         PersonalRemain;
            LB_String_T         OtherTime;
            LB_String_T         OtherRemain;
            CHK_STUB_DED_T      Deduct[PAYLP_MAX_DED_LINES];
            LB_String_T         Earnings;
            LB_String_T         EarningsYTD;
            LB_String_T         NetPay;
            LB_String_T         NetPayYTD;
            LB_String_T         MaritalStatus;
            LB_String_T         Allowances;
            LB_String_T         CostCenter;
            LB_String_T         Msg1;
            LB_String_T         Msg2;
            LB_String_T         Location;
        };

        struct CHK_BODY_T
        {
            LB_String_T         Date;
            LB_String_T         ChkNum;
            LB_String_T         Amt;
            LB_String_T         SpellAmt[2];
            LB_String_T         PayToName;
            LB_String_T         PayToAddr[PAYLP_MAX_ADDR_LINES];
        };

        struct FORM_CHK_T
        {
            struct CHK_STUB_T   Stub;
            struct CHK_BODY_T   Body;
        } m_Chk;

        struct FORM_W2_T
        {
            LB_String_T         Control;
            LB_String_T         FedID;
            LB_String_T         FedWages;
            LB_String_T         FedTax;
            LB_String_T         FicaWages;
            LB_String_T         FicaTax;
            LB_String_T         OrgName;
            LB_String_T         OrgAddr[2];
            LB_String_T         OrgCityState;
            LB_String_T         OrgZipCode;
            LB_String_T         MedWages;
            LB_String_T         MedTax;
            LB_String_T         SocSecNum;
            LB_String_T         EIC;
            LB_String_T         Fringe;
            LB_String_T         DepCare;
            LB_String_T         EmpName;
            LB_String_T         EmpAddr[3];
            LB_String_T         EmpZipCode;
            LB_String_T         Box12Code[4];
            LB_String_T         Box12Amt[4];
            LB_String_T         Box14Code[4];
            LB_String_T         Box14Amt[4];
            LB_String_T         PensionFlag;
            LB_String_T         SubTotalFlag;
            LB_String_T         DefCompFlag;
            LB_String_T         StateName;
            LB_String_T         StateID;
            LB_String_T         StateWages;
            LB_String_T         StateTax;
			LB_String_T         EmpFName;
        } m_W2[PAYLP_MAX_W2_PER_PAGE];

        struct FORM_1099_T
        {
            LB_String_T         PayerFedID;
            LB_String_T         PayerStateID;
            LB_String_T         PayerName;
            LB_String_T         PayerAddr[3];
            LB_String_T         RecipFedID;
            LB_String_T         RecipName;
            LB_String_T         RecipAddr[3];
            LB_String_T         FedTax;
            LB_String_T         NonEmpComp;
			LB_String_T			Filter1099;
			LB_String_T			Rents;
        } m_1099[PAYLP_MAX_1099_PER_PAGE];

        enum
        {
            RPTTYPE_NONE,
            RPTTYPE_CHK,
            RPTTYPE_CHK_CRPE,
            RPTTYPE_NOD,
            RPTTYPE_NOD_CRPE,
            RPTTYPE_W2,
            RPTTYPE_W2_CRPE,
            RPTTYPE_1099,
            RPTTYPE_1099_CRPE,
        } m_RptType;

        //--- Private Methods
        void                    m_Clear();
        void                    m_ChkClear();
        void                    m_W2Clear();
        void                    m_1099Clear();
        void                    m_TemplateClear();
  };
#endif