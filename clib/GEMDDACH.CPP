/*  gemddach.cpp
 *
 *  Direct Deposit - ACH File routines
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
 *      02/05/96        LR      -Initial Release
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
#include <lbdate.hpp>
#include <lbdebug.hpp>
#include <lbtime.hpp>
#include <lbfio.hpp>
#include <gemcntrl.hpp>
#include <gemddach.hpp>



/*---------------------------------------------------------------
  ---                   Private Constants                     ---
  ---------------------------------------------------------------*/
//--* Record Types
#define RT_FH   '1'             // File Header
#define RT_BH   '5'             // Batch Header
#define RT_DT   '6'             // Detail
#define RT_BT   '8'             // Batch Trailer
#define RT_FT   '9'             // File Trailer

//--* File Names
#define ACH_FN  "ACHDEPOS.DAT"
LB_ASSERTFILE(__FILE__)


/*---------------------------------------------------------------
  ---                   Private Methods                       ---
  ---------------------------------------------------------------*/
/*
 *
**/
void                    GemDDACH_T::m_ClearRecord()
  {
  memset(&m_AchInfo.DT,' ',sizeof(m_AchInfo.DT));
  }


/*---------------------------------------------------------------
  ---                   Public  Methods                       ---
  ---------------------------------------------------------------*/
/*
 *
**/
GemDDACH_T::GemDDACH_T()
  {
  m_OrgID = 1;
  }


/*
 *
**/
GemDDACH_T::~GemDDACH_T()
  {
  m_File.Close();
  }


/*
 *
**/
void                    GemDDACH_T::Close()
  {
  m_File.Close();
  }


/*---------------------------------------------------------------
  ---                   GET Methods                           ---
  ---------------------------------------------------------------*/


/*
 *
**/
LB_Bool_T               GemDDACH_T::GetBHInfo(LB_Word_T    pa_Index,
                                              LB_String_T& pa_Buf)
  {
  switch(pa_Index)
    {
    case GEMDDACH_BH_SVCCLS:
      pa_Buf.SetBytes(m_AchInfo.BH.SvcCls,     sizeof(m_AchInfo.BH.SvcCls));
      break;

    case GEMDDACH_BH_ORIGNAME:
      pa_Buf.SetBytes(m_AchInfo.BH.OrigName,   sizeof(m_AchInfo.BH.OrigName));
      break;

    case GEMDDACH_BH_ORIGID:
      pa_Buf.SetBytes(&m_AchInfo.BH.OrigID[1], sizeof(m_AchInfo.BH.OrigID)-1);
      break;

    case GEMDDACH_BH_CLSCD:
      pa_Buf.SetBytes(m_AchInfo.BH.ClsCD,      sizeof(m_AchInfo.BH.ClsCD));
      break;

    case GEMDDACH_BH_DESCENTRY:
      pa_Buf.SetBytes(m_AchInfo.BH.DescEntry,  sizeof(m_AchInfo.BH.DescEntry));
      break;

    case GEMDDACH_BH_DESCDATE:
      pa_Buf.SetBytes(m_AchInfo.BH.DescDate,   sizeof(m_AchInfo.BH.DescDate));
      break;

    case GEMDDACH_BH_EFFDATE:
      pa_Buf.SetBytes(m_AchInfo.BH.EffDate,    sizeof(m_AchInfo.BH.EffDate));
      break;

    case GEMDDACH_BH_DESTID:
      pa_Buf.SetBytes(m_AchInfo.BH.DestID,     sizeof(m_AchInfo.BH.DestID));
      break;

    case GEMDDACH_BH_BATCH:
      pa_Buf.SetBytes(m_AchInfo.BH.Batch,      sizeof(m_AchInfo.BH.Batch));
      break;

    default:
      pa_Buf.Clear();
      return (LB_FALSE);
    }

  return (LB_TRUE);
  }



