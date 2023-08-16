#ifndef  GEMBUDINQ_HPP
#define  GEMBUDINQ_HPP


#include <lbport.h>
#include <lbstring.hpp>
#include <lbhshlst.hpp>
#include <lbini.hpp>
#include <lbstrlst.hpp>
#include <gemseq.hpp>
#include <gembudh.hpp>
#include <gembudm.hpp>
#include <gemcntrl.hpp>
#include <xbtable.hpp>

#define   BUDINQ_PWD_ININAME    "budinq.pwd"
#define   BUDINQ_PWD_TMPNAME    "budinq.tmp"

class GemBudInquiry_T
{
    public:
        GemBudInquiry_T();

        ~GemBudInquiry_T();

        LB_Bool_T               BudMast_LoadAccounts(LB_Word_T    pa_OrgID,
                                                     LB_String_T& pa_SeqFileName,
                                                     LB_DWord_T   *pa_CntAcctsPtr);

        LB_Bool_T               BudMast_GetDetail(LB_String_T&  pa_Account,
                                                  LB_Byte_T *   pa_MastPtr,
                                                  LB_Word_T     pa_SizeMax);

        void                    BudMast_LookupReset();

        LB_Bool_T               BudMast_LookupNext(LB_String_T& pa_NextAccount);

        LB_Bool_T               BudHist_LookupStart(LB_Word_T pa_OrgID);

        LB_Bool_T               BudHist_LookupNext(LB_String_T& pa_Account,
                                                   LB_Byte_T *  pa_HistPtr,
                                                   LB_Word_T    pa_MaxSize,
                                                   LB_Word_T *  pa_CancelFlag);

        void                    BudHist_LookupFinish();

        LB_Bool_T               BudPwd_CreateNew(LB_Word_T pa_OrgID);

        LB_Bool_T               BudPwd_EncodeFile(LB_Word_T pa_OrgID);

        LB_Bool_T               BudPwd_DecodeFile(LB_Word_T pa_OrgID);

        void                    BudPwd_Clear();

        LB_Bool_T               BudPwd_GetSeqFileName(LB_String_T& pa_FileName);

        LB_Bool_T               BudPwd_IsUserAdmin();

        LB_Bool_T               BudPwd_Verify(LB_Word_T pa_OrgID,
                                              LB_String_T& pa_PwdStr);

        LB_Bool_T               CrpeABRGenDataFile(LB_Word_T    pa_OrgID,
                                                   LB_String_T& pa_SeqFileName,
                                                   LB_String_T& pa_OutputPath);

        void                    CrpeABRCleanup();


    private:
        GemSeqFile_T            m_SeqFile;
        GemBudMast_T            m_BudMastFile;
        GemBudHist_T            m_BudHistFile;
        GemControl_T            m_GemCtl;
        XB_Table_T              m_ABRTable;
        LB_HashList_T           m_SeqFileList;
        LB_StringList_T         m_BudMastList;
        LB_String_T             m_PwdSeqFileName;
        LB_Bool_T               m_PwdIsUserValid;
        LB_Bool_T               m_PwdIsUserAdmin;

        //--- A series of detail null-terminated ASCIIZ fields
        struct BUDMAST_T
          {
          LB_Byte_T     Account[32+1];
          LB_Byte_T     Title[40+1];
          LB_Byte_T     Type[1+1];
          LB_Byte_T     Fund[4+1];
          LB_Byte_T     GLDistID[4+1];
          LB_Byte_T     ReencFlag[1+1];
          LB_Byte_T     OrigAppr[13+1];
          LB_Byte_T     NetXFers[13+1];
          LB_Byte_T     ReencAmt[13+1];
          LB_Byte_T     NetAppr[13+1];
          LB_Byte_T     ExpendYTD[13+1];
          LB_Byte_T     ReceiptsYTD[13+1];
          LB_Byte_T     UnexpendBal[13+1];
          LB_Byte_T     EncumYTD[13+1];
          LB_Byte_T     UnencumBal[13+1];
          } m_BudMastRecord;

        struct BUDHIST_T
          {
          LB_Byte_T     Date[6+1];
          LB_Byte_T     Desc[24+1];
          LB_Byte_T     Amount[13+1];
          LB_Byte_T     Ref[3+1];
          LB_Byte_T     Type[1+1];
          LB_Byte_T     VenNum[7+1];
		  LB_Byte_T		PONum[9+1];
          } m_BudHistRecord;
};
#endif
