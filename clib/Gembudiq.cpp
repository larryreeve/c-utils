/*  gembudiq.cpp
 *
 *  Gem Budget Inquiry Processing routines
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
 *      03/19/96        LR      -Initial Release
 *      04/02/96        LR      -Fixed PwdEncode to re-create a pwd file
 *                              instead of re-writing to allow for truncated
 *                              files.
 *      05/29/96        LR      -BudMastList is sorted; required for abbrev
 *                              budget report, but slows down initial loading
 *                              of accounts.
 *      11/09/96        LR      -Added VenNum field to History
 *      03/09/97        LR      -Set BudMastList as unsorted; previously sorted
 *                              for ABR, but ABR is now in sequence file order.
 *
 * Notes:
 *
 *
**/


//---------------------------------------------------------------
//---                INCLUDE FILES                            ---
//---------------------------------------------------------------
#include <mem.h>
#include <stdio.h>
#include <lbport.h>
#include <lbcrypt.hpp>
#include <lbdebug.hpp>
#include <gembudiq.hpp>


//---------------------------------------------------------------
//---                   Public  Methods                       ---
//---------------------------------------------------------------
GemBudInquiry_T::GemBudInquiry_T()
{
    m_PwdIsUserValid = LB_FALSE;
    m_PwdIsUserAdmin = LB_FALSE;
}


GemBudInquiry_T::~GemBudInquiry_T()
{
    m_SeqFile.Close();
    m_BudMastFile.Close();
    m_BudHistFile.Close();
    m_SeqFileList.Clear();
    m_BudMastList.Clear();
}


LB_Bool_T               GemBudInquiry_T::BudMast_LoadAccounts(LB_Word_T    pa_OrgID,
                                                              LB_String_T& pa_SeqFileName,
                                                              LB_DWord_T * pa_CntAcctsPtr)
{
    LB_String_T   AcctNum(32);
    LB_String_T   AcctType(1);
    LB_String_T   SrcMod(3);
    LB_String_T   Status(1);
    LB_String_T   BudMastRec(sizeof(m_BudMastRecord));
    LB_String_T   BudMastValue(40);

    *pa_CntAcctsPtr = 0;

    //--- Read sequence file for list of budget accounts
    if (m_SeqFile.Open(pa_OrgID, pa_SeqFileName) == LB_FALSE)
        return (LB_FALSE);

    m_SeqFileList.Clear();

    while (m_SeqFile.ReadNext() == LB_TRUE)
    {
        //--- Make sure record is active
        if (m_SeqFile.GetColumnValue(GEMSEQ_STATUS, Status) == LB_FALSE)
            return (LB_FALSE);

        if (Status == "A")
        {
            //--- Retrieve source module for account
            if (m_SeqFile.GetColumnValue(GEMSEQ_SRCMOD, SrcMod) == LB_FALSE)
                return (LB_FALSE);

            //--- Make sure account is APPROPRIATIONS
            if (SrcMod == "BUD")
            {
                //--- Retrieve account number
                if (m_SeqFile.GetColumnValue(GEMSEQ_ACCTNUM, AcctNum) == LB_FALSE)
                    return (LB_FALSE);

                //--- Add new sequence record to list
                m_SeqFileList.Add(AcctNum, 0, 32);
            }
        }
    }

    m_SeqFile.Close();

    //--- Read through BUDMAST file to get totals
    m_BudMastList.Clear();
    //m_BudMastList.SetAsUnsorted();

    if (m_BudMastFile.Open(pa_OrgID) == LB_FALSE)
        return (LB_FALSE);

    while (m_BudMastFile.ReadNext() == LB_TRUE)
    {
        //--- Check if budmast account is one we are looking for
        if (m_BudMastFile.GetColumnValue(BUDM_COL_ACCT, AcctNum) == LB_FALSE)
            return (LB_FALSE);

        if (m_SeqFileList.Exists(AcctNum, 0, AcctNum.Size()) == LB_FALSE)
            continue;

        //--- Build account record
        BudMastRec = AcctNum;

        if (m_BudMastFile.GetColumnValue(BUDM_COL_TITLE,       BudMastValue) == LB_FALSE)
            return (LB_FALSE);
        BudMastRec = BudMastRec + BudMastValue;

        if (m_BudMastFile.GetColumnValue(BUDM_COL_TYPE,        AcctType) == LB_FALSE)
            return (LB_FALSE);
        BudMastRec = BudMastRec + AcctType;

        if (m_BudMastFile.GetColumnValue(BUDM_COL_FUND,        BudMastValue) == LB_FALSE)
            return (LB_FALSE);
        BudMastValue.PadLeft(4,'0');
        BudMastRec = BudMastRec + BudMastValue;

        if (m_BudMastFile.GetColumnValue(BUDM_COL_GLDISTID,    BudMastValue) == LB_FALSE)
            return (LB_FALSE);
        BudMastValue.PadLeft(4,'0');
        BudMastRec = BudMastRec + BudMastValue;

        if (m_BudMastFile.GetColumnValue(BUDM_COL_REENCFLAG,   BudMastValue) == LB_FALSE)
            return (LB_FALSE);
        BudMastRec = BudMastRec + BudMastValue;

        if (m_BudMastFile.GetColumnValue(BUDM_COL_ORIGAPPR,    BudMastValue) == LB_FALSE)
            return (LB_FALSE);
        BudMastValue.PadLeft(13,'0'); BudMastValue.MakeNumeric();
        BudMastRec = BudMastRec + BudMastValue;

        if (m_BudMastFile.GetColumnValue(BUDM_COL_NETXFERS,    BudMastValue) == LB_FALSE)
            return (LB_FALSE);
        BudMastValue.PadLeft(13,'0'); BudMastValue.MakeNumeric();
        BudMastRec = BudMastRec + BudMastValue;

        if (m_BudMastFile.GetColumnValue(BUDM_COL_REENCAMT,    BudMastValue) == LB_FALSE)
            return (LB_FALSE);
        BudMastValue.PadLeft(13,'0'); BudMastValue.MakeNumeric();
        BudMastRec = BudMastRec + BudMastValue;

        if (m_BudMastFile.GetColumnValue(BUDM_COL_NETAPPR,     BudMastValue) == LB_FALSE)
            return (LB_FALSE);
        BudMastValue.PadLeft(13,'0'); BudMastValue.MakeNumeric();
        BudMastRec = BudMastRec + BudMastValue;

        if (m_BudMastFile.GetColumnValue(BUDM_COL_EXPENDYTD,   BudMastValue) == LB_FALSE)
            return (LB_FALSE);
        BudMastValue.PadLeft(13,'0'); BudMastValue.MakeNumeric();
        BudMastRec = BudMastRec + BudMastValue;

        if (m_BudMastFile.GetColumnValue(BUDM_COL_RECEIPTSYTD, BudMastValue) == LB_FALSE)
            return (LB_FALSE);
        BudMastValue.PadLeft(13,'0'); BudMastValue.MakeNumeric();
        BudMastRec = BudMastRec + BudMastValue;

        if (m_BudMastFile.GetColumnValue(BUDM_COL_UNEXPENDBAL, BudMastValue) == LB_FALSE)
            return (LB_FALSE);
        BudMastValue.PadLeft(13,'0'); BudMastValue.MakeNumeric();
        BudMastRec = BudMastRec + BudMastValue;

        if (m_BudMastFile.GetColumnValue(BUDM_COL_ENCUMYTD,    BudMastValue) == LB_FALSE)
            return (LB_FALSE);
        BudMastValue.PadLeft(13,'0'); BudMastValue.MakeNumeric();
        BudMastRec = BudMastRec + BudMastValue;

        if (m_BudMastFile.GetColumnValue(BUDM_COL_UNENCUMBAL,  BudMastValue) == LB_FALSE)
            return (LB_FALSE);
        BudMastValue.PadLeft(13,'0'); BudMastValue.MakeNumeric();
        BudMastRec = BudMastRec + BudMastValue;

        //--- Add new record to master list of budget accounts
        m_BudMastList.Add(BudMastRec, 0, 32);

        //--- Only increment count of records returned for non-alpha accounts
        if (AcctType != "A")
            *pa_CntAcctsPtr = *pa_CntAcctsPtr + 1;
    }

    m_BudMastFile.Close();

    //--- Free the sequence file account list
    m_SeqFileList.Clear();

    return (LB_TRUE);
}