/*
 *
**/
LB_Bool_T               GemDDACH_T::GetBTInfo(LB_Word_T    pa_Index,
                                              LB_String_T& pa_Buf)
  {
  switch(pa_Index)
    {
    case GEMDDACH_BT_SVCCLS:
      pa_Buf.SetBytes(m_AchInfo.BT.SvcCls,    sizeof(m_AchInfo.BT.SvcCls));
      break;

    case GEMDDACH_BT_NUMDET:
      pa_Buf.SetBytes(m_AchInfo.BT.NumDet,    sizeof(m_AchInfo.BT.NumDet));
      break;

    case GEMDDACH_BT_HASH:
      pa_Buf.SetBytes(m_AchInfo.BT.HashValue, sizeof(m_AchInfo.BT.HashValue));
      break;

    case GEMDDACH_BT_TOTDB:
      pa_Buf.SetBytes(m_AchInfo.BT.TotDB,     sizeof(m_AchInfo.BT.TotDB));
      break;

    case GEMDDACH_BT_TOTCR:
      pa_Buf.SetBytes(m_AchInfo.BT.TotCR,     sizeof(m_AchInfo.BT.TotCR));
      break;

    case GEMDDACH_BT_ORIGID:
      pa_Buf.SetBytes(m_AchInfo.BT.OrigID,    sizeof(m_AchInfo.BT.OrigID));
      break;

    case GEMDDACH_BT_DESTID:
      pa_Buf.SetBytes(m_AchInfo.BT.DestID,    sizeof(m_AchInfo.BT.DestID));
      break;

    case GEMDDACH_BT_BATCH:
      pa_Buf.SetBytes(m_AchInfo.BT.Batch,     sizeof(m_AchInfo.BT.Batch));
      break;

    default:
      pa_Buf.Clear();
      return (LB_FALSE);
    }

  return (LB_TRUE);
  }


/*
 *
**/
LB_Bool_T               GemDDACH_T::GetDTInfo(LB_Word_T    pa_Index,
                                              LB_String_T& pa_Buf)
  {
  switch(pa_Index)
    {
    case GEMDDACH_DT_TRANSCD:
      pa_Buf.SetBytes(m_AchInfo.DT.TransCD,     sizeof(m_AchInfo.DT.TransCD));
      break;

    case GEMDDACH_DT_BANKID:
      pa_Buf.SetBytes(m_AchInfo.DT.BankID,      sizeof(m_AchInfo.DT.BankID));
      break;

    case GEMDDACH_DT_BANKACCTNUM:
      pa_Buf.SetBytes(m_AchInfo.DT.BankAcctNum, sizeof(m_AchInfo.DT.BankAcctNum));
      pa_Buf.TrimRight();
      break;

    case GEMDDACH_DT_NET:
      pa_Buf.SetBytes(m_AchInfo.DT.Net,         sizeof(m_AchInfo.DT.Net));
      break;

    case GEMDDACH_DT_EMPNUM:
      pa_Buf.SetBytes(m_AchInfo.DT.EmpNum,      sizeof(m_AchInfo.DT.EmpNum));
      break;

    case GEMDDACH_DT_EMPNAME:
      pa_Buf.SetBytes(m_AchInfo.DT.EmpName,     sizeof(m_AchInfo.DT.EmpName));
      break;

    case GEMDDACH_DT_DISCRINFO:
      pa_Buf.SetBytes(m_AchInfo.DT.DiscrInfo,   sizeof(m_AchInfo.DT.DiscrInfo));
      break;

    case GEMDDACH_DT_ADDENDA:
      pa_Buf.SetBytes(&m_AchInfo.DT.Addenda,    sizeof(m_AchInfo.DT.Addenda));
      break;

    case GEMDDACH_DT_TRACE:
      pa_Buf.SetBytes(m_AchInfo.DT.Trace,       sizeof(m_AchInfo.DT.Trace));
      break;

    default:
      pa_Buf.Clear();
      return (LB_FALSE);
    }

  return (LB_TRUE);
  }


/*
 *
**/
LB_Bool_T               GemDDACH_T::GetFIInfo(LB_Word_T pa_Index,
                                              LB_String_T& pa_Buf)
  {
  switch(pa_Index)
    {
    case GEMDDACH_FI_PATH:
      m_File.GetPathAsString(pa_Buf);
      break;

    case GEMDDACH_FI_DATE:
      m_File.GetDateAsString(pa_Buf);
      break;

    case GEMDDACH_FI_TIME:
      m_File.GetTimeAsString(pa_Buf);
      break;

    case GEMDDACH_FI_SIZE:
      m_File.GetSizeAsString(pa_Buf);
      break;

    default:
      pa_Buf.Clear();
      return (LB_FALSE);
    }

  pa_Buf.TrimRight();

  return (LB_TRUE);
  }


