#ifndef  GEMPAYDD_HPP
#define  GEMPAYDD_HPP


#include <lbport.h>
#include <lbstring.hpp>
#include <lbstrlst.hpp>
#include <gempaym.hpp>
#include <gempayh.hpp>
#include <gempayrb.hpp>         // PAYREF Bank
#include <gempayro.hpp>         // PAYREF Organization
#include <gemcntrl.hpp>         // GEMCONTROL
#include <gemddach.hpp>         // ACH Deposit
#include <gemddbnk.hpp>         // ACH Organization Bank


class GemPayDD_T
{
    public:
        GemPayDD_T();

        ~GemPayDD_T();

        void                        Abort();

        LB_Bool_T                   Initialize(LB_Word_T    pa_OrgID,
                                               LB_Word_T    pa_Week,
                                               LB_Bool_T    pa_IsPreNote,
                                               LB_String_T& pa_EffDate);

        LB_Bool_T                   Generate();

        void                        GetLastLogMessage(LB_String_T& pa_Message);

        LB_Bool_T                   Finish();

    private:
        //--- Variables
        LB_Word_T                   m_OrgID;
        LB_String_T                 m_Week;
        LB_String_T                 m_EffDate;
        LB_Bool_T                   m_IsPreNote;

        LB_String_T                 m_Batch;
        LB_String_T                 m_Trace;
        LB_String_T                 m_RefName;
        LB_String_T                 m_RefPhone;
        LB_String_T                 m_RefFedID;
        LB_String_T                 m_BankRTID;
        LB_String_T                 m_BankName;
        LB_String_T                 m_BankType;
        LB_String_T                 m_BankAcctNum;

        LB_String_T                 m_LogFileName;

        //--- Moved to gempaydd.cpp to eliminate
        //--- structure size > 64K messages
        //GemControl_T                m_GemCtlFile;
        //GemPayMast_T                m_GemMastFile;
        //GemPayHist_T                m_GemHistFile;
        //GemPayRefOrg_T              m_GemRefOrgFile;
        //GemPayRefBank_T             m_GemRefBankFile;
        //GemDDACH_T                  m_ACHFile;
        //GemDDBank_T                 m_BankFile;

        LB_Fio_T                    m_LogFile;
        LB_StringList_T             m_HistList;
        LB_StringList_T             m_MastList;
        LB_StringList_T             m_BankRefList;

        char                        m_LogMsg[128];
        //--- Functions
        void                        m_LogMessage();
};
#endif