LB_Bool_T               GemBudInquiry_T::BudMast_GetDetail(LB_String_T&  pa_Account,
                                                           LB_Byte_T *   pa_MastPtr,
                                                           LB_Word_T     pa_MaxSize)
{
    LB_Word_T     MaxRecSize;
    LB_Word_T     NextOffset;
    LB_String_T   BudMastRec(sizeof(m_BudMastRecord));
    LB_String_T   FldValue(40);
    LB_String_T   Acct(sizeof(m_BudMastRecord.Account)-1);

    //--- Clear structures: temporary and return
    memset(&m_BudMastRecord, '\0', sizeof(m_BudMastRecord));
    memset(pa_MastPtr,  '\0', pa_MaxSize);

    //--- Find account information
    Acct = pa_Account;
    Acct.PadRight(sizeof(m_BudMastRecord.Account)-1,' ');

    if (m_BudMastList.Retrieve(Acct, 0, Acct.Size(), BudMastRec) == LB_FALSE)
        return (LB_FALSE);

    BudMastRec.PadRight(sizeof(m_BudMastRecord), '0');

    //--- Extract account details
    //--- (-1) is to account for terminating NULL (ASCIIZ format)
    NextOffset = 0;

    FldValue = BudMastRec.SubstrMid(NextOffset,sizeof(m_BudMastRecord.Account)-1);
    FldValue.GetBytes(m_BudMastRecord.Account,sizeof(m_BudMastRecord.Account)-1);
    NextOffset = NextOffset + sizeof(m_BudMastRecord.Account)-1;

    FldValue = BudMastRec.SubstrMid(NextOffset,sizeof(m_BudMastRecord.Title)-1);
    FldValue.Proper();
    FldValue.GetBytes(m_BudMastRecord.Title,sizeof(m_BudMastRecord.Title)-1);
    NextOffset = NextOffset + sizeof(m_BudMastRecord.Title)-1;

    FldValue = BudMastRec.SubstrMid(NextOffset,sizeof(m_BudMastRecord.Type)-1);
    FldValue.GetBytes(m_BudMastRecord.Type,sizeof(m_BudMastRecord.Type)-1);
    NextOffset = NextOffset + sizeof(m_BudMastRecord.Type)-1;

    FldValue = BudMastRec.SubstrMid(NextOffset,sizeof(m_BudMastRecord.Fund)-1);
    FldValue.GetBytes(m_BudMastRecord.Fund,sizeof(m_BudMastRecord.Fund)-1);
    NextOffset = NextOffset + sizeof(m_BudMastRecord.Fund)-1;

    FldValue = BudMastRec.SubstrMid(NextOffset,sizeof(m_BudMastRecord.GLDistID)-1);
    FldValue.GetBytes(m_BudMastRecord.GLDistID,sizeof(m_BudMastRecord.GLDistID)-1);
    NextOffset = NextOffset + sizeof(m_BudMastRecord.GLDistID)-1;

    FldValue = BudMastRec.SubstrMid(NextOffset,sizeof(m_BudMastRecord.ReencFlag)-1);
    FldValue.GetBytes(m_BudMastRecord.ReencFlag,sizeof(m_BudMastRecord.ReencFlag)-1);
    NextOffset = NextOffset + sizeof(m_BudMastRecord.ReencFlag)-1;

    FldValue = BudMastRec.SubstrMid(NextOffset,sizeof(m_BudMastRecord.OrigAppr)-1);
    FldValue.GetBytes(m_BudMastRecord.OrigAppr,sizeof(m_BudMastRecord.OrigAppr)-1);
    NextOffset = NextOffset + sizeof(m_BudMastRecord.OrigAppr)-1;

    FldValue = BudMastRec.SubstrMid(NextOffset,sizeof(m_BudMastRecord.NetXFers)-1);
    FldValue.GetBytes(m_BudMastRecord.NetXFers,sizeof(m_BudMastRecord.NetXFers)-1);
    NextOffset = NextOffset + sizeof(m_BudMastRecord.NetXFers)-1;

    FldValue = BudMastRec.SubstrMid(NextOffset,sizeof(m_BudMastRecord.ReencAmt)-1);
    FldValue.GetBytes(m_BudMastRecord.ReencAmt,sizeof(m_BudMastRecord.ReencAmt)-1);
    NextOffset = NextOffset + sizeof(m_BudMastRecord.ReencAmt)-1;

    FldValue = BudMastRec.SubstrMid(NextOffset,sizeof(m_BudMastRecord.NetAppr)-1);
    FldValue.GetBytes(m_BudMastRecord.NetAppr,sizeof(m_BudMastRecord.NetAppr)-1);
    NextOffset = NextOffset + sizeof(m_BudMastRecord.NetAppr)-1;

    FldValue = BudMastRec.SubstrMid(NextOffset,sizeof(m_BudMastRecord.ExpendYTD)-1);
    FldValue.GetBytes(m_BudMastRecord.ExpendYTD,sizeof(m_BudMastRecord.ExpendYTD)-1);
    NextOffset = NextOffset + sizeof(m_BudMastRecord.ExpendYTD)-1;

    FldValue = BudMastRec.SubstrMid(NextOffset,sizeof(m_BudMastRecord.ReceiptsYTD)-1);
    FldValue.GetBytes(m_BudMastRecord.ReceiptsYTD,sizeof(m_BudMastRecord.ReceiptsYTD)-1);
    NextOffset = NextOffset + sizeof(m_BudMastRecord.ReceiptsYTD)-1;

    FldValue = BudMastRec.SubstrMid(NextOffset,sizeof(m_BudMastRecord.UnexpendBal)-1);
    FldValue.GetBytes(m_BudMastRecord.UnexpendBal,sizeof(m_BudMastRecord.UnexpendBal)-1);
    NextOffset = NextOffset + sizeof(m_BudMastRecord.UnexpendBal)-1;

    FldValue = BudMastRec.SubstrMid(NextOffset,sizeof(m_BudMastRecord.EncumYTD)-1);
    FldValue.GetBytes(m_BudMastRecord.EncumYTD,sizeof(m_BudMastRecord.EncumYTD)-1);
    NextOffset = NextOffset + sizeof(m_BudMastRecord.EncumYTD)-1;

    FldValue = BudMastRec.SubstrMid(NextOffset,sizeof(m_BudMastRecord.UnencumBal)-1);
    FldValue.GetBytes(m_BudMastRecord.UnencumBal,sizeof(m_BudMastRecord.UnencumBal)-1);
    NextOffset = NextOffset + sizeof(m_BudMastRecord.UnencumBal)-1;

    //--- Copy temporary structure to return structure
    MaxRecSize = sizeof(m_BudMastRecord);
    if (MaxRecSize > pa_MaxSize)
        MaxRecSize = pa_MaxSize;
    memcpy(pa_MastPtr, &m_BudMastRecord, MaxRecSize);

    return (LB_TRUE);
}