/*
 *
**/
LB_Bool_T               GemDDACH_T::GetFHInfo(LB_Word_T    pa_Index,
                                              LB_String_T& pa_Buf)
  {
  switch(pa_Index)
    {
    case GEMDDACH_FH_PRIORCD:
      pa_Buf.SetBytes(m_AchInfo.FH.PriorCD,      sizeof(m_AchInfo.FH.PriorCD));
      break;

    case GEMDDACH_FH_DESTID:
      pa_Buf.SetBytes(&m_AchInfo.FH.DestID[1],   sizeof(m_AchInfo.FH.DestID)-1);
      break;

    case GEMDDACH_FH_ORIGID:
      pa_Buf.SetBytes(&m_AchInfo.FH.OrigID[1],   sizeof(m_AchInfo.FH.OrigID)-1);
      break;

    case GEMDDACH_FH_TRANSDATE:
      pa_Buf.SetBytes(m_AchInfo.FH.TransDate,    sizeof(m_AchInfo.FH.TransDate));
      break;

    case GEMDDACH_FH_TRANSTIME:
      pa_Buf.SetBytes(m_AchInfo.FH.TransTime,    sizeof(m_AchInfo.FH.TransTime));
      break;

    case GEMDDACH_FH_FILEIDMOD:
      pa_Buf.SetBytes(&m_AchInfo.FH.FileIDMod,   sizeof(m_AchInfo.FH.FileIDMod));
      break;

    case GEMDDACH_FH_RECSIZE:
      pa_Buf.SetBytes(m_AchInfo.FH.RecSize,      sizeof(m_AchInfo.FH.RecSize));
      break;

    case GEMDDACH_FH_BLOCKFACT:
      pa_Buf.SetBytes(m_AchInfo.FH.BlockFact,    sizeof(m_AchInfo.FH.BlockFact));
      break;

    case GEMDDACH_FH_FORMATCD:
      pa_Buf.SetBytes(&m_AchInfo.FH.FormatCD,    sizeof(m_AchInfo.FH.FormatCD));
      break;

    case GEMDDACH_FH_DESTNAME:
      pa_Buf.SetBytes(m_AchInfo.FH.DestName,     sizeof(m_AchInfo.FH.DestName));
      break;

    case GEMDDACH_FH_ORIGNAME:
      pa_Buf.SetBytes(m_AchInfo.FH.OrigName,     sizeof(m_AchInfo.FH.OrigName));
      break;

    case GEMDDACH_FH_REFCD:
      pa_Buf.SetBytes(m_AchInfo.FH.RefCD,        sizeof(m_AchInfo.FH.RefCD));
      break;

    default:
      pa_Buf.Clear();
      return (LB_FALSE);
    }

  return (LB_TRUE);
  }


/*
 *
**/
LB_Bool_T               GemDDACH_T::GetFTInfo(LB_Word_T    pa_Index,
                                              LB_String_T& pa_Buf)
  {
  switch(pa_Index)
    {
    case GEMDDACH_FT_BATCHCNT:
      pa_Buf.SetBytes(m_AchInfo.FT.BatchCnt,  sizeof(m_AchInfo.FT.BatchCnt));
      break;

    case GEMDDACH_FT_BLOCKCNT:
      pa_Buf.SetBytes(m_AchInfo.FT.BlockCnt,  sizeof(m_AchInfo.FT.BlockCnt));
      break;

    case GEMDDACH_FT_NUMDET:
      pa_Buf.SetBytes(m_AchInfo.FT.NumDet,    sizeof(m_AchInfo.FT.NumDet));
      break;

    case GEMDDACH_FT_HASH:
      pa_Buf.SetBytes(m_AchInfo.FT.HashValue, sizeof(m_AchInfo.FT.HashValue));
      break;

    case GEMDDACH_FT_TOTDB:
      pa_Buf.SetBytes(m_AchInfo.FT.TotDB,     sizeof(m_AchInfo.FT.TotDB));
      break;

    case GEMDDACH_FT_TOTCR:
      pa_Buf.SetBytes(m_AchInfo.FT.TotCR,     sizeof(m_AchInfo.FT.TotCR));
      break;

    default:
      pa_Buf.Clear();
      return (LB_FALSE);
    }

  return (LB_TRUE);
  }


/*---------------------------------------------------------------
  ---                   SET Methods                           ---
  ---------------------------------------------------------------*/


