/*  gempaydd.cpp
 *
 *  Gem Payroll Direct Deposit Generation routines
 *
 *
 *                       * PROPRIETARY PROGRAM MATERIAL *
 *
 *      NOTICE: THIS MATERIAL IS PROPRIETARY TO UNITED COMPUTER
 *      AND IS NOT TO BE REPRODUCED, USED OR DISCLOSED EXCEPT
 *      UPON WRITTEN AUTHORIZATION OF:
 *
 *                      UNITED COMPUTER SALES & SERVICE, INC.
 *                      100 DOBBS LANE, SUITE 208
 *                      CHERRY HILL, NEW JERSEY 08034
 *                      609.795.7330
 *
 *            COPYRIGHT (C) 1995-1996 UNITED COMPUTER SALES & SERVICE, INC.
 *
 *
 * Revision History:
 *
 *      Date            By      Notes
 *      --------        ---     ------------------------------------------
 *      02/20/96        LR      -Initial Release
 *      03/18/96        LR      -Organization debit was fixed for tran code
 *                              on prenotification
 *
 *
 * Notes:
 *
 *
**/


//---------------------------------------------------------------
//---                INCLUDE FILES                            ---
//---------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <lbport.h>
#include <lbdate.hpp>
#include <lbtime.hpp>
#include <lbfcalc.hpp>
#include <lbfio.hpp>
#include <lbstrlst.hpp>
#include <gemcntrl.hpp>
#include <gempaym.hpp>
#include <gempayh.hpp>
#include <gempaydd.hpp>


//--- Moved from gempaydd.hpp to eliminate structure size > 64K messages
GemControl_T    far m_GemCtlFile;
GemPayMast_T    far m_GemMastFile;
GemPayHist_T    far m_GemHistFile;
GemPayRefOrg_T  far m_GemRefOrgFile;
GemPayRefBank_T far m_GemRefBankFile;
GemDDACH_T      far m_ACHFile;
GemDDBank_T     far m_BankFile;


void                    GemPayDD_T::m_LogMessage()
{
    LB_Fio_T      l_LogFile;
    LB_String_T   l_LogMsg;
    LB_Date_T     l_Date;
    LB_Time_T     l_Time;
    char          l_StrBuf[64];

    l_Date.SetToToday();
    l_Time.SetToNow();

    //--- Get Date
    sprintf(l_StrBuf,"%02d/%02d/%04d",l_Date.GetMonth(),
                                      l_Date.GetDay(),
                                      l_Date.GetYear());
    l_LogMsg = l_StrBuf;
    l_LogMsg = l_LogMsg + " ";

    //--- Get Time
    sprintf(l_StrBuf,"%02d:%02d:%02d",l_Time.GetHour(),
                                      l_Time.GetMinute(),
                                      l_Time.GetSecond());
    l_LogMsg = l_LogMsg + l_StrBuf;
    l_LogMsg = l_LogMsg + " ";

    //--- Get Message
    l_LogMsg = l_LogMsg + m_LogMsg;
    l_LogMsg = l_LogMsg + ".";

    //--- Add CR/LF pair
    sprintf(l_StrBuf,"%c%c",0x0D,0x0A);
    l_LogMsg = l_LogMsg + l_StrBuf;

    //  printf("\n"); l_LogMsg.Display();

    l_LogFile.SetFileName(m_LogFileName);
    if (l_LogFile.Exists() == LB_FALSE)
    {
        if (l_LogFile.Create() == LB_FALSE)
            return;
    }

    if (l_LogFile.Open(FIO_OM_RW_BUF) == LB_FALSE)
        return;

    l_LogFile.SeekFromEOF(0);


    l_LogFile.Write(l_LogMsg);

    l_LogFile.Close();
}


//---------------------------------------------------------------
//---                   Public  Methods                       ---
//---------------------------------------------------------------
GemPayDD_T::GemPayDD_T()
{
    m_OrgID     = 0;
    m_Week      = "0";
    m_IsPreNote = LB_FALSE;
    m_EffDate.Clear();
    memset(m_LogMsg,'\0',sizeof(m_LogMsg));
}


GemPayDD_T::~GemPayDD_T()
{
    Abort();
}


