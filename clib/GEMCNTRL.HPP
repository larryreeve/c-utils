#ifndef  LB_GEMCTL_HPP
#define  LB_GEMCTL_HPP


#include <lbport.h>
#include <lbfio.hpp>
#include <lbstring.hpp>

#define  GEMCTL_MAXPATHSIZE  24


class GemControl_T
  {
  public:
    GemControl_T();

    ~GemControl_T();

    LB_Bool_T           Read            (LB_Word_T      pa_OrgID);

    LB_Bool_T           GetAccountName  (LB_String_T&   pa_Buf);

    LB_Bool_T           GetBudgetPath   (LB_String_T&   pa_Buf);

    LB_Bool_T           GetPayrollPath  (LB_String_T&   pa_Buf);

    LB_Bool_T           GetPrinter      (LB_String_T&   pa_Buf);


  private:
    LB_Bool_T           m_DataReadFlag;
    LB_Fio_T            m_File;

    struct m_GemCtlRecord_T
      {
      LB_Byte_T         Paths[30][GEMCTL_MAXPATHSIZE];
      LB_Byte_T         Printer[6];
      LB_Byte_T         AccountName[40];
      LB_Byte_T         SysPurchases[30];
      LB_Byte_T         AnnualCheck[3];
      LB_Byte_T         AnnualDate[3];
      LB_Byte_T         CheckSum[3];
      LB_Byte_T         Passwords[30][4];
      LB_Byte_T         Filler1[81];
      LB_Byte_T         CheckSum2[3];
      LB_Byte_T         Filler2[7];
      LB_Byte_T         Cr;
      LB_Byte_T         DeletedFlag;      //--- 0x0A=active, 0x00=deleted
      } m_GemCtlRec;
  };
#endif