/*
 *
**/
LB_Bool_T               GemDDACH_T::SetBHInfo(LB_Word_T    pa_Index,
                                              LB_String_T& pa_Buf)
  {
  LB_String_T   l_Value;

  l_Value = pa_Buf;

  switch(pa_Index)
    {
    case GEMDDACH_BH_ORIGNAME:
      l_Value.PadRight(sizeof(m_AchInfo.BH.OrigName),' ');
      l_Value.GetBytes(m_AchInfo.BH.OrigName,   sizeof(m_AchInfo.BH.OrigName));
      break;

    case GEMDDACH_BH_ORIGID:
      l_Value.GetBytes(&m_AchInfo.BH.OrigID[1], sizeof(m_AchInfo.BH.OrigID)-1);
      m_AchInfo.BH.OrigID[0] = '1';        //--* IRS Employer Identification Number
      break;

    case GEMDDACH_BH_EFFDATE:
      l_Value.GetBytes(m_AchInfo.BH.EffDate,  sizeof(m_AchInfo.BH.EffDate));
      break;

    case GEMDDACH_BH_DESTID:
      l_Value.GetBytes(m_AchInfo.BH.DestID,   sizeof(m_AchInfo.BH.DestID));
      break;

    case GEMDDACH_BH_BATCH:
      l_Value.PadLeft(sizeof(m_AchInfo.BH.Batch),'0');
      l_Value.GetBytes(m_AchInfo.BH.Batch,    sizeof(m_AchInfo.BH.Batch));
      break;

    default:
      return (LB_FALSE);
    }

  return (LB_TRUE);
  }



/*
 *
**/
LB_Bool_T               GemDDACH_T::SetBTInfo(LB_Word_T    pa_Index,
                                              LB_String_T& pa_Buf)
  {
  LB_String_T   l_Value;

  l_Value = pa_Buf;

  switch(pa_Index)
    {
    case GEMDDACH_BT_NUMDET:
      l_Value.PadLeft(sizeof(m_AchInfo.BT.NumDet),'0');
      l_Value.GetBytes(m_AchInfo.BT.NumDet,    sizeof(m_AchInfo.BT.NumDet));
      break;

    case GEMDDACH_BT_HASH:
      l_Value.PadLeft(sizeof(m_AchInfo.BT.HashValue),'0');
      l_Value.GetBytes(m_AchInfo.BT.HashValue, sizeof(m_AchInfo.BT.HashValue));
      break;

    case GEMDDACH_BT_TOTDB:
      l_Value.PadLeft(sizeof(m_AchInfo.BT.TotDB),'0');
      l_Value.GetBytes(m_AchInfo.BT.TotDB,     sizeof(m_AchInfo.BT.TotDB));
      break;

    case GEMDDACH_BT_TOTCR:
      l_Value.PadLeft(sizeof(m_AchInfo.BT.TotCR),'0');
      l_Value.GetBytes(m_AchInfo.BT.TotCR,     sizeof(m_AchInfo.BT.TotCR));
      break;

    case GEMDDACH_BT_ORIGID:
      l_Value.GetBytes(&m_AchInfo.BT.OrigID[1], sizeof(m_AchInfo.BT.OrigID)-1);
      m_AchInfo.BT.OrigID[0] = '1';
      break;

    case GEMDDACH_BT_DESTID:
      l_Value = l_Value.SubstrLeft(8);
      l_Value.GetBytes(m_AchInfo.BT.DestID,    sizeof(m_AchInfo.BT.DestID));
      break;

    case GEMDDACH_BT_BATCH:
      l_Value.PadLeft(sizeof(m_AchInfo.BT.Batch),'0');
      l_Value.GetBytes(m_AchInfo.BT.Batch,     sizeof(m_AchInfo.BT.Batch));
      break;

    default:
      return (LB_FALSE);
    }

  return (LB_TRUE);
  }


/*
 *
**/
LB_Bool_T               GemDDACH_T::SetDTInfo(LB_Word_T    pa_Index,
                                              LB_String_T& pa_Buf)
  {
  LB_String_T   l_Value;

  l_Value = pa_Buf;

  switch(pa_Index)
    {
    case GEMDDACH_DT_TRANSCD:
      l_Value.PadLeft(sizeof(m_AchInfo.DT.TransCD),'0');
      l_Value.GetBytes(m_AchInfo.DT.TransCD,      sizeof(m_AchInfo.DT.TransCD));
      break;

    case GEMDDACH_DT_BANKID:
      l_Value.GetBytes(m_AchInfo.DT.BankID,       sizeof(m_AchInfo.DT.BankID));
      break;

    case GEMDDACH_DT_BANKACCTNUM:
      l_Value.PadRight(sizeof(m_AchInfo.DT.BankAcctNum),' ');
      l_Value.GetBytes(m_AchInfo.DT.BankAcctNum,  sizeof(m_AchInfo.DT.BankAcctNum));
      break;

    case GEMDDACH_DT_NET:
      l_Value.PadLeft(sizeof(m_AchInfo.DT.Net),'0');
      l_Value.GetBytes(m_AchInfo.DT.Net,          sizeof(m_AchInfo.DT.Net));
      break;

    case GEMDDACH_DT_EMPNUM:
      l_Value.PadLeft(sizeof(m_AchInfo.DT.EmpNum),'0');
      l_Value.GetBytes(m_AchInfo.DT.EmpNum,       sizeof(m_AchInfo.DT.EmpNum));
      break;

    case GEMDDACH_DT_EMPNAME:
      l_Value.PadRight(sizeof(m_AchInfo.DT.EmpName),' ');
      l_Value.GetBytes(m_AchInfo.DT.EmpName,      sizeof(m_AchInfo.DT.EmpName));
      break;

    case GEMDDACH_DT_TRACE:
      l_Value.PadLeft(sizeof(m_AchInfo.DT.Trace),'0');
      l_Value.GetBytes(m_AchInfo.DT.Trace,        sizeof(m_AchInfo.DT.Trace));
      break;

    default:
      return (LB_FALSE);
    }

  return (LB_TRUE);
  }