void                    GemPayDD_T::Abort()
{
    //--- Close Files
    m_GemMastFile.Close();
    m_GemHistFile.Close();
    m_GemRefBankFile.Close();
    m_GemRefOrgFile.Close();
    m_ACHFile.Close();
    m_LogFile.Close();
}


void                    GemPayDD_T::GetLastLogMessage(LB_String_T& pa_Message)
{
    if (strlen(m_LogMsg) > sizeof(m_LogMsg))
        memset(m_LogMsg,'\0',sizeof(m_LogMsg));

    pa_Message = m_LogMsg;
    pa_Message = pa_Message + ".";
}


LB_Bool_T               GemPayDD_T::Initialize(LB_Word_T    pa_OrgID,
                                               LB_Word_T    pa_Week,
                                               LB_Bool_T    pa_IsPreNote,
                                               LB_String_T& pa_EffDate)
{
    LB_String_T   l_String;
    LB_String_T   l_GemPayPath;

    LB_String_T   l_HistWeek;
    LB_String_T   l_HistEmpNum;
    LB_String_T   l_HistExtra;
    LB_String_T   l_HistManual;
    LB_String_T   l_HistRemoved;
    LB_String_T   l_HistNet;
    LB_String_T   l_HistRecord;

    LB_String_T   l_MastEmpNum;
    LB_String_T   l_MastEmpName;
    LB_String_T   l_MastDDAcct;
    LB_String_T   l_MastDDBank;
    LB_String_T   l_MastUser10;
    LB_String_T   l_MastRecord;

    LB_String_T   l_BankRefNumber;
    LB_String_T   l_BankRefRTID;
    LB_String_T   l_BankRefRecord;

    m_OrgID     = pa_OrgID;
    m_Week      = pa_Week;
    m_EffDate   = pa_EffDate;
    m_IsPreNote = pa_IsPreNote;


    //--- Get Payroll Path
    if (m_GemCtlFile.Read(pa_OrgID) == LB_FALSE)
    {
        sprintf(m_LogMsg,"Initialize: Could not read GemControl for id %u",pa_OrgID);
        return (LB_FALSE);
    }
    m_GemCtlFile.GetPayrollPath(l_GemPayPath);


    //--- Get Log File Name
    m_LogFileName = l_GemPayPath;
    m_LogFileName = m_LogFileName + "achlog.dat";

    sprintf(m_LogMsg,"***Initialize: Start, OrgID#%u, Week#%u", pa_OrgID, pa_Week);
    m_LogMessage();

    {
        char PathBuf[512];
        l_GemPayPath.AsAsciiZ(PathBuf, sizeof(PathBuf));
        sprintf(m_LogMsg, "Payroll path is '%s'", PathBuf);
        m_LogMessage();
    }

    //--- Read BANK Information
    if (m_BankFile.Read(m_OrgID) == LB_FALSE)
    {
        sprintf(m_LogMsg,"Initialize: Could not read bank file for id %u",pa_OrgID);
        m_LogMessage();
        return (LB_FALSE);
    }
    if (m_BankFile.GetColumnValue(GEMDDBNK_COL_RTID,    m_BankRTID)    == LB_FALSE)
    {
        sprintf(m_LogMsg,"Initialize: Could not read BankFile -> RTID");
        m_LogMessage();
        return (LB_FALSE);
    }
    if (m_BankFile.GetColumnValue(GEMDDBNK_COL_NAME,    m_BankName)    == LB_FALSE)
    {
        sprintf(m_LogMsg,"Initialize: Could not read BankFile -> BankName");
        m_LogMessage();
        return (LB_FALSE);
    }
    if (m_BankFile.GetColumnValue(GEMDDBNK_COL_TYPE,    m_BankType)    == LB_FALSE)
    {
        sprintf(m_LogMsg,"Initialize: Could not read BankFile -> BankType");
        m_LogMessage();
        return (LB_FALSE);
    }
    if (m_IsPreNote == LB_TRUE)
    {
        if (m_BankType == "27")
            m_BankType = "28";
        else if (m_BankType == "37")
            m_BankType = "38";
    }
    if (m_BankFile.GetColumnValue(GEMDDBNK_COL_ACCTNUM, m_BankAcctNum) == LB_FALSE)
    {
        sprintf(m_LogMsg,"Initialize: Could not read BankFile -> BankAcctNum");
        m_LogMessage();
        return (LB_FALSE);
    }

    //--- Get PAYREF ORGANIZATION Information
    //------ Open PAYREF Organization
    if (m_GemRefOrgFile.Open(m_OrgID) == LB_FALSE)
    {
        sprintf(m_LogMsg,"Initialize: Could not open PayRefOrgFile");
        m_LogMessage();
        return (LB_FALSE);
    }

    //------ Read PAYREF Organization
    if (m_GemRefOrgFile.ReadNext() == LB_FALSE)
    {
        sprintf(m_LogMsg,"Initialize: Could not read PayRefOrgFile");
        m_LogMessage();
        return (LB_FALSE);
    }
    if (m_GemRefOrgFile.GetColumnValue(PAYRO_COL_ORGNAME,  m_RefName) == LB_FALSE)
    {
        sprintf(m_LogMsg,"Initialize: Could not read PayRefOrgFile -> RefName");
        m_LogMessage();
        return (LB_FALSE);
    }
    if (m_GemRefOrgFile.GetColumnValue(PAYRO_COL_ORGPHONE, m_RefPhone) == LB_FALSE)
    {
        sprintf(m_LogMsg,"Initialize: Could not read PayRefOrgFile -> RefPhone");
        m_LogMessage();
        return (LB_FALSE);
    }
    if (m_GemRefOrgFile.GetColumnValue(PAYRO_COL_FEDID,    m_RefFedID) == LB_FALSE)
    {
        sprintf(m_LogMsg,"Initialize: Could not read PayRefOrgFile -> RefFedID");
        m_LogMessage();
        return (LB_FALSE);
    }
    m_RefFedID.Filter('-');
    m_GemRefOrgFile.Close();

    //--- Create a subset list of PAYREF BANK Information
    //------ Open PAYREF Bank
    m_BankRefList.Clear();
    m_BankRefList.SetAsUnsorted();
    if (m_GemRefBankFile.Open(m_OrgID) == LB_FALSE)
    {
        sprintf(m_LogMsg,"Initialize: Could not read PayRefBankFile");
        m_LogMessage();
        return (LB_FALSE);
    }
    while (m_GemRefBankFile.ReadNext() == LB_TRUE)
    {
        if (m_GemRefBankFile.GetColumnValue(PAYRB_COL_ID,l_String) == LB_FALSE)
        {
            sprintf(m_LogMsg,"Initialize: Could not read PayRefBankFile -> ID");
            m_LogMessage();
            return (LB_FALSE);
        }
        if (l_String != "9")
            continue;

        if (m_GemRefBankFile.GetColumnValue(PAYRB_COL_INDEX,l_String) == LB_FALSE)
        {
            sprintf(m_LogMsg,"Initialize: Could not read PayRefBankFile -> Index");
            m_LogMessage();
            return (LB_FALSE);
        }
        if (l_String != "0")
            continue;

        if (m_GemRefBankFile.GetColumnValue(PAYRB_COL_REC,l_BankRefNumber) == LB_FALSE)
        {
            sprintf(m_LogMsg,"Initialize: Could not read PayRefBankFile -> RefNumber");
            m_LogMessage();
            return (LB_FALSE);
        }
        l_BankRefNumber.PadLeft(4,'0');

        if (m_GemRefBankFile.GetColumnValue(PAYRB_COL_BANKRTID,l_BankRefRTID) == LB_FALSE)
        {
            sprintf(m_LogMsg,"Initialize: Could not get BankRef -> RTID");
            m_LogMessage();
            return (LB_FALSE);
        }
        //--- If Routing/Transit is empty, don't add to list
        l_BankRefRTID.Trim();
        if (l_BankRefRTID.IsEmpty() == LB_TRUE)
            continue;
        else
        {
            l_BankRefRTID.PadLeft(9,'0');
            l_BankRefRecord = l_BankRefNumber;
            l_BankRefRecord = l_BankRefRecord + " ";
            l_BankRefRecord = l_BankRefRecord + l_BankRefRTID;
            if (m_BankRefList.Add(l_BankRefRecord,0,4) == LB_FALSE)
            {
                sprintf(m_LogMsg,"Initialize: Could not add to BankRefList");
                m_LogMessage();
                return (LB_FALSE);
            }
        }
    }
    m_GemRefBankFile.Close();

    //--- Create a subset list of master records
    //------ Open PAYMAST
    m_MastList.Clear();
    if (m_GemMastFile.Open(m_OrgID) == LB_FALSE)
    {
        sprintf(m_LogMsg,"Initialize: Could not open master file");
        m_LogMessage();
        return (LB_FALSE);
    }
    //------ Loop through file
    m_GemMastFile.Restart();
    while (m_GemMastFile.ReadNext() == LB_TRUE)
    {
        //------ Get direct deposit account
        if (m_GemMastFile.GetColumnValue(PAYM_COL_DDACCT,l_MastDDAcct) == LB_FALSE)
        {
            sprintf(m_LogMsg,"Initialize: Could not read MastFile -> MastDDAcct");
            m_LogMessage();
            return (LB_FALSE);
        }
        l_MastDDAcct.Trim();

        //------ Skip if employee has no direct deposit account
        if (l_MastDDAcct.IsEmpty() == LB_TRUE)
            continue;

        //------ Pad dd acct
        l_MastDDAcct.PadRight(15,' ');
        //------ Get employee number
        if (m_GemMastFile.GetColumnValue(PAYM_COL_EMPNUM, l_MastEmpNum) == LB_FALSE)
        {
            sprintf(m_LogMsg,"Initialize: Could not read MastFile -> EmpNum");
            m_LogMessage();
            return (LB_FALSE);
        }
        l_MastEmpNum.PadLeft(7,'0');

        //------ Get employee name
        if (m_GemMastFile.GetColumnValue(PAYM_COL_EMPNAME,l_MastEmpName) == LB_FALSE)
        {
            sprintf(m_LogMsg,"Initialize: Could not read MastFile -> EmpName");
            m_LogMessage();
            return (LB_FALSE);
        }
        l_MastEmpName.PadRight(30,' ');

        //------ Get employee dd account type
        if (m_GemMastFile.GetColumnValue(PAYM_COL_USER10, l_MastUser10) == LB_FALSE)
        {
            sprintf(m_LogMsg,"Initialize: Could not read MastFile -> User Field #10");
            m_LogMessage();
            return (LB_FALSE);
        }

        if (m_IsPreNote == LB_TRUE)
        {
            if (l_MastUser10 == "22")
                l_MastUser10 = "23";
            else if (l_MastUser10 == "32")
                l_MastUser10 = "33";
        }
        l_MastUser10.PadLeft(2,'0');

        //------ Get Employee Bank Number
        if (m_GemMastFile.GetColumnValue(PAYM_COL_DDBANKNUM,l_MastDDBank) == LB_FALSE)
        {
            sprintf(m_LogMsg,"Initialize: Could not read MastFile -> MastDDBank");
            m_LogMessage();
            return (LB_FALSE);
        }
        l_MastDDBank.PadLeft(4,'0');


        l_MastRecord = l_MastEmpNum;
        l_MastRecord = l_MastRecord + " ";
        l_MastRecord = l_MastRecord + l_MastEmpName;
        l_MastRecord = l_MastRecord + " ";
        l_MastRecord = l_MastRecord + l_MastDDAcct;
        l_MastRecord = l_MastRecord + " ";
        l_MastRecord = l_MastRecord + l_MastDDBank;
        l_MastRecord = l_MastRecord + " ";
        l_MastRecord = l_MastRecord + l_MastUser10;

        //--- Add to sorted list
        if (m_MastList.Add(l_MastRecord,0,7) == LB_FALSE)
        {
            sprintf(m_LogMsg,"Initialize: Could not add to MastList");
            m_LogMessage();
            return (LB_FALSE);
        }
    }
    m_GemMastFile.Close();

    //--- Create a subset list of history records
    //--- Open PAYHIST
    m_HistList.Clear();
    m_HistList.SetAsUnsorted();
    if (m_GemHistFile.Open(m_OrgID) == LB_FALSE)
    {
        sprintf(m_LogMsg,"Initialize: Could not open history file");
        m_LogMessage();
        return (LB_FALSE);
    }
    //--- Loop through file
    m_GemHistFile.Restart();
    while (m_GemHistFile.ReadNext() == LB_TRUE)
    {
        //------ Check if history was removed
        if (m_GemHistFile.GetColumnValue(PAYH_COL_REMOVED,l_HistRemoved) == LB_FALSE)
        {
            sprintf(m_LogMsg,"Initialize: Could not read HistFile -> HistRemoved");
            m_LogMessage();
            return (LB_FALSE);
        }
        if (l_HistRemoved.AsFloatPt() > 0.00)
            continue;

        //------ Check if week is current
        if (m_GemHistFile.GetColumnValue(PAYH_COL_WEEK,l_HistWeek) == LB_FALSE)
        {
            sprintf(m_LogMsg,"Initialize: Could not read HistFile -> HistWeek");
            m_LogMessage();
            return (LB_FALSE);
        }
        if (l_HistWeek != m_Week)
            continue;

        //------ Get Employee Number
        if (m_GemHistFile.GetColumnValue(PAYH_COL_EMPNUM,l_HistEmpNum) == LB_FALSE)
        {
            sprintf(m_LogMsg,"Initialize: Could not read HistFile -> EmpNum");
            m_LogMessage();
            return (LB_FALSE);
        }
        l_HistEmpNum.PadLeft(7,'0');

        //--- Get Net
        if (m_GemHistFile.GetColumnValue(PAYH_COL_NET,l_HistNet) == LB_FALSE)
        {
            sprintf(m_LogMsg,"Initialize: Could not read HistFile -> Net");
            m_LogMessage();
            return (LB_FALSE);
        }
        l_HistNet.PadLeft(11,'0');

        //------ Get Extra
        if (m_GemHistFile.GetColumnValue(PAYH_COL_EXTRA,l_HistExtra) == LB_FALSE)
        {
            sprintf(m_LogMsg,"Initialize: Could not read HistFile -> Extra");
            m_LogMessage();
            return (LB_FALSE);
        }
        l_HistExtra.PadLeft(2,'0');

        //------ Get Manual
        if (m_GemHistFile.GetColumnValue(PAYH_COL_MANUAL,l_HistManual) == LB_FALSE)
        {
            sprintf(m_LogMsg,"Initialize: Could not read HistFile -> Manual");
            m_LogMessage();
            return (LB_FALSE);
        }

        //------ Build record
        l_HistRecord = l_HistEmpNum;
        l_HistRecord = l_HistRecord + " ";
        l_HistRecord = l_HistRecord + l_HistExtra;
        l_HistRecord = l_HistRecord + " ";
        l_HistRecord = l_HistRecord + l_HistManual;
        l_HistRecord = l_HistRecord + " ";
        l_HistRecord = l_HistRecord + l_HistNet;

        //--- Add to sorted list
        if (m_HistList.Add(l_HistRecord,0,7) == LB_FALSE)
        {
            sprintf(m_LogMsg,"Initialize: Could not add to HistList");
            m_LogMessage();
            return (LB_FALSE);
        }
    }
    m_GemHistFile.Close();

    //--- Create/Open ACH Deposit File
    if (m_ACHFile.Create(m_OrgID) == LB_FALSE)
    {
        sprintf(m_LogMsg,"Initialize: Could not create ACH file");
        m_LogMessage();
        return (LB_FALSE);
    }

    if (m_ACHFile.Open(m_OrgID) == LB_FALSE)
    {
        sprintf(m_LogMsg,"Initialize: Could not open ACH file");
        m_LogMessage();
        return (LB_FALSE);
    }

    //--- Write file header
    //------ Get Origination RTID
    m_ACHFile.SetFHInfo(GEMDDACH_FH_ORIGID,   m_RefFedID);

    //------ Get Destination RTID
    m_ACHFile.SetFHInfo(GEMDDACH_FH_DESTID,   m_BankRTID);

    //------ Get Destination Name
    m_ACHFile.SetFHInfo(GEMDDACH_FH_DESTNAME, m_BankName);

    //------ Get Origination Name
    m_ACHFile.SetFHInfo(GEMDDACH_FH_ORIGNAME, m_RefName);

    //------ Get Reference Code
    m_ACHFile.SetFHInfo(GEMDDACH_FH_REFCD,    m_RefPhone);

    //------ Write Record
    if (m_ACHFile.WriteFH() == LB_FALSE)
    {
        sprintf(m_LogMsg,"Initialize: Could not write File Header record");
        m_LogMessage();
        return (LB_FALSE);
    }

    //--- Write batch header
    //------ Origination Name
    m_ACHFile.SetBHInfo(GEMDDACH_BH_ORIGNAME, m_RefName);

    //------ Origination ID
    m_ACHFile.SetBHInfo(GEMDDACH_BH_ORIGID,   m_RefFedID);

    //------ Effective Date
    l_String = pa_EffDate.SubstrMid(0,2) +
               pa_EffDate.SubstrMid(3,2) +
               pa_EffDate.SubstrMid(6,2);
    m_ACHFile.SetBHInfo(GEMDDACH_BH_EFFDATE,  l_String);

    //------ Destination RTID
    m_ACHFile.SetBHInfo(GEMDDACH_BH_DESTID,   m_BankRTID);

    //------ Batch
    m_Batch = pa_Week; m_Batch.PadLeft(2,'0');
    m_Batch = m_Batch + pa_EffDate.SubstrMid(0,2) +
                        pa_EffDate.SubstrMid(3,2) +
                        pa_EffDate.SubstrMid(6,1);
    m_ACHFile.SetBHInfo(GEMDDACH_BH_BATCH,    m_Batch);

    //------ Write record
    if (m_ACHFile.WriteBH() == LB_FALSE)
    {
        sprintf(m_LogMsg,"Initialize: Could not write Batch Header record");
        m_LogMessage();
        return (LB_FALSE);
    }

    //--- Create trace sequence
    m_Trace = m_BankRTID;
    m_Trace = m_Trace.SubstrLeft(8);
    m_Trace = m_Trace + "0000000";

    return (LB_TRUE);
}