void                    GemBudInquiry_T::BudMast_LookupReset()
{
    m_BudMastList.GetReset();
}


LB_Bool_T               GemBudInquiry_T::BudMast_LookupNext(LB_String_T& pa_NextAccount)
{
    LB_String_T AcctTitle;
    LB_String_T AcctType;
    LB_String_T BudMastRec(sizeof(m_BudMastRecord));
    LB_Bool_T   EndOfListFlag;

    EndOfListFlag = LB_FALSE;
    do
    {
        if (m_BudMastList.GetNext(BudMastRec) == LB_FALSE)
        {
            EndOfListFlag = LB_TRUE;
            break;
        }
        else
        {
            //--- Skip alpha accounts
            AcctType = BudMastRec.SubstrMid(72,1);
            if (AcctType == "A")
                continue;

            break;
        }
    }
    while (EndOfListFlag == LB_FALSE);

    if (EndOfListFlag == LB_TRUE)
        return (LB_FALSE);

    pa_NextAccount = BudMastRec.SubstrMid(0,32);
    pa_NextAccount = pa_NextAccount + " ";

    AcctTitle = BudMastRec.SubstrMid(32,40);
    AcctTitle.Proper();

    pa_NextAccount = pa_NextAccount + AcctTitle;

    return (LB_TRUE);
}