/*
 *
**/
LB_Bool_T               GemDDACH_T::SetFHInfo(LB_Word_T    pa_Index,
                                              LB_String_T& pa_Buf)
  {
  LB_String_T l_Value;

  l_Value = pa_Buf;

  switch(pa_Index)
    {
    case GEMDDACH_FH_DESTID:
      l_Value.PadRight(sizeof(m_AchInfo.FH.DestID),' ');
      l_Value.GetBytes(&m_AchInfo.FH.DestID[1],   sizeof(m_AchInfo.FH.DestID)-1);
      m_AchInfo.FH.DestID[0] = ' ';
      break;

    case GEMDDACH_FH_ORIGID:
      l_Value.PadRight(sizeof(m_AchInfo.FH.OrigID),' ');
      l_Value.GetBytes(&m_AchInfo.FH.OrigID[1],   sizeof(m_AchInfo.FH.OrigID)-1);
      m_AchInfo.FH.OrigID[0] = '1';
      break;

    case GEMDDACH_FH_DESTNAME:
      l_Value.PadRight(sizeof(m_AchInfo.FH.DestName),' ');
      l_Value.GetBytes(m_AchInfo.FH.DestName,     sizeof(m_AchInfo.FH.DestName));
      break;

    case GEMDDACH_FH_ORIGNAME:
      l_Value.PadRight(sizeof(m_AchInfo.FH.OrigName),' ');
      l_Value.GetBytes(m_AchInfo.FH.OrigName,     sizeof(m_AchInfo.FH.OrigName));
      break;

    case GEMDDACH_FH_REFCD:
      l_Value.PadRight(sizeof(m_AchInfo.FH.RefCD),' ');
      l_Value.GetBytes(m_AchInfo.FH.RefCD,        sizeof(m_AchInfo.FH.RefCD));
      break;

    default:
      return (LB_FALSE);
    }

  return (LB_TRUE);
  }


/*
 *
**/
LB_Bool_T               GemDDACH_T::SetFTInfo(LB_Word_T    pa_Index,
                                              LB_String_T& pa_Buf)
  {
  LB_String_T   l_Value;

  l_Value = pa_Buf;

  switch(pa_Index)
    {
    case GEMDDACH_FT_BATCHCNT:
      l_Value.PadLeft(sizeof(m_AchInfo.FT.BatchCnt),'0');
      l_Value.GetBytes(m_AchInfo.FT.BatchCnt,  sizeof(m_AchInfo.FT.BatchCnt));
      break;

    case GEMDDACH_FT_BLOCKCNT:
      l_Value.PadLeft(sizeof(m_AchInfo.FT.BlockCnt),'0');
      l_Value.GetBytes(m_AchInfo.FT.BlockCnt,  sizeof(m_AchInfo.FT.BlockCnt));
      break;

    case GEMDDACH_FT_NUMDET:
      l_Value.PadLeft(sizeof(m_AchInfo.FT.NumDet),'0');
      l_Value.GetBytes(m_AchInfo.FT.NumDet,    sizeof(m_AchInfo.FT.NumDet));
      break;

    case GEMDDACH_FT_HASH:
      l_Value.PadLeft(sizeof(m_AchInfo.FT.HashValue),'0');
      l_Value.GetBytes(m_AchInfo.FT.HashValue, sizeof(m_AchInfo.FT.HashValue));
      break;

    case GEMDDACH_FT_TOTDB:
      l_Value.PadLeft(sizeof(m_AchInfo.FT.TotDB),'0');
      l_Value.GetBytes(m_AchInfo.FT.TotDB,     sizeof(m_AchInfo.FT.TotDB));
      break;

    case GEMDDACH_FT_TOTCR:
      l_Value.PadLeft(sizeof(m_AchInfo.FT.TotCR),'0');
      l_Value.GetBytes(m_AchInfo.FT.TotCR,     sizeof(m_AchInfo.FT.TotCR));
      break;

    default:
      return (LB_FALSE);
    }

  return (LB_TRUE);
  }