LB_Bool_T               GemPayDD_T::Generate()
{
    LB_String_T   l_BankType;
    LB_String_T   l_BankID;
    LB_String_T   l_BankAcctNum;

    LB_String_T   l_MastRecord;
    LB_String_T   l_MastEmpNum;
    LB_String_T   l_MastEmpName;
    LB_String_T   l_MastDDAcct;
    LB_String_T   l_MastDDBank;
    LB_String_T   l_MastUser10;

    LB_String_T   l_HistRecord;
    LB_String_T   l_HistEmpNum;
    LB_String_T   l_HistExtra;
    LB_String_T   l_HistManual;
    LB_String_T   l_HistNet;

    LB_String_T   l_BankRefNumber;
    LB_String_T   l_BankRefRTID;
    LB_String_T   l_BankRefRecord;

    LB_String_T   l_String;
    LB_String_T   l_BlockCnt;
    LB_FinCalc_T  l_FinCalc;
    LB_FloatPt_T  l_DetailNet;
    LB_FloatPt_T  l_HashValue;
    LB_DWord_T    l_TotalDetailCnt;
    LB_FloatPt_T  l_TotalCredits;
    LB_FloatPt_T  l_TotalDebits;
    LB_Bool_T     l_EmpFound;

    if (m_OrgID == 0)
    {
        sprintf(m_LogMsg,"Generation: Organization ID is 0");
        m_LogMessage();
        return (LB_FALSE);
    }

    sprintf(m_LogMsg,"Generation: Start, OrgID#%u, Week#%u",m_OrgID,m_Week);
    m_LogMessage();

    //--- Initialize Totals
    l_TotalDetailCnt = 0;
    l_TotalCredits   = 0.00;
    l_TotalDebits    = 0.00;
    l_HashValue      = 0.00;

    //--- Read each master file record
    l_MastRecord.PadLeft(62,' ');
    m_MastList.GetReset();
    while (m_MastList.GetNext(l_MastRecord) == LB_TRUE)
    {
        l_MastEmpNum  = l_MastRecord.SubstrMid(0,7);
        l_MastEmpName = l_MastRecord.SubstrMid(8,30);
        l_MastDDAcct  = l_MastRecord.SubstrMid(39,15);
        l_MastDDBank  = l_MastRecord.SubstrMid(55,4);
        l_MastUser10  = l_MastRecord.SubstrMid(60,2);

        //--- Read history subset file for net totals
        l_HistRecord.PadLeft(24,' ');
        l_DetailNet = 0.00;
        l_EmpFound  = LB_FALSE;

        m_HistList.GetReset();
        while (m_HistList.GetNext(l_HistRecord) == LB_TRUE)
        {
            l_HistEmpNum = l_HistRecord.SubstrMid(0,7);
            if (l_HistEmpNum != l_MastEmpNum)
                continue;

            l_EmpFound = LB_TRUE;

            l_HistExtra  = l_HistRecord.SubstrMid(8,2);
            l_HistManual = l_HistRecord.SubstrMid(11,1);
            l_HistNet    = l_HistRecord.SubstrMid(13,11);

            //--- Check if manual check and not a pre-notification
            if (l_HistManual != "0")
            {
                if ((l_MastUser10 != "23") || (l_MastUser10 != "33"))
                    continue;
            }

            //--- Prenotification
            if ((l_MastUser10 == "23") || (l_MastUser10 == "33"))
            {
                l_DetailNet = 0.00;
            }
            else
            {
                //--- Standard payroll
                l_DetailNet    = l_FinCalc.Add(l_DetailNet,    l_HistNet.AsFloatPt());
                l_TotalCredits = l_FinCalc.Add(l_TotalCredits, l_HistNet.AsFloatPt());
                l_TotalDebits  = l_FinCalc.Add(l_TotalDebits,  l_HistNet.AsFloatPt());
            }
        }

        //--- Skip if employee has direct deposit but has no entries for the week
        if (l_EmpFound == LB_FALSE)
            continue;

        //--- Write detail entry
        //------ Get Transaction Code
        m_ACHFile.SetDTInfo(GEMDDACH_DT_TRANSCD, l_MastUser10);

        //------ Search for bank in reference file
        l_BankRefRecord.PadLeft('0',14);
        m_BankRefList.GetReset();
        while (m_BankRefList.GetNext(l_BankRefRecord) == LB_TRUE)
        {
            l_BankRefNumber = l_BankRefRecord.SubstrMid(0,4);
            l_BankRefNumber.Trim();
            l_BankRefRTID   = l_BankRefRecord.SubstrMid(5,9);
            if (l_BankRefNumber == l_MastDDBank)
                break;
        }

        if (l_BankRefNumber != l_MastDDBank)
        {
            sprintf(m_LogMsg,"Generation: Bank %.0f not found",l_MastDDBank.AsFloatPt());
            m_LogMessage();
            return (LB_FALSE);
        }

        //------ Get Bank ID
        m_ACHFile.SetDTInfo(GEMDDACH_DT_BANKID, l_BankRefRTID);

        //------ Get Bank Account Number
        l_String = l_MastDDAcct;
        m_ACHFile.SetDTInfo(GEMDDACH_DT_BANKACCTNUM, l_String);

        //------ Get Net amount as a whole number
        l_String = l_DetailNet * 100;
        m_ACHFile.SetDTInfo(GEMDDACH_DT_NET,l_String);

        //------ Get employee number
        m_ACHFile.SetDTInfo(GEMDDACH_DT_EMPNUM,  l_MastEmpNum);

        //------ Get employee name
        m_ACHFile.SetDTInfo(GEMDDACH_DT_EMPNAME, l_MastEmpName);

        //------ Get trace
        m_Trace++;
        m_ACHFile.SetDTInfo(GEMDDACH_DT_TRACE,   m_Trace);

        //------ Increment Hash Value
        l_String    = l_BankRefRTID;
        l_String    = l_String.SubstrLeft(8);
        l_HashValue = l_HashValue + l_String.AsFloatPt();

        //------ Write Detail record
        if (m_ACHFile.WriteDT() == LB_FALSE)
        {
            sprintf(m_LogMsg,"Generation: Could not write detail record");
            m_LogMessage();
            return (LB_FALSE);
        }

        l_TotalDetailCnt++;
    }

    //--- Write offsetting organization credit detail record
    m_Trace++;
    l_TotalDetailCnt++;

    m_ACHFile.SetDTInfo(GEMDDACH_DT_TRANSCD,     m_BankType);
    m_ACHFile.SetDTInfo(GEMDDACH_DT_BANKID,      m_BankRTID);
    m_ACHFile.SetDTInfo(GEMDDACH_DT_BANKACCTNUM, m_BankAcctNum);

    //------ Increment Hash Value
    l_String    = m_BankRTID;
    l_String    = l_String.SubstrLeft(8);
    l_HashValue = l_HashValue + l_String.AsFloatPt();

    l_String = l_TotalDebits * 100;
    m_ACHFile.SetDTInfo(GEMDDACH_DT_NET,  l_String);

    l_String = "99999";
    m_ACHFile.SetDTInfo(GEMDDACH_DT_EMPNUM,  l_String);

    m_ACHFile.SetDTInfo(GEMDDACH_DT_EMPNAME, m_RefName);

    m_ACHFile.SetDTInfo(GEMDDACH_DT_TRACE,   m_Trace);

    if (m_ACHFile.WriteDT() == LB_FALSE)
    {
        sprintf(m_LogMsg,"Generation: Could not write organization detail record");
        m_LogMessage();
        return (LB_FALSE);
    }

    //--- Write batch trailer
    l_String = l_TotalDetailCnt;
    m_ACHFile.SetBTInfo(GEMDDACH_BT_NUMDET,    l_String);

    l_String = l_HashValue;
    m_ACHFile.SetBTInfo(GEMDDACH_BT_HASH,      l_String);

    l_String = l_TotalDebits * 100;
    m_ACHFile.SetBTInfo(GEMDDACH_BT_TOTDB,     l_String);

    l_String = l_TotalCredits * 100;
    m_ACHFile.SetBTInfo(GEMDDACH_BT_TOTCR,     l_String);

    m_ACHFile.SetBTInfo(GEMDDACH_BT_ORIGID,    m_RefFedID);

    m_ACHFile.SetBTInfo(GEMDDACH_BT_DESTID,    m_BankRTID);

    m_ACHFile.SetBTInfo(GEMDDACH_BT_BATCH,     m_Batch);

    if (m_ACHFile.WriteBT() == LB_FALSE)
    {
        sprintf(m_LogMsg,"Generation: Could not write batch trailer record");
        m_LogMessage();
        return (LB_FALSE);
    }

    //--- Write file trailer
    l_String = (LB_Word_T) 1;
    m_ACHFile.SetFTInfo(GEMDDACH_FT_BATCHCNT,  l_String);

    l_BlockCnt = (l_TotalDetailCnt + 4) / 10;
    if (((l_TotalDetailCnt + 4) % 10) != 0)
        l_BlockCnt++;
    m_ACHFile.SetFTInfo(GEMDDACH_FT_BLOCKCNT,  l_BlockCnt);

    l_String = l_TotalDetailCnt;
    m_ACHFile.SetFTInfo(GEMDDACH_FT_NUMDET,    l_String);

    l_String = l_HashValue;
    m_ACHFile.SetFTInfo(GEMDDACH_FT_HASH,      l_String);

    l_String = l_TotalDebits * 100;
    m_ACHFile.SetFTInfo(GEMDDACH_FT_TOTDB,     l_String);

    l_String = l_TotalCredits * 100;
    m_ACHFile.SetFTInfo(GEMDDACH_FT_TOTCR,     l_String);

    if (m_ACHFile.WriteFT() == LB_FALSE)
    {
        sprintf(m_LogMsg,"Generation: Could not write file trailer record");
        m_LogMessage();
        return (LB_FALSE);
    }

    //--- Write file filler
    while ((l_TotalDetailCnt + 4) < (l_BlockCnt.AsWord() * 10))
    {
        if (m_ACHFile.WriteFiller() == LB_FALSE)
        {
            sprintf(m_LogMsg,"Generation: Could not write file filler record");
            m_LogMessage();
            return (LB_FALSE);
        }
        l_TotalDetailCnt++;
    }

    return (LB_TRUE);
}


LB_Bool_T               GemPayDD_T::Finish()
{
    if (m_OrgID == 0)
    {
        sprintf(m_LogMsg,"Completion: Organization ID is 0");
        m_LogMessage();
        return (LB_FALSE);
    }

    //--- Close Files
    m_ACHFile.Close();

    m_MastList.Clear();
    m_HistList.Clear();
    m_BankRefList.Clear();

    sprintf(m_LogMsg,"Direct Deposit Generation Completed");
    m_LogMessage();

    return (LB_TRUE);
}