LB_Bool_T               GemBudInquiry_T::BudHist_LookupStart(LB_Word_T pa_OrgID)
{
     return (m_BudHistFile.Open(pa_OrgID));
}


LB_Bool_T               GemBudInquiry_T::BudHist_LookupNext(LB_String_T&  pa_Account,
                                                            LB_Byte_T *   pa_HistPtr,
                                                            LB_Word_T     pa_MaxSize,
                                                            LB_Word_T *   pa_CancelFlag)
{
    LB_String_T   BudHistRec(sizeof(m_BudHistRecord));
    LB_String_T   Acct(32);
    LB_Word_T     MaxRecSize;

    //--- Clear structure temporary and return structures
    memset(&m_BudHistRecord, '\0', sizeof(m_BudHistRecord));
    memset(pa_HistPtr,       '\0', pa_MaxSize);

    pa_Account.PadRight(32,' ');

    //--- Continue through history until next account history record is found
    while (m_BudHistFile.ReadNext() == LB_TRUE)
    {
        // Test if user cancelled lookup
        if (*pa_CancelFlag != 0)
           return (LB_FALSE);


        if (m_BudHistFile.GetColumnValue(BUDH_COL_ACCT, Acct) == LB_FALSE)
            return (LB_FALSE);

        if (Acct == pa_Account)
            break;
    }

    if (Acct != pa_Account)
        return (LB_FALSE);

    //--- Extract account details
    //--- (-1) is to account for terminating NULL (ASCIIZ format)

    if (m_BudHistFile.GetColumnValue(BUDH_COL_DATE,BudHistRec) == LB_FALSE)
        return (LB_FALSE);
    BudHistRec.GetBytes(m_BudHistRecord.Date,sizeof(m_BudHistRecord.Date)-1);

    if (m_BudHistFile.GetColumnValue(BUDH_COL_DESC,BudHistRec) == LB_FALSE)
        return (LB_FALSE);
    BudHistRec.GetBytes(m_BudHistRecord.Desc,sizeof(m_BudHistRecord.Desc)-1);

    if (m_BudHistFile.GetColumnValue(BUDH_COL_AMOUNT,BudHistRec) == LB_FALSE)
        return (LB_FALSE);
    BudHistRec.PadLeft(sizeof(m_BudHistRecord.Amount)-1,'0');
    BudHistRec.MakeNumeric();
    BudHistRec.GetBytes(m_BudHistRecord.Amount,sizeof(m_BudHistRecord.Amount)-1);

    if (m_BudHistFile.GetColumnValue(BUDH_COL_REF,BudHistRec) == LB_FALSE)
        return (LB_FALSE);
    BudHistRec.GetBytes(m_BudHistRecord.Ref,sizeof(m_BudHistRecord.Ref)-1);

    if (m_BudHistFile.GetColumnValue(BUDH_COL_TYPE,BudHistRec) == LB_FALSE)
        return (LB_FALSE);
    BudHistRec.GetBytes(m_BudHistRecord.Type,sizeof(m_BudHistRecord.Type)-1);

    if (m_BudHistFile.GetColumnValue(BUDH_COL_VENNUM,BudHistRec) == LB_FALSE)
        return (LB_FALSE);
    BudHistRec.PadLeft(sizeof(m_BudHistRecord.VenNum)-1,' ');
    BudHistRec.GetBytes(m_BudHistRecord.VenNum,sizeof(m_BudHistRecord.VenNum)-1);

    if (m_BudHistFile.GetColumnValue(BUDH_COL_PONUM, BudHistRec) == LB_FALSE)
        return (LB_FALSE);
    BudHistRec.PadRight(sizeof(m_BudHistRecord.PONum) - 1,' ');
    BudHistRec.GetBytes(m_BudHistRecord.PONum,sizeof(m_BudHistRecord.PONum)-1);

    //--- Copy temporary structure to return structure
    MaxRecSize = sizeof(m_BudHistRecord);
    if (MaxRecSize > pa_MaxSize)
        MaxRecSize = pa_MaxSize;
    memcpy(pa_HistPtr, &m_BudHistRecord, MaxRecSize);

    return (LB_TRUE);
}