/*
 *
**/
LB_Bool_T               GemDDACH_T::Open(LB_Word_T pa_OrgID)
  {
  LB_String_T l_String;

  m_File.Close();

  if (pa_OrgID <= 0)
    return(LB_FALSE);

  m_OrgID = pa_OrgID;

  //--* Get Payroll Path
  if (m_GemCtl.Read(m_OrgID) == LB_FALSE)
    return (LB_FALSE);

  m_GemCtl.GetPayrollPath(l_String);
  l_String.Append((LB_CZString_T) ACH_FN);
  m_File.SetFileName(l_String);

  if (m_File.Exists() == LB_FALSE)
    return (LB_FALSE);

  if (m_File.Open(FIO_OM_RW_BUF) == LB_FALSE)
    return (LB_FALSE);

  m_File.SeekFromStart(0);

  return (LB_TRUE);
  }


/*
 *
**/
LB_Bool_T               GemDDACH_T::Create(LB_Word_T pa_OrgID)
  {
  LB_String_T   l_String;

  m_File.Close();

  if (pa_OrgID <= 0)
    return(LB_FALSE);

  m_OrgID = pa_OrgID;

  //--* Get Payroll Path
  if (m_GemCtl.Read(m_OrgID) == LB_FALSE)
    return (LB_FALSE);

  m_GemCtl.GetPayrollPath(l_String);
  l_String.Append((LB_CZString_T) ACH_FN);

  m_File.SetFileName(l_String);

  if (m_File.Create() == LB_FALSE)
    return (LB_FALSE);

  return (LB_TRUE);
  }


/*---------------------------------------------------------------
  ---                   READ Methods                          ---
  ---------------------------------------------------------------*/


/*
 *
**/
LB_Bool_T               GemDDACH_T::ReadFI()
  {
  if (m_File.IsOpen() == LB_FALSE)
    return (LB_FALSE);

  return (LB_TRUE);
  }


/*
 *
**/
LB_Bool_T               GemDDACH_T::ReadFH()
  {
  LB_String_T   l_String;

  if (m_File.IsOpen() == LB_FALSE)
    return (LB_FALSE);

  m_File.SeekFromStart(0);

  l_String.PadRight(sizeof(m_AchInfo.FH),0x20);

  while (m_File.Read(l_String) == sizeof(m_AchInfo))
    {
    l_String.GetBytes(&m_AchInfo.FH.RecType,sizeof(m_AchInfo.FH));

    if (m_AchInfo.FH.RecType == RT_FH)
      break;
    }

  if (m_AchInfo.FH.RecType != RT_FH)
    return (LB_FALSE);

  return (LB_TRUE);
  }


/*
 *
**/
LB_Bool_T               GemDDACH_T::ReadFT()
  {
  LB_String_T   l_String;

  if (m_File.IsOpen() == LB_FALSE)
    return (LB_FALSE);

  m_File.SeekFromStart(0);

  l_String.PadRight(sizeof(m_AchInfo.FT),0x20);

  while (m_File.Read(l_String) == sizeof(m_AchInfo.FT))
    {
    l_String.GetBytes(&m_AchInfo.FT.RecType,sizeof(m_AchInfo.FT));

    if (m_AchInfo.FT.RecType == RT_FT)
      break;
    }

  if (m_AchInfo.FT.RecType != RT_FT)
    return (LB_FALSE);

  return (LB_TRUE);
  }


/*
 *
**/
LB_Bool_T               GemDDACH_T::ReadBH()
  {
  LB_String_T   l_String;

  if (m_File.IsOpen() == LB_FALSE)
    return (LB_FALSE);

  m_File.SeekFromStart(0);

  l_String.PadRight(sizeof(m_AchInfo.BH),0x20);

  while (m_File.Read(l_String) == sizeof(m_AchInfo.BH))
    {
    l_String.GetBytes(&m_AchInfo.BH.RecType,sizeof(m_AchInfo.BH));

    if (m_AchInfo.BH.RecType == RT_BH)
      break;
    }

  if (m_AchInfo.BH.RecType != RT_BH)
    return (LB_FALSE);

  return (LB_TRUE);
  }


