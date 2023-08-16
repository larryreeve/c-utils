/*  mftable.cpp
 *
 *  MicroFocus Table Input/Output routines
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
 * Notes:
 *      1) No write capabilites are implemented
 *      2) Only forward movement is allowed
 *      3) Fixed record sizes only
 *
 *
 * Revision History:
 *
 *      Date            By      Notes
 *      --------        ---     ------------------------------------------
 *      01/10/96        LR      -Initial Release
 *      02/16/96        LR      -Updated for flags in column class
 *      03/04/96        LR      -StringList speeds searching
 *      03/21/96        LR      -Changed StringList to HashList
 *
**/


//---------------------------------------------------------------
//---                INCLUDE FILES                            ---
//---------------------------------------------------------------
#include <assert.h>
#include <mem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lbport.h>
#include <lbdebug.hpp>
#include <lbdate.hpp>
#include <mfcolumn.hpp>
#include <lbfio.hpp>
#include <mftable.hpp>

LB_ASSERTFILE(__FILE__)


//---------------------------------------------------------------
//---                   Private Methods                       ---
//---------------------------------------------------------------
LB_Word_T               MF_Table_T::m_SwapWord(LB_Word_T pa_WordValue)
{
  LB_Byte_T TempByte;
  union
  {
    LB_Byte_T Bytes[2];
    LB_Word_T WordValue;
  } WordData;

  WordData.WordValue = pa_WordValue;

  TempByte = WordData.Bytes[1];
  WordData.Bytes[1] = WordData.Bytes[0];
  WordData.Bytes[0] = TempByte;

  return WordData.WordValue;
}


//--- Goto start of record as indicated by m_FilePos
void                    MF_Table_T::m_GotoCurrentRow()
{
  LB_Word_T     RecHdr;
  LB_Word_T     RecSize;
  LB_Word_T     RecType;
  LB_String_T   InpBuffer;

GCR_ReadNext:
  //--* Goto start of record, adjust for 4-byte record boundary
  m_FilePos = m_FilePos + (m_FilePos % 4);

  m_DataFile.SeekFromStart(m_FilePos);

  RecHdr = 0x00;

  //--* Read next record header, returning if it could not be read
  InpBuffer.PadRight(sizeof(RecHdr), 0x20);
  if (m_DataFile.Read(InpBuffer) != sizeof(RecHdr))
    return;

  InpBuffer.GetBytes((LB_Byte_T *) &RecHdr, sizeof(RecHdr));

  //--- Parse Record Header
  RecHdr   = m_SwapWord(RecHdr);
  RecSize  = (RecHdr & 0x0FFF);
  RecType  = (RecHdr & 0xF000) >> 12;

  m_FilePos = m_FilePos + RecSize + sizeof(RecHdr);

  //--- Check for malformed record
  if (RecSize != m_RowSize)
    goto GCR_ReadNext;

  //--- Check if data record, otherwise skip
  if (RecType != 4)
    goto GCR_ReadNext;
}


//--- Reads all the columns in a row
LB_Bool_T               MF_Table_T::m_ReadColumns()
{
  LB_Word_T      Idx;
  LB_Word_T      ColumnOffset;
  LB_CobolComp_T CobCompValue;
  LB_FloatPt_T   FloatValue;
  LB_String_T    ColumnValue;

  m_OS.Yield();

  m_GotoCurrentRow();

  //--- Read row
  m_IOString.PadRight(m_RowSize, 0x20);
  if (m_DataFile.Read(m_IOString) == LB_FALSE)
    return(LB_FALSE);

  //--* Parse each column from the read buffer (row)
  ColumnOffset = 0;
  for (Idx=0; Idx < m_NumColumns; Idx++)
    {
    //--- Extract column value from row
    ColumnValue.SetBytes(&m_IOString[ColumnOffset], m_Columns[Idx].GetSize());

    //--- Store the column value
    m_Columns[Idx].SetValue(ColumnValue);

    ColumnOffset = ColumnOffset + m_Columns[Idx].GetSize();
    }

  return (LB_TRUE);
}


