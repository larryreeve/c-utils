#ifndef  GEMPAYHIST_HPP
#define  GEMPAYHIST_HPP


#include <lbport.h>
#include <lbstring.hpp>
#include <gemcntrl.hpp>
#include <mftable.hpp>


#define PAYH_COL_EMPNUM         1
#define PAYH_COL_WEEK           2
#define PAYH_COL_EXTRA          3
#define PAYH_COL_NET            4
#define PAYH_COL_GROSS          5
#define PAYH_COL_REMOVED        6
#define PAYH_COL_MANUAL         7


class GemPayHist_T
  {
  public:
        GemPayHist_T();

        ~GemPayHist_T();

        LB_Bool_T               Open(LB_Word_T pa_OrgID);

        void                    Close();

        LB_Bool_T               ReadNext();

        LB_Bool_T               Restart();

        LB_Bool_T               GetColumnValue(LB_Word_T pa_ColID,
                                               LB_String_T& pa_String);

        LB_Bool_T               SearchNext(LB_String_T& pa_EmpNum,
                                           LB_String_T& pa_Week,
                                           LB_String_T& pa_Extra);

  private:
        GemControl_T            m_GemCtl;
        MF_Table_T              m_DataFile;
  };
#endif
