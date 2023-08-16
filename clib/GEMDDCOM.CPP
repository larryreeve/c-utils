/*  gemddcom.cpp
 *
 *  Direct Deposit - Communication Processing routines
 *
 *
 *                       * PROPRIETARY PROGRAM MATERIAL *
 *
 *      NOTICE: THIS MATERIAL IS PROPRIETARY TO UNITED COMPUTER
 *      AND IS NOT TO BE REPRODUCED, USED OR DISCLOSED EXCEPT
 *      UPON WRITTEN AUTHORIZATION OF:
 *
 *                      UNITED COMPUTER SALES & SERVICE, INC.
 *                      100 DOBBS LANE, SUITE 208
 *                      CHERRY HILL, NEW JERSEY 08034
 *                      609.795.7330
 *
 *            COPYRIGHT (C) 1995-1996 UNITED COMPUTER SALES & SERVICE, INC.
 *
 *
 * Revision History:
 *
 *      Date            By      Notes
 *      --------        ---     ------------------------------------------
 *      01/31/96        LR      -Initial Release
 *      05/01/96        LR      -Expanded modem field from 16 to 32 bytes
 *                              -Added scriptname field
 *      05/13/96        LR      -Removed duplex & protocol comm options
 *
 * Notes:
 *
 *
**/


/*---------------------------------------------------------------
  ---                INCLUDE FILES                            ---
  ---------------------------------------------------------------*/
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <time.h>

#include <lbport.h>
#include <lbfio.hpp>
#include <gemcntrl.hpp>
#include <gemddcom.hpp>


/*---------------------------------------------------------------
  ---                   Private Constants                     ---
  ---------------------------------------------------------------*/
#define COMM_FN "ACHCOMM.DAT"


/*---------------------------------------------------------------
  ---                   Public  Methods                       ---
  ---------------------------------------------------------------*/
/*
 *
**/
GemDDComm_T::GemDDComm_T()
  {
  m_OrgID           = 1;
  m_DataReadFlag    = LB_FALSE;
  }


/*
 *
**/
GemDDComm_T::~GemDDComm_T()
  {
  m_File.Close();
  }


/*
 *--- Returns a NULL-Terminated string with the value of the indicated index.
 *
 * NOTE: RecOffset requires an address; some members of the structure are
 *       single bytes, while others are arrays. Pay attention!
 *
**/
LB_Bool_T               GemDDComm_T::GetColumnValue(LB_Word_T pa_Index,
                                                    LB_String_T& pa_Buf)
  {
  pa_Buf.Clear();

  if (m_DataReadFlag == LB_FALSE)
    return (LB_FALSE);

  switch(pa_Index)
    {
    case GEMDDCOM_COL_PORTID:
      pa_Buf.SetBytes((LB_Byte_T *) m_CommInfo.PortID,sizeof(m_CommInfo.PortID));
      break;

    case GEMDDCOM_COL_MODEMID:
      pa_Buf.SetBytes((LB_Byte_T *) m_CommInfo.ModemID,sizeof(m_CommInfo.ModemID));
      break;

    case GEMDDCOM_COL_SCRIPT:
      pa_Buf.SetBytes((LB_Byte_T *) m_CommInfo.ScriptName,sizeof(m_CommInfo.ScriptName));
      break;

    case GEMDDCOM_COL_SPEED:
      pa_Buf.SetBytes((LB_Byte_T *) m_CommInfo.Speed,sizeof(m_CommInfo.Speed));
      break;

    case GEMDDCOM_COL_DATABITS:
      pa_Buf.SetBytes((LB_Byte_T *) &m_CommInfo.DataBits,sizeof(m_CommInfo.DataBits));
      break;

    case GEMDDCOM_COL_STOPBITS:
      pa_Buf.SetBytes((LB_Byte_T *) &m_CommInfo.StopBits,sizeof(m_CommInfo.StopBits));
      break;

    case GEMDDCOM_COL_PARITY:
      pa_Buf.SetBytes((LB_Byte_T *) &m_CommInfo.Parity,sizeof(m_CommInfo.Parity));
      break;

    default:
      return (LB_FALSE);
    }

  pa_Buf.TrimRight();

  return (LB_TRUE);
  }


/*
 *
**/
LB_Bool_T               GemDDComm_T::Read(LB_Word_T pa_OrgID)
  {
  LB_Word_T     l_BytesRead;
  LB_Word_T     l_Index;
  LB_String_T   l_String;

  m_DataReadFlag = LB_FALSE;

  if (pa_OrgID == 0)
    return (LB_FALSE);

  if (m_GemCtl.Read(pa_OrgID) == LB_FALSE)
    return (LB_FALSE);

  m_GemCtl.GetPayrollPath(l_String);
  l_String.Append((LB_CZString_T) COMM_FN);
  m_File.SetFileName(l_String);

  if (m_File.Exists() == LB_FALSE)
    return (LB_FALSE);

  if (m_File.Open(FIO_OM_RO_BUF) == LB_FALSE)
    return (LB_FALSE);

  m_File.SeekFromStart(0);
  l_String.Clear();
  l_String.PadRight(sizeof(m_CommInfo),0x20);
  for (l_Index=0; l_Index < pa_OrgID; l_Index++)
    l_BytesRead = m_File.Read(l_String);

  m_File.Close();

  if (l_BytesRead == sizeof(m_CommInfo))
    {
    m_DataReadFlag = LB_TRUE;
    l_String.GetBytes((LB_Byte_T *)&m_CommInfo,sizeof(m_CommInfo));
    return (LB_TRUE);
    }
  else
    return (LB_FALSE);
  }