LB_Bool_T               MF_Table_T::m_WriteColumns()
{
  return (LB_FALSE);
}


//---------------------------------------------------------------
//---                   Public  Methods                       ---
//---------------------------------------------------------------
MF_Table_T::MF_Table_T()
{
  m_FilePos    = 0;
  m_RowDeleted = LB_FALSE;
  m_NumColumns = 0;
}


MF_Table_T::~MF_Table_T()
{
  Clear();
}


void                    MF_Table_T::Clear()
{
  int   Idx;

  //--- Clear Field Information
  for (Idx=0; Idx < MFTBL_MAX_COLUMNS; Idx++)
    m_Columns[Idx].Clear();

  m_FilePos    = 0;
  m_RowDeleted = LB_FALSE;
  m_NumColumns = 0;

  m_TableName.Clear();

  m_SearchList.Clear();

  m_DataFile.Close();
}


void                    MF_Table_T::Close()
{
  m_FilePos    = 0;
  m_RowDeleted = LB_FALSE;

  m_DataFile.Close();
}


//--- Declare a column within a table. If valid column, return a
//--- column handle. Column handles range from 1..MFTBL_MAX_COLUMNS
//--- A column handle of MFTBL_MAX_COLUMNS+1 indicates a failure.
LB_Word_T               MF_Table_T::ColumnDefine(LB_CZString_T pa_Name,
                                                 LB_Word_T     pa_Size,
                                                 LB_Word_T     pa_Flags)
{
  char          NumBuf[16];
  LB_String_T   ColName;
  LB_String_T   SearchStr;

  LB_ASSERT((m_NumColumns+1) < MFTBL_MAX_COLUMNS);

  m_Columns[m_NumColumns].Clear();

  ColName = pa_Name;

  if (m_Columns[m_NumColumns].Define(ColName,pa_Size,pa_Flags) == LB_FALSE)
    return (MFTBL_MAX_COLUMNS);

  //--- Add to string list for searching
  //------ Save column id
  sprintf(NumBuf,"%06d",m_NumColumns);
  SearchStr = NumBuf;
  //------ Save column name
  ColName.UpperCase();
  SearchStr = SearchStr + ColName;
  //------ Add to sorted list
  m_SearchList.Add(SearchStr,6,ColName.Size());

  m_NumColumns++;

  return (m_NumColumns - 1);
}


LB_Bool_T               MF_Table_T::GetColumnValue(LB_Word_T    pa_ColID,
                                                   LB_String_T& pa_Value)
{
  if (pa_ColID > m_NumColumns)
    return (LB_FALSE);

  return (m_Columns[pa_ColID].GetValue(pa_Value));
}


LB_Bool_T               MF_Table_T::GetColumnValue(LB_String_T& pa_ColName,
                                                   LB_String_T& pa_Value)
{
  LB_Word_T     Idx;
  LB_String_T   Key;

  pa_ColName.UpperCase();

  if (m_SearchList.Retrieve(pa_ColName,6,0,Key) == LB_TRUE)
  {
    Key = Key.SubstrMid(0,6);
    Idx = Key.AsWord();
    if (Idx < m_NumColumns)
    {
      m_Columns[Idx].GetValue(pa_Value);
      return(LB_TRUE);
    }
  }

  pa_Value.Clear();

  return (LB_FALSE);
}


LB_Bool_T               MF_Table_T::SetColumnValue(LB_Word_T pa_ColID, LB_String_T& pa_String)
{
  if (pa_ColID > m_NumColumns)
    return (LB_FALSE);

  m_Columns[pa_ColID].SetValue(pa_String);

  return (LB_TRUE);
}


