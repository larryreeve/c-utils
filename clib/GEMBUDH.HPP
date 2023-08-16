#ifndef  GEMBUDHIST_HPP
#define  GEMBUDHIST_HPP


#include <lbport.h>
#include <lbstring.hpp>
#include <mftable.hpp>
#include <gemcntrl.hpp>


#define BUDH_COL_ACCT           1
#define BUDH_COL_DATE           2
#define BUDH_COL_YEAR           3
#define BUDH_COL_MONTH          4
#define BUDH_COL_DESC           5
#define BUDH_COL_REF            6
#define BUDH_COL_TYPE           7
#define BUDH_COL_PONUM          8
#define BUDH_COL_INVNUM         9
#define BUDH_COL_VENNUM        10
#define BUDH_COL_POLINE        11
#define BUDH_COL_AMOUNT        12
#define BUDH_COL_CURMTDBAL     13
#define BUDH_COL_CURYTDBAL     14
#define BUDH_COL_RCPTNUM       15
#define BUDH_COL_JRNLNUM       16
#define BUDH_COL_DIST          17


class GemBudHist_T
  {
  public:
      GemBudHist_T();

      ~GemBudHist_T();

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