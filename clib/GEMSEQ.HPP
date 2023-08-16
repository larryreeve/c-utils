#ifndef  LB_GEMSEQ_HPP
#define  LB_GEMSEQ_HPP

#include <lbport.h>
#include <lbfio.hpp>
#include <gemcntrl.hpp>

//--- Index Values
#define GEMSEQ_SRCMOD       1
#define GEMSEQ_ACCTNUM      2
#define GEMSEQ_TOTLEVEL     5
#define GEMSEQ_HIPOSN       7
#define GEMSEQ_HICHAR       8
#define GEMSEQ_STATUS       9

class GemSeqFile_T
{
    public:
        ~GemSeqFile_T();

        LB_Bool_T                   Open(LB_Word_T   pa_OrgID,
                                         LB_String_T pa_SeqFileName);

        void                        Close();

        LB_Bool_T                   GetColumnValue(LB_Word_T    pa_ColID,
                                                   LB_String_T& pa_Value);

        LB_Bool_T                   ReadNext();


    private:
        //--- Variables
        LB_Fio_T            m_File;
        GemControl_T        m_GemCtl;

        struct
        {
            LB_Byte_T Source[3];
            LB_Byte_T Account[32];
            LB_Byte_T PrintFlag;
            LB_Byte_T AdvanceBy;
            LB_Byte_T TotalLevel;
            LB_Byte_T ClearTotals;
            LB_Byte_T HighlightPosn;
            LB_Byte_T HighlightChar;
            LB_Byte_T Status;
            LB_Byte_T Filler[18];
            LB_Byte_T Cr;
            LB_Byte_T DeleteFlag;
      } m_SeqFileRec;
};
#endif
