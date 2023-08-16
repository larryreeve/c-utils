#ifndef  GEMPAYRB_HPP
#define  GEMPAYB_HPP


#include <lbport.h>
#include <lbstring.hpp>
#include <gemcntrl.hpp>
#include <mftable.hpp>


#define PAYRB_COL_ID            1
#define PAYRB_COL_REC           2
#define PAYRB_COL_INDEX         3
#define PAYRB_COL_BANKRTID      4
#define PAYRB_COL_BANKNAME      5


class GemPayRefBank_T
  {
  public:
        GemPayRefBank_T();

        ~GemPayRefBank_T();

        LB_Bool_T               Open(LB_Word_T pa_OrgID);

        void                    Close();

        LB_Bool_T               Find(LB_String_T& pa_AcctNum);

        LB_Bool_T               ReadNext();

        LB_Bool_T               GetColumnValue(LB_Word_T    pa_ColID,
                                               LB_String_T& pa_String);

  private:
        GemControl_T            m_GemCtl;
        MF_Table_T              m_DataFile;
  };
#endif
