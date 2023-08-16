#ifndef  GEMPAYMAST_HPP
#define  GEMPAYMAST_HPP


#include <lbport.h>
#include <lbstring.hpp>
#include <gemcntrl.hpp>
#include <mftable.hpp>

#define PAYM_COL_EMPNAME        1
#define PAYM_COL_EMPNUM         2
#define PAYM_COL_DDACCT         3
#define PAYM_COL_DDBANKNUM      4
#define PAYM_COL_USER1          5
#define PAYM_COL_USER10         6


class GemPayMast_T
  {
  public:
        GemPayMast_T();

        ~GemPayMast_T();

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