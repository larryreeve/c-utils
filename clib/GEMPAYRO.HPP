#ifndef  GEMPAYREF_HPP
#define  GEMPAYREF_HPP


#include <lbport.h>
#include <lbstring.hpp>
#include <gemcntrl.hpp>
#include <mftable.hpp>

#define PAYRO_COL_ORGNAME       1
#define PAYRO_COL_ORGPHONE      2
#define PAYRO_COL_FEDID         3


class GemPayRefOrg_T
  {
  public:
        GemPayRefOrg_T();

        ~GemPayRefOrg_T();

        LB_Bool_T               Open(LB_Word_T pa_OrgID);

        void                    Close();

        LB_Bool_T               ReadNext();

        LB_Bool_T               GetColumnValue(LB_Word_T    pa_ColID,
                                               LB_String_T& pa_Value);

  private:
        GemControl_T            m_GemCtl;
        MF_Table_T              m_DataFile;
  };
#endif
