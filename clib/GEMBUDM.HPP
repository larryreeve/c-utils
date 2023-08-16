#ifndef  GEMBUDMAST_HPP
#define  GEMBUDMAST_HPP


#include <lbport.h>
#include <lbstring.hpp>
#include <mftable.hpp>
#include <gemcntrl.hpp>


#define BUDM_COL_ACCT           1
#define BUDM_COL_TITLE          2
#define BUDM_COL_ORIGAPPR       3
#define BUDM_COL_TYPE           4
#define BUDM_COL_FUND           5
#define BUDM_COL_EXPENDYTD      6
#define BUDM_COL_UNENCUMBAL     7
#define BUDM_COL_UNEXPENDBAL    8
#define BUDM_COL_ENCUMYTD       9
#define BUDM_COL_NETXFERS       10
#define BUDM_COL_NETAPPR        11
#define BUDM_COL_RECEIPTSYTD    12
#define BUDM_COL_REENCAMT       13
#define BUDM_COL_GLDISTID       14
#define BUDM_COL_REENCFLAG      15


class GemBudMast_T
  {
  public:
      GemBudMast_T();

      ~GemBudMast_T();

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