/*
 *
**/
LB_Bool_T               GemDDACH_T::ReadBT()
  {
  LB_String_T   l_String;

  if (m_File.IsOpen() == LB_FALSE)
    return (LB_FALSE);

  m_File.SeekFromStart(0);

  l_String.PadRight(sizeof(m_AchInfo.BT),0x20);

  while (m_File.Read(l_String) == sizeof(m_AchInfo.BT))
    {
    l_String.GetBytes(&m_AchInfo.FT.RecType,sizeof(m_AchInfo.BT));

    if (m_AchInfo.FT.RecType == RT_BT)
      break;
    }

  if (m_AchInfo.FT.RecType != RT_BT)
    return (LB_FALSE);

  return (LB_TRUE);
  }


/*
 *
 *--- Assumes record 0 contains no detail records, as per ACH format
 *
**/
LB_Bool_T               GemDDACH_T::ReadDT(LB_Word_T *pa_NextEntry,
                                           LB_Bool_T pa_MoveNext)
  {
  LB_String_T   l_String;

  if (m_File.IsOpen() == LB_FALSE)
    return (LB_FALSE);

  l_String.PadRight(sizeof(m_AchInfo.DT),0x20);

  //--* Find first record
  if (*pa_NextEntry == 0)
    {
    m_File.SeekFromStart(0);
    while (m_File.Read(l_String) == sizeof(m_AchInfo.DT))
      {
      l_String.GetBytes(&m_AchInfo.DT.RecType,sizeof(m_AchInfo.DT));
  
      if (m_AchInfo.DT.RecType == RT_DT)
        return (LB_TRUE);

      *pa_NextEntry = *pa_NextEntry + 1;
      }
    }
  else
    {
    //--* Find Previous/Next Record
    if (pa_MoveNext == LB_FALSE)
      *pa_NextEntry = *pa_NextEntry - 1;
    else
      *pa_NextEntry = *pa_NextEntry + 1;

    //--* Read Record
    l_String.Fill(' ');

    if ((*pa_NextEntry > 0) && (*pa_NextEntry * sizeof(m_AchInfo.DT) < m_File.Size()))
      {
      m_File.SeekFromStart((*pa_NextEntry) * (sizeof(m_AchInfo.DT)));
      if (m_File.Read(l_String) != sizeof(m_AchInfo.DT))
        return (LB_FALSE);
      }

    l_String.GetBytes(&m_AchInfo.DT.RecType,sizeof(m_AchInfo.DT));

    if (m_AchInfo.DT.RecType == RT_DT)
      return (LB_TRUE);
    else
      {
      if (pa_MoveNext == LB_FALSE)
        *pa_NextEntry = *pa_NextEntry + 1;
      else
        *pa_NextEntry = *pa_NextEntry - 1;
      return (LB_FALSE);
      }
    }
  return (LB_FALSE);
 }


/*---------------------------------------------------------------
  ---                   WRITE Methods                         ---
  ---------------------------------------------------------------*/


/*
 *
**/
LB_Bool_T               GemDDACH_T::WriteFH()
  {
  LB_String_T   l_String;
  LB_Date_T     l_SysDate;
  LB_Time_T     l_SysTime;

  if (m_File.IsOpen() == LB_FALSE)
    return (LB_FALSE);

  memcpy(&m_AchInfo.FH.RecType,        "1",   1);
  memcpy( m_AchInfo.FH.PriorCD,        "01",  2);

  l_SysDate.SetToToday(); l_SysDate.GetSortDateShort(l_String);
  l_String.GetBytes(m_AchInfo.FH.TransDate,sizeof(m_AchInfo.FH.TransDate));

  l_SysTime.SetToNow(); l_SysTime.GetSortTimeShort(l_String);
  l_String.GetBytes(m_AchInfo.FH.TransTime,sizeof(m_AchInfo.FH.TransTime));

  memcpy(&m_AchInfo.FH.FileIDMod,      "A",   1);
  memcpy( m_AchInfo.FH.RecSize,        "094", 3);
  memcpy( m_AchInfo.FH.BlockFact,      "10",  2);
  memcpy(&m_AchInfo.FH.FormatCD,       "1",   1);

  m_AchInfo.FH.Cr         = 0x0D;
  m_AchInfo.FH.DeleteFlag = 0x0A;

  l_String.SetBytes((LB_Byte_T *) &m_AchInfo.FH,
                    (LB_Word_T) sizeof(m_AchInfo.FH));

  m_ClearRecord();

  return (m_File.Write(l_String));
  }