void                    GemBudInquiry_T::BudHist_LookupFinish()
{
    m_BudHistFile.Close();
}


LB_Bool_T               GemBudInquiry_T::BudPwd_CreateNew(LB_Word_T pa_OrgID)
{
    LB_String_T   FileName;
    LB_String_T   IniLine;
    LB_Fio_T      DecodedFile;

    if (m_GemCtl.Read(pa_OrgID) == LB_FALSE)
        return (LB_FALSE);

    m_GemCtl.GetBudgetPath(FileName);

    //--- Set File Name
    FileName = FileName + BUDINQ_PWD_TMPNAME;
    DecodedFile.SetFileName(FileName);

    //--* Create new encoded file with length 0
    if (DecodedFile.Create() == LB_FALSE)
        return (LB_FALSE);

    if (DecodedFile.Open(FIO_OM_RW_BUF) == LB_FALSE)
        return (LB_FALSE);

    IniLine = ";";
    DecodedFile.WriteLine(IniLine);

    IniLine = "; --- Budget Inquiry Password File ---";
    DecodedFile.WriteLine(IniLine);

    IniLine = ";";
    DecodedFile.WriteLine(IniLine);

    IniLine = "[Password-UCSS]";
    DecodedFile.WriteLine(IniLine);

    IniLine = "Admin=Y";
    DecodedFile.WriteLine(IniLine);

    IniLine = "SeqFile=BUDINQ.SEQ";
    DecodedFile.WriteLine(IniLine);

    IniLine = ";";
    DecodedFile.WriteLine(IniLine);

    DecodedFile.Close();

    BudPwd_EncodeFile(pa_OrgID);

    return (LB_TRUE);
}


LB_Bool_T               GemBudInquiry_T::BudPwd_DecodeFile(LB_Word_T pa_OrgID)
{
    LB_String_T   BudPath;
    LB_String_T   FileName;
    LB_String_T   FileBuf;
    LB_Fio_T      EncodedFile;
    LB_Fio_T      DecodedFile;
    LB_Crypt_T    CryptEngine;

    if (m_GemCtl.Read(pa_OrgID) == LB_FALSE)
        return (LB_FALSE);

    m_GemCtl.GetBudgetPath(BudPath);

    //--- Set File Names
    FileName = BudPath + BUDINQ_PWD_ININAME;  EncodedFile.SetFileName(FileName);
    FileName = BudPath + BUDINQ_PWD_TMPNAME;  DecodedFile.SetFileName(FileName);

    //--- Check if no password file exists, & create new if necessary
    if (EncodedFile.Exists() == LB_FALSE)
        BudPwd_CreateNew(pa_OrgID);

    if (EncodedFile.Open(FIO_OM_RO_BUF) == LB_FALSE)
        return (LB_FALSE);

    if (DecodedFile.Create() == LB_FALSE)
        return (LB_FALSE);

    if (DecodedFile.Open(FIO_OM_RW_BUF) == LB_FALSE)
        return (LB_FALSE);

    FileBuf.PadRight(EncodedFile.Size(), ' ');
    if (EncodedFile.Read(FileBuf) == LB_FALSE)
        return (LB_FALSE);
    EncodedFile.Close();

    FileBuf = CryptEngine.Decrypt(FileBuf);

    if (DecodedFile.Create() == LB_FALSE)
        return (LB_FALSE);

    if (DecodedFile.Open(FIO_OM_RW_BUF) == LB_FALSE)
        return (LB_FALSE);

    if (DecodedFile.Write(FileBuf) == LB_FALSE)
    {
        DecodedFile.Close();
        return (LB_FALSE);
    }
    DecodedFile.Close();

    return (LB_TRUE);
  }