/*
 *
 * NOTE: RecOffset requires an address; some members of the structure are
 *       single bytes, while others are arrays. Pay attention!
 *
**/
LB_Bool_T               GemDDComm_T::SetColumnValue(LB_Word_T pa_Index,
                                                    LB_String_T& pa_Buf)
  {
  switch(pa_Index)
    {
    case GEMDDCOM_COL_PORTID:
      memset((LB_Byte_T *) m_CommInfo.PortID,0x20,sizeof(m_CommInfo.PortID));
      pa_Buf.GetBytes((LB_Byte_T *) m_CommInfo.PortID,sizeof(m_CommInfo.PortID));
      break;

    case GEMDDCOM_COL_MODEMID:
      memset((LB_Byte_T *) m_CommInfo.ModemID,0x20,sizeof(m_CommInfo.ModemID));
      pa_Buf.GetBytes((LB_Byte_T *) m_CommInfo.ModemID,sizeof(m_CommInfo.ModemID));
      break;

    case GEMDDCOM_COL_SCRIPT:
      memset((LB_Byte_T *) m_CommInfo.ScriptName,0x20,sizeof(m_CommInfo.ScriptName));
      pa_Buf.GetBytes((LB_Byte_T *) m_CommInfo.ScriptName,sizeof(m_CommInfo.ScriptName));
      break;

    case GEMDDCOM_COL_SPEED:
      memset((LB_Byte_T *) m_CommInfo.Speed,0x20,sizeof(m_CommInfo.Speed));
      pa_Buf.GetBytes((LB_Byte_T *) m_CommInfo.Speed,sizeof(m_CommInfo.Speed));
      break;

    case GEMDDCOM_COL_DATABITS:
      memset((LB_Byte_T *) &m_CommInfo.DataBits,0x20,sizeof(m_CommInfo.DataBits));
      pa_Buf.GetBytes((LB_Byte_T *) &m_CommInfo.DataBits,sizeof(m_CommInfo.DataBits));
      break;

    case GEMDDCOM_COL_STOPBITS:
      memset((LB_Byte_T *) &m_CommInfo.StopBits,0x20,sizeof(m_CommInfo.StopBits));
      pa_Buf.GetBytes((LB_Byte_T *) &m_CommInfo.StopBits,sizeof(m_CommInfo.StopBits));
      break;

    case GEMDDCOM_COL_PARITY:
      memset((LB_Byte_T *) &m_CommInfo.Parity,0x20,sizeof(m_CommInfo.Parity));
      pa_Buf.GetBytes((LB_Byte_T *) &m_CommInfo.Parity,sizeof(m_CommInfo.Parity));
      break;

    default:
      return (LB_FALSE);
    }

  return (LB_TRUE);
  }


/*
 *
**/
LB_Bool_T               GemDDComm_T::Write(LB_Word_T pa_OrgID)
  {
  LB_Word_T     l_BytesWritten;
  LB_String_T   l_String;

  if (pa_OrgID == 0)
    return (LB_FALSE);

  if (m_GemCtl.Read(pa_OrgID) == LB_FALSE)
    return (LB_FALSE);

  //--* Initialize filler fields
  m_CommInfo.Cr         = 0x0D;
  m_CommInfo.DeleteFlag = 0x0A;

  //--* Get path to bank file
  m_GemCtl.GetPayrollPath(l_String);
  l_String.Append((LB_CZString_T) COMM_FN);
  m_File.SetFileName(l_String);

  if (m_File.Exists() == LB_FALSE)
    {
    if (m_File.Create() == LB_FALSE)
      return (LB_FALSE);
    }

  if (m_File.Open(FIO_OM_RW_BUF) == LB_FALSE)
    return (LB_FALSE);

  l_String.Clear();
  l_String.PadRight(sizeof(m_CommInfo),0x20);
  l_String.SetBytes((LB_Byte_T *) &m_CommInfo,sizeof(m_CommInfo));
  m_File.SeekFromStart((pa_OrgID-1) * sizeof(m_CommInfo));
  l_BytesWritten = m_File.Write(l_String);

  m_File.Close();

  if (l_BytesWritten == sizeof(m_CommInfo))
    return (LB_TRUE);
  else
    return (LB_FALSE);
  }
