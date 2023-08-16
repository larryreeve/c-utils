#ifndef  LB_GEMDDCOM_HPP
#define  LB_GEMDDCOM_HPP

#include <lbport.h>
#include <lbfio.hpp>
#include <gemcntrl.hpp>
#include <gemddbnk.hpp>

#define GEMDDCOM_COL_PORTID      1
#define GEMDDCOM_COL_MODEMID     2
#define GEMDDCOM_COL_SCRIPT      3
#define GEMDDCOM_COL_SPEED       4
#define GEMDDCOM_COL_DATABITS    5
#define GEMDDCOM_COL_STOPBITS    6
#define GEMDDCOM_COL_PARITY      7

class GemDDComm_T
  {
  public:
    GemDDComm_T();

    ~GemDDComm_T();

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

    struct COMMINFO_T
      {
      LB_Byte_T         PortID[16];
      LB_Byte_T         ModemID[32];
      LB_Byte_T         ScriptName[16];
      LB_Byte_T         Speed[6];
      LB_Byte_T         DataBits;
      LB_Byte_T         StopBits;
      LB_Byte_T         Parity;
      LB_Byte_T         Cr;
      LB_Byte_T         DeleteFlag; //--- 0x0A=active, 0x00=deleted
      } m_CommInfo;
  };
#endif