LB_Bool_T               MF_Table_T::SetColumnValue(LB_String_T& pa_ColName, LB_String_T& pa_String)
{
  LB_Word_T    Idx;
  LB_String_T  ColumnName;

  pa_ColName.UpperCase();

  for (Idx=0; Idx < m_NumColumns; Idx++)
  {
    m_Columns[Idx].GetName(ColumnName);
    if (ColumnName == pa_ColName)
    {
      m_Columns[Idx].SetValue(pa_String);
      return(LB_TRUE);
    }
  }
  return (LB_FALSE);
}


//--- Only read capabilities are currently implemented
LB_Bool_T               MF_Table_T::Create()
{
  return (LB_FALSE);
}


LB_Bool_T               MF_Table_T::IsTableAtBottom()
{
  if (m_FilePos >= m_DataFile.Size())
    return (LB_TRUE);

  return (LB_FALSE);
}


LB_Bool_T               MF_Table_T::IsTableAtTop()
{
  if (m_FilePos == 128)
    return (LB_TRUE);

  return (LB_FALSE);
}


LB_Bool_T               MF_Table_T::IsRowDeleted()
{
  return m_RowDeleted;
}


LB_Bool_T               MF_Table_T::Open()
{
  LB_Word_T     ReadCnt;
  LB_String_T   FileHdrBuffer;

  //--- Check for no Columns
  if (m_NumColumns == 0)
    return (LB_FALSE);

  m_DataFile.Close();

  //--- Check for no filename
  if (m_TableName.IsEmpty() == LB_TRUE)
    return(LB_FALSE);

  m_DataFile.SetFileName(m_TableName);

  //--- Check if file exists
  if (m_DataFile.Exists() == LB_FALSE)
    return (LB_FALSE);

  //--- Low-level open file
  if (m_DataFile.Open(FIO_OM_RW_BUF) == LB_FALSE)
    return (LB_FALSE);

  //--- Read Header
  m_DataFile.SeekFromStart(0L);
	FileHdrBuffer.PadRight(sizeof(m_DataFileHdr),0x20);
  ReadCnt = m_DataFile.Read(FileHdrBuffer);
  FileHdrBuffer.GetBytes((LB_Byte_T *) &m_DataFileHdr,sizeof(m_DataFileHdr));
  if ((ReadCnt != sizeof(m_DataFileHdr)) || (m_DataFileHdr.HdrLen != 0x00007E30))
    return (LB_FALSE);

  //--- Set row size
  assert(m_DataFileHdr.MaxRecSize == m_DataFileHdr.MinRecSize);
	m_RowSize = m_SwapWord(m_DataFileHdr.MaxRecSize);
  assert(m_RowSize <= MFTBL_MAX_ROWSIZE);

  //--- Set start of database navigation
  m_FilePos = 128L;

  return (LB_TRUE);
}


LB_Bool_T               MF_Table_T::RowAdd()
{
  return (LB_FALSE);
}


LB_Bool_T               MF_Table_T::RowDelete()
{
  return (LB_FALSE);
}


LB_Bool_T               MF_Table_T::RowModify()
{
  return (LB_FALSE);
}


LB_Bool_T               MF_Table_T::RowMoveNext()
{
  if (m_DataFile.IsOpen() == LB_FALSE)
    return (LB_FALSE);

  return (m_ReadColumns());
}


LB_Bool_T               MF_Table_T::RowMovePrevious()
{
  return (LB_FALSE);
}


LB_Bool_T               MF_Table_T::RowMoveBottom()
{
  return (LB_FALSE);
}


LB_Bool_T               MF_Table_T::RowMoveTop()
{
  m_FilePos = 128L;

  return (LB_TRUE);
}


void                    MF_Table_T::SetTableName(LB_String_T& pa_TableName)
{
  m_TableName = pa_TableName;
}


LB_Bool_T               MF_Table_T::TableLock()
{
  return (m_DataFile.Lock());
}


void                    MF_Table_T::TableUnlock()
{
  m_DataFile.Unlock();
}