/*
 *
**/
LB_Bool_T               GemDDACH_T::WriteBH()
  {
  LB_String_T   l_String;
  LB_Date_T     l_SysDate;

  if (m_File.IsOpen() == LB_FALSE)
    return (LB_FALSE);

  memcpy(&m_AchInfo.BH.RecType,         "5",            1);
  memcpy( m_AchInfo.BH.SvcCls,          "200",          3);
  memset( m_AchInfo.BH.OrigDesc,        ' ',           20);
  memcpy( m_AchInfo.BH.ClsCD,           "PPD",          3);
  memcpy( m_AchInfo.BH.DescEntry,       "PAYROLL   ",  10);

  l_SysDate.SetToToday(); l_SysDate.GetSortDateShort(l_String);
  l_String.GetBytes(m_AchInfo.BH.DescDate,sizeof(m_AchInfo.BH.DescDate));

  memcpy( m_AchInfo.BH.SettleDate,     "000",           3);
  memcpy(&m_AchInfo.BH.OrigStatusCode, "1",             1);

  m_AchInfo.BH.Cr         = 0x0D;
  m_AchInfo.BH.DeleteFlag = 0x0A;

  l_String.SetBytes((LB_Byte_T *) &m_AchInfo.BH,
                    (LB_Word_T) sizeof(m_AchInfo.BH));

  m_ClearRecord();

  return (m_File.Write(l_String));
  }


/*
 *
**/
LB_Bool_T               GemDDACH_T::WriteDT()
  {
  LB_String_T   l_String;
  LB_Date_T     l_SysDate;

  if (m_File.IsOpen() == LB_FALSE)
    return (LB_FALSE);

  memcpy(&m_AchInfo.DT.RecType,         "6",            1);
  memcpy( m_AchInfo.DT.DiscrInfo,       "  ",           2);
  memcpy(&m_AchInfo.DT.Addenda,         "0",            1);

  m_AchInfo.DT.Cr         = 0x0D;
  m_AchInfo.DT.DeleteFlag = 0x0A;

  l_String.SetBytes((LB_Byte_T *) &m_AchInfo.DT,
                    (LB_Word_T) sizeof(m_AchInfo.DT));

  m_ClearRecord();

  return (m_File.Write(l_String));
  }


/*
 *
**/
LB_Bool_T               GemDDACH_T::WriteBT()
  {
  LB_String_T   l_String;

  if (m_File.IsOpen() == LB_FALSE)
    return (LB_FALSE);

  memcpy(&m_AchInfo.BT.RecType,         "8",            1);
  memcpy( m_AchInfo.BT.SvcCls,          "200",          3);
  memset( m_AchInfo.BT.Filler,          ' ',           sizeof(m_AchInfo.BT.Filler));

  m_AchInfo.BT.Cr         = 0x0D;
  m_AchInfo.BT.DeleteFlag = 0x0A;

  l_String.SetBytes((LB_Byte_T *) &m_AchInfo.BT,
                    (LB_Word_T) sizeof(m_AchInfo.BT));

  m_ClearRecord();

  return (m_File.Write(l_String));
  }


/*
 *
**/
LB_Bool_T               GemDDACH_T::WriteFT()
  {
  LB_String_T   l_String;
  LB_Date_T     l_SysDate;

  if (m_File.IsOpen() == LB_FALSE)
    return (LB_FALSE);

  memcpy(&m_AchInfo.FT.RecType,         "9",            1);
  memset( m_AchInfo.FT.Filler,          ' ',           sizeof(m_AchInfo.FT.Filler));

  m_AchInfo.FT.Cr         = 0x0D;
  m_AchInfo.FT.DeleteFlag = 0x0A;

  l_String.SetBytes((LB_Byte_T *) &m_AchInfo.FT,
                    (LB_Word_T) sizeof(m_AchInfo.FT));

  m_ClearRecord();

  return (m_File.Write(l_String));
  }

/*
 *
**/
LB_Bool_T               GemDDACH_T::WriteFiller()
  {
  LB_String_T   l_String;
  LB_Date_T     l_SysDate;

  if (m_File.IsOpen() == LB_FALSE)
    return (LB_FALSE);

  memset(&m_AchInfo.FT.RecType, '9', sizeof(m_AchInfo.FT));
  m_AchInfo.FT.Cr         = 0x0D;
  m_AchInfo.FT.DeleteFlag = 0x0A;

  l_String.SetBytes((LB_Byte_T *) &m_AchInfo.FT,
                    (LB_Word_T) sizeof(m_AchInfo.FT));

  m_ClearRecord();

  return (m_File.Write(l_String));
  }

