/*  gemseq.cpp
 *
 *  Sequence File routines
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
 *      03/14/96        LR      -Initial Release
 *      06/22/97        LR      -See Visual SourceSafe History
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
#include <lbcblcmp.hpp>
#include <lbdebug.hpp>
#include <lbfio.hpp>
#include <lbstring.hpp>
#include <gemseq.hpp>



//---------------------------------------------------------------
//---                   Private Constants                     ---
//---------------------------------------------------------------
#define SEQ_SRC_BUD             'B'
#define SEQ_SRC_GEN             'G'
#define SEQ_SRC_REV             'R'

#define SEQ_STAT_ACTIVE         'A'
#define SEQ_STAT_INACTIVE       'I'
#define SEQ_STAT_DELETED        'D'

LB_ASSERTFILE(__FILE__)


//---------------------------------------------------------------
//---                   Public  Methods                       ---
//---------------------------------------------------------------
GemSeqFile_T::~GemSeqFile_T()
{
    m_File.Close();
}


void                    GemSeqFile_T::Close()
{
    m_File.Close();
}


LB_Bool_T               GemSeqFile_T::GetColumnValue(LB_Word_T    pa_ColID,
                                                     LB_String_T& pa_Value)
{
    switch(pa_ColID)
    {
        case GEMSEQ_SRCMOD:
            pa_Value.SetBytes(m_SeqFileRec.Source, sizeof(m_SeqFileRec.Source));
            break;

        case GEMSEQ_ACCTNUM:
            pa_Value.SetBytes(m_SeqFileRec.Account, sizeof(m_SeqFileRec.Account));
            break;

        case GEMSEQ_TOTLEVEL:
        {
            LB_CobolComp_T CobolValue;
            LB_String_T    RawValue;
            LB_FloatPt_T   Result;
            char           ResultStr[16];

            //--- Value in sequence file is a 2-byte field packed into 1 byte
            //--- by Cobol. Unpack the byte and return a 2-byte ASCII representation.
            RawValue.SetBytes(&m_SeqFileRec.TotalLevel, 1);
            CobolValue.Decode(&Result, RawValue, 0, LB_FALSE);
            sprintf(ResultStr, "%02.0f", Result);
            pa_Value = ResultStr;
            break;
        }

        case GEMSEQ_HIPOSN:
            pa_Value.SetBytes(&m_SeqFileRec.HighlightPosn, sizeof(m_SeqFileRec.HighlightPosn));
            break;

        case GEMSEQ_HICHAR:
            pa_Value.SetBytes(&m_SeqFileRec.HighlightChar, sizeof(m_SeqFileRec.HighlightChar));
            break;

        case GEMSEQ_STATUS:
            pa_Value.SetBytes(&m_SeqFileRec.Status, sizeof(m_SeqFileRec.Status));
            break;

        default:
            LB_Debug_ErrorLog(__FILE__, __LINE__, "unknown column id: %d", pa_ColID);
            pa_Value.Clear();
            return (LB_FALSE);
    }

    return (LB_TRUE);
}


LB_Bool_T               GemSeqFile_T::Open(LB_Word_T   pa_OrgID,
                                           LB_String_T pa_FileName)
{
    LB_String_T BudPathStr;

    m_File.Close();

    if (pa_OrgID <= 0)
        return(LB_FALSE);

    //--- Get Budget Path
    if (m_GemCtl.Read(pa_OrgID) == LB_FALSE)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "GemCtlRead failed, orgid=%d", pa_OrgID);
        return (LB_FALSE);
    }

    m_GemCtl.GetBudgetPath(BudPathStr);
    BudPathStr = BudPathStr + pa_FileName;

    m_File.SetFileName(BudPathStr);

    if (m_File.Exists() == LB_FALSE)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "seqfile does not exist");
        return (LB_FALSE);
    }

    if (m_File.Open(FIO_OM_RO_BUF) == LB_FALSE)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "seqfile failed to open");
        return (LB_FALSE);
    }

    m_File.SeekFromStart(0);

    return (LB_TRUE);
}


LB_Bool_T               GemSeqFile_T::ReadNext()
{
    LB_String_T IOBuffer;

    if (m_File.IsOpen() == LB_FALSE)
        return (LB_FALSE);

    while (m_File.IsEOF() == LB_FALSE)
    {
        IOBuffer.PadRight(sizeof(m_SeqFileRec),0x20);

        if (m_File.Read(IOBuffer) != sizeof(m_SeqFileRec))
            return (LB_FALSE);

        IOBuffer.GetBytes((LB_Byte_T *) &m_SeqFileRec, sizeof(m_SeqFileRec));

        //--- Skip deleted records in file
        //--- 0x0A = active, 0x00 = deleted
        if (m_SeqFileRec.DeleteFlag == 0x0A)
            return (LB_TRUE);
    }

    return (LB_FALSE);
}
