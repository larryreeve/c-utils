/*  gemddbnk.cpp
 *
 *  Direct Deposit - Bank Processing routines
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
 *      01/30/96        LR      -Initial Release
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
#include <gemddbnk.hpp>


/*---------------------------------------------------------------
  ---                   Private Constants                     ---
  ---------------------------------------------------------------*/
#define BNK_FN  "ACHBANK.DAT"


/*---------------------------------------------------------------
  ---                   Public  Methods                       ---
  ---------------------------------------------------------------*/
/*
 *
**/
GemDDBank_T::GemDDBank_T()
  {
  m_OrgID        = 1;
  m_DataReadFlag = LB_FALSE;
  }


/*
 *
**/
GemDDBank_T::~GemDDBank_T()
  {
  m_File.Close();
  }


/*
 *
**/
LB_Bool_T               GemDDBank_T::GetColumnValue(LB_Word_T    pa_Index,
                                                    LB_String_T& pa_Buf)
  {
  pa_Buf.Clear();

  if (m_DataReadFlag == LB_FALSE)
    return (LB_FALSE);

  switch(pa_Index)
    {
    case GEMDDBNK_COL_RTID:
      pa_Buf.SetBytes((LB_Byte_T *) m_BankInfo.RTID,sizeof(m_BankInfo.RTID));
      break;

    case GEMDDBNK_COL_NAME:
      pa_Buf.SetBytes((LB_Byte_T *) m_BankInfo.Name,sizeof(m_BankInfo.Name));
      break;

    case GEMDDBNK_COL_ACCTNUM:
      pa_Buf.SetBytes((LB_Byte_T *) m_BankInfo.AcctNum,sizeof(m_BankInfo.AcctNum));
      break;

    case GEMDDBNK_COL_TYPE:
      pa_Buf.SetBytes((LB_Byte_T *) m_BankInfo.Type,sizeof(m_BankInfo.Type));
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
LB_Bool_T               GemDDBank_T::Read(LB_Word_T pa_OrgID)
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
  l_String.Append((LB_CZString_T) BNK_FN);
  m_File.SetFileName(l_String);

  if (m_File.Exists() == LB_FALSE)
    return (LB_FALSE);

  if (m_File.Open(FIO_OM_RO_BUF) == LB_FALSE)
    return (LB_FALSE);

  m_File.SeekFromStart(0);
  l_String.Clear(); l_String.PadRight(sizeof(m_BankInfo),0x20);
  for (l_Index=0; l_Index < pa_OrgID; l_Index++)
    l_BytesRead = m_File.Read(l_String);

  m_File.Close();

  if (l_BytesRead == sizeof(m_BankInfo))
    {
    m_DataReadFlag = LB_TRUE;
    l_String.GetBytes((LB_Byte_T *) &m_BankInfo,sizeof(m_BankInfo));
    return (LB_TRUE);
    }
  else
    return (LB_FALSE);
  }


/*
 *
**/
LB_Bool_T               GemDDBank_T::SetColumnValue(LB_Word_T    pa_Index,
                                                    LB_String_T& pa_Buf)
  {
  switch(pa_Index)
    {
    case GEMDDBNK_COL_RTID:
      memset((LB_Byte_T *) m_BankInfo.RTID,0x20,sizeof(m_BankInfo.RTID));
      pa_Buf.GetBytes((LB_Byte_T *) m_BankInfo.RTID,sizeof(m_BankInfo.RTID));
      break;

    case GEMDDBNK_COL_NAME:
      memset((LB_Byte_T *) m_BankInfo.Name,0x20,sizeof(m_BankInfo.Name));
      pa_Buf.GetBytes((LB_Byte_T *) m_BankInfo.Name,sizeof(m_BankInfo.Name));
      break;

    case GEMDDBNK_COL_ACCTNUM:
      memset((LB_Byte_T *) m_BankInfo.AcctNum,0x20,sizeof(m_BankInfo.AcctNum));
      pa_Buf.GetBytes((LB_Byte_T *) m_BankInfo.AcctNum,sizeof(m_BankInfo.AcctNum));
      break;

    case GEMDDBNK_COL_TYPE:
      memset((LB_Byte_T *) m_BankInfo.Type,0x20,sizeof(m_BankInfo.Type));
      pa_Buf.GetBytes((LB_Byte_T *) m_BankInfo.Type,sizeof(m_BankInfo.Type));
      break;
  
    default:
      return (LB_FALSE);
    }

  return (LB_TRUE);
  }



/*
 *
**/
LB_Bool_T               GemDDBank_T::Write(LB_Word_T pa_OrgID)
  {
  LB_Word_T     l_BytesWritten;
  LB_String_T   l_String;

  if (pa_OrgID == 0)
    return (LB_FALSE);

  if (m_GemCtl.Read(pa_OrgID) == LB_FALSE)
    return (LB_FALSE);

  //--* Initialize filler fields
  memset(m_BankInfo.Filler,0x20,sizeof(m_BankInfo.Filler));
  m_BankInfo.Cr         = 0x0D;
  m_BankInfo.DeleteFlag = 0x0A;

  //--* Get path to bank file
  m_GemCtl.GetPayrollPath(l_String);
  l_String.Append((LB_CZString_T) BNK_FN);
  m_File.SetFileName(l_String);

  if (m_File.Exists() == LB_FALSE)
    {
    if (m_File.Create() == LB_FALSE)
      return (LB_FALSE);
    }

  if (m_File.Open(FIO_OM_RW_BUF) == LB_FALSE)
    return (LB_FALSE);

  l_String.Clear(); l_String.PadRight(sizeof(m_BankInfo),0x20);
  l_String.SetBytes((LB_Byte_T *) &m_BankInfo,sizeof(m_BankInfo));
  m_File.SeekFromStart((pa_OrgID-1) * sizeof(m_BankInfo));
  l_BytesWritten = m_File.Write(l_String);

  m_File.Close();

  if (l_BytesWritten == sizeof(m_BankInfo))
    return (LB_TRUE);
  else
    return (LB_FALSE);
  }