LB_Bool_T               GemBudInquiry_T::BudPwd_EncodeFile(LB_Word_T pa_OrgID)
{
    LB_String_T   BudPath;
    LB_String_T   FileName;
    LB_String_T   FileBuf;
    LB_Fio_T      EncodedFile;
    LB_Fio_T      DecodedFile;
    LB_Crypt_T    CryptEngine;

    if (m_GemCtl.Read(pa_OrgID) == LB_FALSE)
        return (LB_FALSE);

    m_GemCtl.GetBudgetPath(BudPath);

    //--- Set File Names
    FileName = BudPath + BUDINQ_PWD_ININAME;  EncodedFile.SetFileName(FileName);
    FileName = BudPath + BUDINQ_PWD_TMPNAME;  DecodedFile.SetFileName(FileName);

    //--- Read decoded script
    if (DecodedFile.Open(FIO_OM_RO_BUF) == LB_FALSE)
        return (LB_FALSE);
    FileBuf.PadRight(DecodedFile.Size(),' ');
    if (DecodedFile.Read(FileBuf) == LB_FALSE)
        return (LB_FALSE);
    DecodedFile.Close();
    DecodedFile.Remove();

    //--- Encrypt the contents of the file
    FileBuf = CryptEngine.Encrypt(FileBuf);

    //--- Write encoded contents
    if (EncodedFile.Create() == LB_FALSE)
        return (LB_FALSE);

    if (EncodedFile.Open(FIO_OM_RW_BUF) == LB_FALSE)
        return (LB_FALSE);

    if (EncodedFile.Write(FileBuf) == LB_FALSE)
    {
        EncodedFile.Close();
        return (LB_FALSE);
    }
    EncodedFile.Close();

    return (LB_TRUE);
}


void                    GemBudInquiry_T::BudPwd_Clear()
{
    m_PwdIsUserValid = LB_FALSE;
    m_PwdIsUserAdmin = LB_FALSE;
    m_PwdSeqFileName.Clear();
}


LB_Bool_T               GemBudInquiry_T::BudPwd_GetSeqFileName(LB_String_T& pa_FileName)
{
    if (m_PwdSeqFileName.IsEmpty() == LB_TRUE)
        return (LB_FALSE);

    pa_FileName = m_PwdSeqFileName;

    return (LB_TRUE);
}


LB_Bool_T               GemBudInquiry_T::BudPwd_IsUserAdmin()
{
    return (m_PwdIsUserAdmin);
}


LB_Bool_T               GemBudInquiry_T::BudPwd_Verify(LB_Word_T pa_OrgID,
                                                       LB_String_T& pa_PwdStr)
{
    LB_IniFile_T IniFile;
    LB_String_T  BudPath;
    LB_String_T  FileName;
    LB_String_T  PwdSection;
    LB_String_T  PwdIdent;
    LB_String_T  PwdValue;

    if (m_GemCtl.Read(pa_OrgID) == LB_FALSE)
        return (LB_FALSE);

    m_GemCtl.GetBudgetPath(BudPath);

    //--- Set File Names
    FileName = BudPath + BUDINQ_PWD_ININAME;
    IniFile.SetFileName(FileName);

    //--- If no password file, create a default
    if (IniFile.Exists() == LB_FALSE)
        BudPwd_CreateNew(pa_OrgID);

    //--- If password still does not exist, there is another problem
    if (IniFile.Exists() == LB_FALSE)
        return (LB_FALSE);

    if (IniFile.Load() == LB_FALSE)
        return (LB_FALSE);

    IniFile.Decode();

    //--- Get Value of ADMIN  
    PwdSection = "PASSWORD-";
    PwdSection = PwdSection + pa_PwdStr;
    PwdIdent   = "Admin";
    if (IniFile.ReadValue(PwdSection,PwdIdent,PwdValue) == LB_FALSE)
        return (LB_FALSE);

    m_PwdIsUserValid = LB_TRUE;

    if (PwdValue == "Y")
        m_PwdIsUserAdmin = LB_TRUE;
    else
        m_PwdIsUserAdmin = LB_FALSE;

    //--- Get Value of SEQFILE
    m_PwdSeqFileName.Clear();
    PwdSection = "PASSWORD-";
    PwdSection = PwdSection + pa_PwdStr;
    PwdIdent   = "SeqFile";
    if (IniFile.ReadValue(PwdSection,PwdIdent,m_PwdSeqFileName) == LB_FALSE)
        return (LB_FALSE);
    if (PwdValue.IsEmpty() == LB_TRUE)
        return (LB_FALSE);

    return (LB_TRUE);
}


