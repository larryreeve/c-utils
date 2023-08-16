#ifndef  LB_GEMDDBNK_HPP
#define  LB_GEMDDBNK_HPP

#define  GEMDDBNK_CHECKING      27
#define  GEMDDBNK_SAVINGS       37


#include <lbport.h>
#include <lbfio.hpp>
#include <gemcntrl.hpp>
#include <gemddbnk.hpp>

#define GEMDDBNK_COL_RTID       1
#define GEMDDBNK_COL_NAME       2
#define GEMDDBNK_COL_ACCTNUM    3
#define GEMDDBNK_COL_TYPE       4


class GemDDBank_T
  {
  public:
    GemDDBank_T();

    ~GemDDBank_T();

    LB_Bool_T   GetColumnValue  (LB_Word_T pa_Index, LB_String_T& pa_Buf);
    LB_Bool_T   SetColumnValue  (LB_Word_T pa_Index, LB_String_T& pa_Buf);

    LB_Bool_T   Read            (LB_Word_T pa_OrgID);
    LB_Bool_T   Write           (LB_Word_T pa_OrgID);


  private:
    //--- Variables
    LB_Word_T           m_OrgID;
    LB_Bool_T           m_DataReadFlag;
    LB_Fio_T            m_File;
    GemControl_T        m_GemCtl;

    struct BANKINFO_T
      {
      LB_Byte_T         RTID[9];        //--- Routing/Transit, 9th=check digit
      LB_Byte_T         Name[23];
      LB_Byte_T         AcctNum[20];
      LB_Byte_T         Type[2];        //--- 27=checking, 37=savings
      LB_Byte_T         Filler[39];
      LB_Byte_T         Cr;
      LB_Byte_T         DeleteFlag;     //--- 0x0A=active, 0x00=deleted
      } m_BankInfo;

  };
#endif
