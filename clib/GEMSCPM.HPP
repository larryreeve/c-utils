#ifndef  GEMBSCPMAST_HPP
#define  GEMSCPMAST_HPP


#include <lbport.h>
#include <lbstring.hpp>
#include <mftable.hpp>
#include <gemcntrl.hpp>


#define SCPM_COL_BANK           1
#define SCPM_COL_STATUS         2


class GemSCPMast_T
  {
  public:
        GemSCPMast_T();

        ~GemSCPMast_T();

        LB_Bool_T               Open(LB_Word_T pa_OrgID);

        void                    Close();

        LB_Bool_T               ReadNext();

        LB_Bool_T               Restart();

        LB_Bool_T               GetColumnValue(LB_Word_T    pa_ColID,
                                               LB_String_T& pa_Value);

  private:
        GemControl_T            m_GemCtl;
        MF_Table_T              m_DataFile;
  };
#endif