LB_Bool_T GemBudInquiry_T::CrpeABRGenDataFile(LB_Word_T    pa_OrgID,
                                              LB_String_T& pa_SeqFileName,
                                              LB_String_T& pa_OutputPath)
{
    LB_String_T    AcctNum;
    LB_String_T    SrcMod;
    LB_String_T    OutFileName;
    LB_String_T    OutputPath;
    LB_String_T    Status(1);
    LB_String_T    BudMastRec(sizeof(m_BudMastRecord));
    LB_String_T    FldValue;
    LB_Bool_T      ReturnFlag;
    int            NextOffset;

    ReturnFlag = LB_TRUE;

    //--- Check output file path
    if (pa_OutputPath.Size() == 0)
        return (LB_FALSE);

    //--- Fix up path
    OutputPath = pa_OutputPath;
    if (OutputPath[OutputPath.Size() - 1] != '\\')
        OutputPath = OutputPath + "\\";

    //--- Clear contents of existing table definitions
    m_ABRTable.Clear();

    //--- Define check table
    m_ABRTable.ColumnDefine("ACCTNUM",  32, XB_COLFLG_STRING);
    m_ABRTable.ColumnDefine("TITLE",    40, XB_COLFLG_STRING);
    m_ABRTable.ColumnDefine("TYPE",      1, XB_COLFLG_STRING);
    m_ABRTable.ColumnDefine("FUND",      4, XB_COLFLG_STRING);
    m_ABRTable.ColumnDefine("GLDISTID",  4, XB_COLFLG_STRING);
    m_ABRTable.ColumnDefine("REENCFLG",  1, XB_COLFLG_STRING);
    m_ABRTable.ColumnDefine("ORIGAPPR", 13, XB_COLFLG_STRING);
    m_ABRTable.ColumnDefine("NETXFERS", 13, XB_COLFLG_STRING);
    m_ABRTable.ColumnDefine("REENCAMT", 13, XB_COLFLG_STRING);
    m_ABRTable.ColumnDefine("NETAPPR",  13, XB_COLFLG_STRING);
    m_ABRTable.ColumnDefine("XPENDYTD", 13, XB_COLFLG_STRING);
    m_ABRTable.ColumnDefine("RCPTSYTD", 13, XB_COLFLG_STRING);
    m_ABRTable.ColumnDefine("UNEXPBAL", 13, XB_COLFLG_STRING);
    m_ABRTable.ColumnDefine("ENCUMYTD", 13, XB_COLFLG_STRING);
    m_ABRTable.ColumnDefine("UNENCBAL", 13, XB_COLFLG_STRING);
    m_ABRTable.ColumnDefine("TOTLVL",    2, XB_COLFLG_STRING);
    m_ABRTable.ColumnDefine("HIPOSN",    1, XB_COLFLG_STRING);
    m_ABRTable.ColumnDefine("HICHAR",    1, XB_COLFLG_STRING);

    //--- Create and Open the Deductions Table
    OutFileName = OutputPath;
    OutFileName = OutFileName + "abr";                          
    m_ABRTable.SetTableName(OutFileName);
    if (m_ABRTable.Create() == LB_FALSE)
    {
        ReturnFlag = LB_FALSE;
        goto Cleanup;
    }

    if (m_ABRTable.Open() == LB_FALSE)
    {
        ReturnFlag = LB_FALSE;
        goto Cleanup;
    }

    //--- Add rows to ABR report table
    //--- Read sequence file for list of budget accounts
    if (m_SeqFile.Open(pa_OrgID, pa_SeqFileName) == LB_FALSE)
    {
        ReturnFlag = LB_FALSE;
        goto Cleanup;
    }

    while (m_SeqFile.ReadNext() == LB_TRUE)
    {
        //--- Make sure record is active
        if (m_SeqFile.GetColumnValue(GEMSEQ_STATUS, Status) == LB_FALSE)
            return (LB_FALSE);

        if (Status != "A")
            continue;

        //--- Retrieve source module for account
        if (m_SeqFile.GetColumnValue(GEMSEQ_SRCMOD, SrcMod) == LB_FALSE)
        {
            ReturnFlag = LB_FALSE;
            goto Cleanup;
        }

        //--- Make sure account is APPROPRIATIONS
        if (SrcMod != "BUD")
            continue;

        //--- Retrieve account number
        if (m_SeqFile.GetColumnValue(GEMSEQ_ACCTNUM, AcctNum) == LB_FALSE)
        {
            ReturnFlag = LB_FALSE;
            goto Cleanup;
        }

        if (m_BudMastList.Retrieve(AcctNum, 0, AcctNum.Size(), BudMastRec) == FALSE)
        {
            char AcctNumStr[128];
            char MsgStr[256];

            AcctNum.AsAsciiZ(AcctNumStr, sizeof(AcctNumStr));
            sprintf(MsgStr, "Acct '%s' not found in budmast list", AcctNumStr);
            LB_Debug_ErrorLog(__FILE__, __LINE__, MsgStr);

            ReturnFlag = LB_FALSE;
            goto Cleanup;
        }

        //--- Extract account details
        //--- (-1) is to account for terminating NULL (ASCIIZ format)
        NextOffset = 0;

        //--- Set account number
        FldValue = BudMastRec.SubstrMid(NextOffset,sizeof(m_BudMastRecord.Account)-1);
        m_ABRTable.SetColumnValue( 0, FldValue);
        FldValue.GetBytes(m_BudMastRecord.Account,sizeof(m_BudMastRecord.Account)-1);
        NextOffset = NextOffset + sizeof(m_BudMastRecord.Account)-1;

        //--- Set title
        FldValue = BudMastRec.SubstrMid(NextOffset,sizeof(m_BudMastRecord.Title)-1);
        m_ABRTable.SetColumnValue( 1, FldValue);
        NextOffset = NextOffset + sizeof(m_BudMastRecord.Title)-1;

        //--- Set type
        FldValue = BudMastRec.SubstrMid(NextOffset,sizeof(m_BudMastRecord.Type)-1);
        m_ABRTable.SetColumnValue( 2, FldValue);
        NextOffset = NextOffset + sizeof(m_BudMastRecord.Type)-1;

        //--- Set Fund
        FldValue = BudMastRec.SubstrMid(NextOffset,sizeof(m_BudMastRecord.Fund)-1);
        m_ABRTable.SetColumnValue( 3, FldValue);
        NextOffset = NextOffset + sizeof(m_BudMastRecord.Fund)-1;

        //--- Set GL Distribution ID
        FldValue = BudMastRec.SubstrMid(NextOffset,sizeof(m_BudMastRecord.GLDistID)-1);
        m_ABRTable.SetColumnValue( 4, FldValue);
        NextOffset = NextOffset + sizeof(m_BudMastRecord.GLDistID)-1;

        //--- Set Reencumbrance Flag
        FldValue = BudMastRec.SubstrMid(NextOffset,sizeof(m_BudMastRecord.ReencFlag)-1);
        m_ABRTable.SetColumnValue( 5, FldValue);
        NextOffset = NextOffset + sizeof(m_BudMastRecord.ReencFlag)-1;

        //--- Set Original Appropriations
        FldValue = BudMastRec.SubstrMid(NextOffset,sizeof(m_BudMastRecord.OrigAppr)-1);
        m_ABRTable.SetColumnValue( 6, FldValue);
        NextOffset = NextOffset + sizeof(m_BudMastRecord.OrigAppr)-1;

        //--- Set Net Transfers
        FldValue = BudMastRec.SubstrMid(NextOffset,sizeof(m_BudMastRecord.NetXFers)-1);
        m_ABRTable.SetColumnValue( 7, FldValue);
        NextOffset = NextOffset + sizeof(m_BudMastRecord.NetXFers)-1;

        //--- Set Reencumbered Amt
        FldValue = BudMastRec.SubstrMid(NextOffset,sizeof(m_BudMastRecord.ReencAmt)-1);
        m_ABRTable.SetColumnValue( 8, FldValue);
        NextOffset = NextOffset + sizeof(m_BudMastRecord.ReencAmt)-1;

        //--- Set Net Appropriation
        FldValue = BudMastRec.SubstrMid(NextOffset,sizeof(m_BudMastRecord.NetAppr)-1);
        m_ABRTable.SetColumnValue( 9, FldValue);
        NextOffset = NextOffset + sizeof(m_BudMastRecord.NetAppr)-1;

        //--- Set Expended YTD
        FldValue = BudMastRec.SubstrMid(NextOffset,sizeof(m_BudMastRecord.ExpendYTD)-1);
        m_ABRTable.SetColumnValue(10, FldValue);
        NextOffset = NextOffset + sizeof(m_BudMastRecord.ExpendYTD)-1;

        //--- Set Receipts YTD
        FldValue = BudMastRec.SubstrMid(NextOffset,sizeof(m_BudMastRecord.ReceiptsYTD)-1);
        m_ABRTable.SetColumnValue(11, FldValue);
        NextOffset = NextOffset + sizeof(m_BudMastRecord.ReceiptsYTD)-1;

        //--- Set Unexpended Balance
        FldValue = BudMastRec.SubstrMid(NextOffset,sizeof(m_BudMastRecord.UnexpendBal)-1);
        m_ABRTable.SetColumnValue(12, FldValue);
        NextOffset = NextOffset + sizeof(m_BudMastRecord.UnexpendBal)-1;

        //--- Set Encumbrances YTD
        FldValue = BudMastRec.SubstrMid(NextOffset,sizeof(m_BudMastRecord.EncumYTD)-1);
        m_ABRTable.SetColumnValue(13, FldValue);
        NextOffset = NextOffset + sizeof(m_BudMastRecord.EncumYTD)-1;

        //--- Set Unencumbered Balance
        FldValue = BudMastRec.SubstrMid(NextOffset,sizeof(m_BudMastRecord.UnencumBal)-1);
        m_ABRTable.SetColumnValue(14, FldValue);
        NextOffset = NextOffset + sizeof(m_BudMastRecord.UnencumBal)-1;

        //--- Set total level
        m_SeqFile.GetColumnValue(GEMSEQ_TOTLEVEL, FldValue);
        m_ABRTable.SetColumnValue(15, FldValue);

        //--- Set total highlight position
        m_SeqFile.GetColumnValue(GEMSEQ_HIPOSN, FldValue);
        m_ABRTable.SetColumnValue(16, FldValue);

        //--- Set total highlight character
        m_SeqFile.GetColumnValue(GEMSEQ_HICHAR, FldValue);
        m_ABRTable.SetColumnValue(17, FldValue); 

        //--- Write columns to new row in table
        if (m_ABRTable.RowAdd() == LB_FALSE)
        {
            LB_Debug_ErrorLog(__FILE__, __LINE__, "add row to table failed");
            ReturnFlag = LB_FALSE;
            goto Cleanup;
        }
    }

    //--- Cleanup
    Cleanup:
        m_SeqFile.Close();
        m_ABRTable.Close();

    return (ReturnFlag);
}

void GemBudInquiry_T::CrpeABRCleanup()
{
    #ifdef LB_NODEBUG
    m_ABRTable.Remove();
    #endif

    m_ABRTable.Clear();
}
