/*  lbini.cpp
 *
 *  Ini File Processing routines
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
 *      03/26/96        LR      -Initial Release
 *      05/17/97        LR      -Change file delete method to file remove 
 *
 * Notes:
 *
 *
**/


//---------------------------------------------------------------
//---                INCLUDE FILES                            ---
//---------------------------------------------------------------
#include <lbport.h>
#include <lbini.hpp>


//---------------------------------------------------------------
//---                   Private Methods                       ---
//---------------------------------------------------------------
LB_Bool_T               LB_IniFile_T::m_NextLine(LB_String_T& pa_NextLine)
  {
  LB_Word_T CurSize;

  if (m_CurPos >= m_FileBuf.Size())
    return (LB_FALSE);

  pa_NextLine.PadRight(256,' '); pa_NextLine.Fill(' ');
  CurSize = 0;

  while (m_CurPos < m_FileBuf.Size())
    {
    if (m_FileBuf[m_CurPos] == '\n')
      {
      m_CurPos = m_CurPos + 1;
      break;
      }

    pa_NextLine[CurSize] = m_FileBuf[m_CurPos];

    m_CurPos = m_CurPos + 1;
    CurSize  = CurSize + 1;
    if (CurSize > pa_NextLine.Size())
      break;
    }

  pa_NextLine.TrimRight();
  pa_NextLine.UpperCase();

  return (LB_TRUE);
  }



//---------------------------------------------------------------
//---                   Public  Methods                       ---
//---------------------------------------------------------------
LB_IniFile_T::LB_IniFile_T()
  {
  m_FileBuf.Clear();
  m_FileName.Clear();
  m_File.Close();
  m_CurPos = 0;
  }


LB_IniFile_T::~LB_IniFile_T()
  {
  m_FileBuf.Clear();
  m_FileName.Clear();
  m_File.Close();
  m_CurPos = 0;
  }


LB_Bool_T               LB_IniFile_T::Create()
  {
  return (m_File.Create());
  }


/*
 *
**/
LB_Bool_T               LB_IniFile_T::CreateSection(LB_String_T& pa_Section)
  {
  if (m_File.IsOpen() == LB_FALSE)
    return (LB_FALSE);

  if (pa_Section.IsEmpty() == LB_TRUE)
    return (LB_FALSE);

  return (LB_FALSE);
  }


LB_Bool_T               LB_IniFile_T::DeleteSection(LB_String_T& pa_Section)
  {
  if (m_File.IsOpen() == LB_FALSE)
    return (LB_FALSE);

  if (pa_Section.IsEmpty() == LB_TRUE)
    return (LB_FALSE);

  return (LB_FALSE);
  }


void                    LB_IniFile_T::Decode()
  {
  m_FileBuf = m_CryptEngine.Decrypt(m_FileBuf);
  }


void                    LB_IniFile_T::Encode()
  {
  m_FileBuf = m_CryptEngine.Encrypt(m_FileBuf);
  }

LB_Bool_T               LB_IniFile_T::Exists()
  {
  return (m_File.Exists());
  }


LB_Bool_T               LB_IniFile_T::Load()
  {
  if (m_File.Exists() == LB_FALSE)
    return (LB_FALSE);

  if (m_File.Open(FIO_OM_RO_BUF) == LB_FALSE)
    return (LB_FALSE);

  m_FileBuf.PadRight(m_File.Size(),' ');
  if (m_File.Read(m_FileBuf) != m_FileBuf.Size())
    {
    m_File.Close();
    return (LB_FALSE);
    }

  m_File.Close();
  m_CurPos = 0;

  return (LB_TRUE);
  }


LB_Bool_T               LB_IniFile_T::ReadValue(LB_String_T& pa_Section,
                                                LB_String_T& pa_Ident,
                                                LB_String_T& pa_Value)
  {
  LB_String_T   ReadLine;
  LB_String_T   ReadIdent;
  LB_String_T   SearchSection;
  LB_String_T   SearchIdent;

  if (m_FileBuf.Size() <= 0)
    return (LB_FALSE);

  //--- Reset ptr to start of buffer
  m_CurPos = 0;

  SearchSection = "[";
  SearchSection = SearchSection + pa_Section;
  SearchSection = SearchSection + "]";
  SearchSection.UpperCase();

  SearchIdent = pa_Ident;
  SearchIdent = SearchIdent + "=";
  SearchIdent.UpperCase();

  //--- Find Section
  while (m_NextLine(ReadLine) == LB_TRUE)
    {
    if (ReadLine == SearchSection)
      break;
    }

  //--- Find Ident Value
  if (ReadLine == SearchSection)
    {
    while (m_NextLine(ReadLine) == LB_TRUE)
      {
      ReadIdent = ReadLine.SubstrLeft(SearchIdent.Size());

      if (ReadIdent == SearchIdent)
        break;
      }
    }

  //--- Extract Ident
  if (ReadIdent == SearchIdent)
    {
    //---Ident found
    pa_Value = ReadLine.SubstrMid(ReadIdent.Size(), ReadLine.Size() - ReadIdent.Size());
    pa_Value.UpperCase();

    return (LB_TRUE);
    }

  return (LB_FALSE);
  }


LB_Bool_T               LB_IniFile_T::Remove()
  {
  return (m_File.Remove());
  }


#pragma argsused
LB_Bool_T               LB_IniFile_T::WriteValue(LB_String_T& pa_Section,
                                                 LB_String_T& pa_Ident,
                                                 LB_String_T& pa_Value)
  {
  if (m_File.IsOpen() == LB_FALSE)
    return (LB_FALSE);

  return (LB_FALSE);
  }


void                    LB_IniFile_T::SetFileName(LB_String_T& pa_FileName)
  {
  m_File.SetFileName(pa_FileName);
  }