/*  lbstring.cpp
 *
 *  String Processing routines
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
 *      11/20/95        LR      -Initial Release
 *      01/16/96        LR      -Added copy constructor
 *                              -Added max size restriction for fixed-length
 *                              strings
 *      02/05/96        LR      -Added appendtoasciiz function
 *                              -Added append methods
 *      02/21/96        LR      -Added justify methods
 *                              -Removed max size restriction
 *      03/18/96        LR      -Added HashKey routine
 *                              -Changed allocation routine in m_resize to
 *                              reduce the number of calls to heap mgmt.
 *      06/11/96        LR      -Added IsAllSpaces method
 *      06/21/96        LR      -Added Search method
 *      04/23/97        LR      -Updates for VC++ non-standard extension messages
 *                              -Made operator + const
 *                              -Fixed memory overwrite in SubstrRight
 *
 *
 * Notes:
 *      All strings: 1) are limited to LB_Word_T in size;
 *                   2) are NOT in ASCIIZ format (no trailing '\0');
 *                   3) are in byte not char format
 *
 *      Based on the article "A Portable C++ String Class", William Hill,
 *      Dr. Dobbs Sourcebook July/August 1995, pp. 19-25.
 *
 *
**/


///*---------------------------------------------------------------
//  ---                INCLUDE FILES                            ---
//  ---------------------------------------------------------------
#ifdef __TURBOC__
  #include <alloc.h>
  #include <mem.h>
#endif

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <lbport.h>
#include <lbdebug.hpp>
#include <lbstring.hpp>


LB_ASSERTFILE(__FILE__)


//---------------------------------------------------------------
//---                   Private Methods                       ---
//---------------------------------------------------------------
LB_Bool_T               LB_String_T::m_Resize(LB_Word_T pa_NewSize)
  {
  LB_Byte_T     *l_TempData;
  LB_Word_T     l_TempSize;
  LB_Word_T     l_Cnt;

  if (pa_NewSize == 0)
    {
    if (m_Data != NULL)
      {
      LB_DEL_INITIALIZE(m_Data,m_BlkSize);
      delete[] m_Data;
      }
    m_Data    = NULL;
    m_CurSize = 0;
    m_BlkSize = 0;
    return (LB_TRUE);
    }

  //--- If NewSize is less than the memory block size already allocated,
  //--- adjust cursize with freeing and allocating additional memory.
  if (pa_NewSize <= m_BlkSize)
    {
    m_CurSize = pa_NewSize;
    return (LB_TRUE);
    }

  //--- NewSize > BlkSize

  //--- Save existing data
  l_TempData = NULL;
  l_TempSize = 0;

  if (m_CurSize > 0)
    {
    l_TempSize = m_CurSize;

    //--- Allocate temp buffer
    l_TempData = new LB_Byte_T[l_TempSize];
    LB_ASSERT(l_TempData != NULL);
    if (l_TempData == NULL)
      return (LB_FALSE);
    LB_NEW_INITIALIZE(l_TempData,l_TempSize);
    memcpy(l_TempData,m_Data,l_TempSize);
    }

  //--- Delete current buffer
  if (m_Data != NULL)
    {
    LB_DEL_INITIALIZE(m_Data,m_BlkSize);
    delete[] m_Data;
    }

  m_CurSize = 0;
  m_BlkSize = 0;
  m_Data    = NULL;

  //--- Allocate new buffer
  if (pa_NewSize > 0)
    {
    m_CurSize = pa_NewSize;
    m_BlkSize = pa_NewSize;
    m_Data    = new LB_Byte_T[pa_NewSize];
    LB_ASSERT(m_Data != NULL);
    LB_NEW_INITIALIZE(m_Data,pa_NewSize);
    if (m_Data == NULL)
      return (LB_FALSE);
    }

  //--- Restore old data
  if (l_TempData != NULL)
    {
    l_Cnt = l_TempSize;
    if (l_Cnt > m_CurSize)
      l_Cnt = m_CurSize;

    memcpy(m_Data,l_TempData,l_Cnt);

    delete[] l_TempData;
    l_TempData = NULL;
    }

  LB_ASSERT(l_TempData == NULL);

  return (LB_TRUE);
  }


//---------------------------------------------------------------
//---                   Constructors & Destructor             ---
//---------------------------------------------------------------
LB_String_T::LB_String_T()
  {
  m_Data        = NULL;
  m_CurSize     = 0;
  m_BlkSize     = 0;

  m_ClassActive = m_Resize(0);
  }


//--- Copy constructor
LB_String_T::LB_String_T(const LB_String_T& pa_Copy)
  {
  m_ClassActive = LB_FALSE;
  m_Data        = NULL;
  m_CurSize     = 0;
  m_BlkSize     = 0;

  if (pa_Copy.BlockSize() > 0)
    {
    if (m_Resize(pa_Copy.BlockSize()) == LB_TRUE)
      {
      //pa_Copy.GetBytes(m_Data, m_CurSize);
      memcpy(m_Data, pa_Copy.m_Data, m_CurSize);
      m_ClassActive = LB_TRUE;
      }
    }
  }


LB_String_T::LB_String_T(LB_Word_T pa_InitialSize)
  {
  m_Data    = NULL;
  m_CurSize = 0;
  m_BlkSize = 0;

  m_ClassActive = m_Resize(pa_InitialSize);
  }


LB_String_T::LB_String_T(LB_ZString_T pa_InString)
  {
  m_Data    = NULL;
  m_CurSize = 0;
  m_BlkSize = 0;

  m_ClassActive = LB_TRUE;

  if (m_Resize(strlen(pa_InString) == LB_TRUE))
    memcpy(m_Data,pa_InString,m_CurSize);
  else
    m_ClassActive = LB_FALSE;
  }


LB_String_T::~LB_String_T()
  {
  LB_ASSERT(m_ClassActive == LB_TRUE);

  m_Resize(0);

  m_ClassActive = LB_FALSE;
  }


//---------------------------------------------------------------
//---                   Overloaded Operators                  ---
//---------------------------------------------------------------
LB_String_T             LB_String_T::operator +  (const LB_String_T& pa_String) const
  {
  LB_String_T   l_String;

  l_String.m_Resize(m_CurSize + pa_String.m_CurSize);

  memcpy(l_String.m_Data,m_Data,m_CurSize);
  memcpy(&l_String.m_Data[m_CurSize],pa_String.m_Data,pa_String.Size());

  return (l_String);
  }


LB_String_T             LB_String_T::operator +  (LB_ZString_T  pa_String)
  {
  LB_String_T   l_String;
  LB_Word_T     l_StrLen;

  l_StrLen = strlen(pa_String);
  LB_ASSERT(l_StrLen < 255);

  l_String.m_Resize(m_CurSize + l_StrLen);

  memcpy(l_String.m_Data,m_Data,m_CurSize);
  memcpy(&l_String.m_Data[m_CurSize],pa_String,l_StrLen);

  return (l_String);
  }


//--- Integer addition
void                    LB_String_T::operator ++ (int)
  {
  LB_FloatPt_T  l_Value;
  char          l_Buf[128];
  LB_Word_T     l_OldSize;

  //--- Save width of string
  l_OldSize = m_CurSize;

  l_Value = AsFloatPt() + 1.00;

  sprintf(l_Buf,"%.0f",l_Value);

  m_Resize(strlen(l_Buf));
  memcpy(m_Data,l_Buf,strlen(l_Buf));

  //--- Restore width of string
  if (m_CurSize < l_OldSize)
    PadLeft(l_OldSize,'0');
  }


LB_String_T&            LB_String_T::operator =  (LB_ZString_T pa_String)
  {
  LB_Word_T l_StrLen;

  l_StrLen = strlen(pa_String);
  LB_ASSERT(l_StrLen < 255);

  if (m_Resize(l_StrLen) == LB_TRUE)
    memcpy(m_Data,pa_String,l_StrLen);

  return (*this);
  }


LB_String_T&            LB_String_T::operator =  (LB_CZString_T pa_String)
  {
  LB_Word_T l_StrLen;

  l_StrLen = strlen(pa_String);
  LB_ASSERT(l_StrLen < 255);

  if (m_Resize(l_StrLen) == LB_TRUE)
    memcpy(m_Data,pa_String,l_StrLen);

  return (*this);
  }


LB_String_T&            LB_String_T::operator =  (const LB_String_T& pa_String)
  {
  LB_Bool_T l_Result;

  if (this == &pa_String)
    return (*this);

  l_Result  = m_Resize(pa_String.Size());

  if (l_Result == LB_TRUE)
    memcpy(m_Data, pa_String.m_Data, m_CurSize);
    //pa_String.GetBytes(m_Data, m_CurSize);

  return (*this);
  }


LB_String_T&            LB_String_T::operator =  (const LB_Word_T pa_Value)
  {
  char        l_Buf[32];
  LB_Bool_T   l_Result;

  sprintf(l_Buf,"%u",pa_Value);

  l_Result  = m_Resize(strlen(l_Buf));
  if (l_Result == LB_TRUE)
    memcpy(m_Data,l_Buf,strlen(l_Buf));

  return (*this);
  }


LB_String_T&            LB_String_T::operator =  (const LB_DWord_T pa_Value)
  {
  char        l_Buf[32];
  LB_Bool_T   l_Result;

  sprintf(l_Buf,"%lu",pa_Value);

  l_Result  = m_Resize(strlen(l_Buf));
  if (l_Result == LB_TRUE)
    memcpy(m_Data,l_Buf,strlen(l_Buf));

  return (*this);
  }


LB_String_T&            LB_String_T::operator =  (const LB_FloatPt_T pa_Value)
  {
  char        l_Buf[64];
  LB_Bool_T   l_Result;

  sprintf(l_Buf,"%.0f",pa_Value);

  l_Result  = m_Resize(strlen(l_Buf));
  if (l_Result == LB_TRUE)
    memcpy(m_Data,l_Buf,strlen(l_Buf));

  return (*this);
  }


LB_Bool_T               LB_String_T::operator ==  (LB_String_T& pa_String)
  {
  if (pa_String.Size() != m_CurSize)
    return (LB_FALSE);

  if (memcmp(m_Data,pa_String.m_Data,pa_String.m_CurSize) == 0)
    return (LB_TRUE);
  else
    return (LB_FALSE);
  }


LB_Bool_T               LB_String_T::operator ==  (LB_ZString_T pa_String)
  {
  if (strlen(pa_String) != m_CurSize)
    return (LB_FALSE);

  if (memcmp(m_Data,pa_String,strlen(pa_String)) == 0)
    return (LB_TRUE);
  else
    return (LB_FALSE);
  }


LB_Bool_T               LB_String_T::operator !=  (LB_String_T& pa_String)
  {
  if (pa_String.m_CurSize != m_CurSize)
    return (LB_TRUE);

  if (memcmp(m_Data,pa_String.m_Data,pa_String.m_CurSize) == 0)
    return (LB_FALSE);
  else
    return (LB_TRUE);
  }


LB_Bool_T               LB_String_T::operator !=  (LB_ZString_T pa_String)
  {
  if (strlen(pa_String) != m_CurSize)
    return (LB_TRUE);

  if (memcmp(m_Data,pa_String,strlen(pa_String)) == 0)
    return (LB_FALSE);
  else
    return (LB_TRUE);
  }


LB_Bool_T               LB_String_T::operator <  (LB_String_T& pa_String)
  {
  if (pa_String.Size() != m_CurSize)
    return (LB_FALSE);

  if (memcmp(m_Data,pa_String.m_Data,pa_String.m_CurSize) < 0)
    return (LB_TRUE);
  else
    return (LB_FALSE);
  }


LB_Byte_T&              LB_String_T::operator[] (LB_Word_T pa_Position)
  {
  LB_ASSERT(pa_Position <= (m_CurSize-1));

  if (pa_Position > (m_CurSize-1))
    {
    LB_ASSERT(pa_Position <= (m_CurSize-1));
    return (m_Data[0]);
    }
  else
    {
    LB_ASSERT(pa_Position <= (m_CurSize-1));
    return (m_Data[pa_Position]);
    }
  }


//---------------------------------------------------------------
//---                   Input/Output Methods                  ---
//---------------------------------------------------------------
LB_Bool_T               LB_String_T::Append(LB_String_T& pa_String)
  {
  LB_Word_T     l_OldSize;
  LB_Bool_T     l_Result;

  l_OldSize = m_CurSize;

  l_Result  = m_Resize(m_CurSize + pa_String.Size());
  LB_ASSERT(l_Result == LB_TRUE);
  if (l_Result == LB_FALSE)
    return (LB_FALSE);

  pa_String.GetBytes(&m_Data[l_OldSize],pa_String.Size());

  return (LB_TRUE);
  }


LB_Bool_T               LB_String_T::Append(LB_CZString_T pa_ZString)
  {
  LB_Word_T     l_OldSize;
  LB_Bool_T     l_Result;
  LB_Word_T     l_StrLen;

  if (pa_ZString == NULL)
    return(LB_FALSE);

  l_StrLen  = strlen(pa_ZString);

  l_OldSize = m_CurSize;

  l_Result  = m_Resize(m_CurSize + l_StrLen);
  LB_ASSERT(l_Result == LB_TRUE);
  if (l_Result == LB_FALSE)
    return (LB_FALSE);

  //--- Append data
  memcpy(&m_Data[l_OldSize],pa_ZString,l_StrLen);

  return (LB_TRUE);
  }


LB_Bool_T               LB_String_T::Append(const LB_Byte_T pa_Byte)
  {
  LB_Bool_T     l_Result;

  l_Result  = m_Resize(m_CurSize + 1);
  LB_ASSERT(l_Result == LB_TRUE);
  if (l_Result == LB_FALSE)
    return (LB_FALSE);

  //--- Append data
  m_Data[m_CurSize-1] = pa_Byte;

  return (LB_TRUE);
  }


LB_Bool_T               LB_String_T::AppendToAsciiZ(LB_ZString_T pa_InString,
                                                    LB_Word_T    pa_MaxSize)
  {
  if (pa_InString == (char *) NULL)
    return (LB_FALSE);

  if (pa_MaxSize < ((strlen(pa_InString) + (m_CurSize + 1))))
    return (LB_FALSE);

  pa_InString = pa_InString + strlen(pa_InString) + 1;

  for (LB_Word_T i=0; i < m_CurSize; i++, pa_InString++)
    *pa_InString = m_Data[i];

  *pa_InString = '\0';

  return (LB_TRUE);
  }


LB_Bool_T               LB_String_T::AssignMoney(const LB_FloatPt_T pa_Value)
  {
  char        l_Buf[64];
  LB_Bool_T   l_Result;

  sprintf(l_Buf,"%.2f",pa_Value);

  l_Result  = m_Resize(strlen(l_Buf));
  if (l_Result == LB_TRUE)
    memcpy(m_Data,l_Buf,m_CurSize);

  return (LB_TRUE);
  }


LB_Bool_T               LB_String_T::Insert(LB_String_T& pa_String)
  {
  LB_Word_T     l_TempSize;
  LB_Byte_T     *l_TempData;
  LB_Bool_T     l_Result;

  //--- Save old data
  l_TempSize = m_CurSize;

  l_TempData = new LB_Byte_T[l_TempSize];
  LB_ASSERT(l_TempData != NULL);
  LB_NEW_INITIALIZE(l_TempData,l_TempSize);

  if (l_TempData == NULL)
    return (LB_FALSE);

  memcpy(l_TempData,m_Data,l_TempSize);

  //--- Create Larger Buffer
  l_Result = m_Resize(m_CurSize + pa_String.Size());
  LB_ASSERT(l_Result == LB_TRUE);
  if (l_Result == LB_FALSE)
    return (LB_FALSE);

  //--- Insert new data
  pa_String.GetBytes(&m_Data[0],pa_String.Size());

  //--- Restore original data
  memcpy(&m_Data[pa_String.Size()],l_TempData,l_TempSize);

  //--- Remove temporary buffer
  LB_DEL_INITIALIZE(l_TempData,l_TempSize);
  delete[] l_TempData;

  return (LB_TRUE);
  }


LB_Bool_T               LB_String_T::Insert(LB_CZString_T pa_ZString)
  {
  LB_Word_T     l_StrLen;
  LB_Word_T     l_TempSize;
  LB_Byte_T     *l_TempData;
  LB_Bool_T     l_Result;

  if (pa_ZString == NULL)
    return(LB_FALSE);

  l_StrLen = strlen(pa_ZString);

  l_TempSize = m_CurSize;
  l_TempData = new LB_Byte_T[l_TempSize];
  LB_ASSERT(l_TempData != NULL);
  LB_NEW_INITIALIZE(l_TempData,l_TempSize);

  if (l_TempData == NULL)
    return (LB_FALSE);

  memcpy(l_TempData,m_Data,l_TempSize);

  //--- Create Larger Buffer
  l_Result = m_Resize(m_CurSize + l_StrLen);
  LB_ASSERT(l_Result == LB_TRUE);
  if (l_Result == LB_FALSE)
    return (LB_FALSE);

  //--- Insert new data
  memcpy(&m_Data[0],pa_ZString,strlen(pa_ZString));

  //--- Restore original data
  memcpy(&m_Data[strlen(pa_ZString)],l_TempData,l_TempSize);

  //--- Remove temporary buffer
  delete[] l_TempData;

  return (LB_TRUE);
  }


//--- Search a string for pa_SearchString.
LB_Bool_T               LB_String_T::Search(LB_String_T& pa_SearchString,
                                            LB_Bool_T    pa_IgnoreCase)
  {
  LB_Word_T   l_SearchLen;
  LB_Word_T   l_Index;
  LB_String_T l_CurrentString;
  LB_String_T l_SearchString;
  LB_String_T l_Substr;

  //--- Is search string empty?
  if (pa_SearchString.Size() < 1)
    return (LB_FALSE);

  //--- Is search string bigger than the string to be searched?
  if (pa_SearchString.Size() > m_CurSize)
    return (LB_FALSE);

  //--- Copy current string to a temporary string
  l_CurrentString.PadRight(m_CurSize,' ');
  for (l_Index=0; l_Index < m_CurSize; l_Index++)
    l_CurrentString[l_Index] = m_Data[l_Index];

  l_SearchString = pa_SearchString;

  if (pa_IgnoreCase == LB_TRUE)
    {
    l_CurrentString.LowerCase();
    l_SearchString.LowerCase();
    }

  l_SearchLen = l_CurrentString.Size() - l_SearchString.Size();

  for (l_Index=0; l_Index < l_SearchLen; l_Index++)
    {
    l_Substr = l_CurrentString.SubstrMid(l_Index,l_SearchString.Size());
    if (l_Substr == l_SearchString)
      return (LB_TRUE);
    }

  return (LB_FALSE);
  }


//--- Place user byte array into string
LB_Bool_T               LB_String_T::SetBytes(LB_Byte_T *pa_Bytes,
                                              LB_Word_T pa_Size)
  {
  LB_Bool_T     l_Result;

  if (pa_Bytes == NULL)
    return(LB_FALSE);

  //--- Create Larger Buffer
  l_Result = m_Resize(pa_Size);
  LB_ASSERT(l_Result == LB_TRUE);
  if (l_Result == LB_FALSE)
    return (LB_FALSE);

  //--- Copy data from user buffer into class buffer
  memcpy(m_Data,pa_Bytes,pa_Size);

  return (LB_TRUE);
  }


//--- Retrieve bytes from string and place into user byte array
LB_Bool_T               LB_String_T::GetBytes(LB_Byte_T *pa_Bytes,
                                              LB_Word_T pa_MaxSize)
  {
  LB_Word_T l_Cnt;

  if (pa_Bytes == NULL)
    return (LB_FALSE);

  l_Cnt = pa_MaxSize;
  if (l_Cnt > m_CurSize)
    l_Cnt = m_CurSize;

  memcpy(pa_Bytes,m_Data,l_Cnt);

  return (LB_TRUE);
  }


//---------------------------------------------------------------
//---                   Conversion Methods                    ---
//---------------------------------------------------------------
LB_Bool_T               LB_String_T::AsAsciiZ(LB_ZString_T pa_InString,
                                              LB_Word_T    pa_MaxSize)
  {
  if (pa_InString == NULL)
    return (LB_FALSE);

  if (pa_MaxSize < (m_CurSize + 1))
    return (LB_FALSE);

  for (size_t i=0; i < m_CurSize; i++, pa_InString++)
    *pa_InString = m_Data[i];

  *pa_InString = '\0';

  return (LB_TRUE);
  }


LB_FloatPt_T            LB_String_T::AsFloatPt()
  {
  char          *l_BufPtr;
  LB_FloatPt_T  l_Value;

  l_BufPtr = new char[m_CurSize + 1];

  AsAsciiZ(l_BufPtr,m_CurSize+1);

  l_Value = strtod(l_BufPtr,NULL);

  delete[] l_BufPtr;

  return ((LB_FloatPt_T) (l_Value));
  }


LB_Word_T               LB_String_T::AsWord()
  {
  char          *l_BufPtr;
  LB_Word_T     l_Value;

  l_BufPtr = new char[m_CurSize + 1];

  AsAsciiZ(l_BufPtr,m_CurSize+1);

  l_Value = atoi(l_BufPtr);

  delete[] l_BufPtr;

  return ((LB_Word_T) (l_Value));
  }


void                    LB_String_T::UpperCase()
  {
  size_t i;

  for (i=0; i < m_CurSize; i++)
    m_Data[i] = toupper(m_Data[i]);
  }


void                    LB_String_T::LowerCase()
  {
  size_t i;

  for (i=0; i < m_CurSize; i++)
    m_Data[i] = tolower(m_Data[i]);
  }


void                    LB_String_T::Proper()
  {
  LB_String_T   l_Result;
  LB_String_T   l_TempStr;
  LB_Word_T     l_Index;
  LB_Word_T     l_Index2;
  l_Index = 0;

  while (l_Index < m_CurSize)
    {
    //--- Find first letter, skip past non-alpha
    while ((!isalpha(m_Data[l_Index])) && (l_Index < m_CurSize))
      {
      l_Result.Append(m_Data[l_Index]);
      l_Index++;
      }

    //--- No letters found
    if (l_Index >= m_CurSize)
      break;

    //--- Build new string
    l_TempStr.Clear();
    while (l_Index < m_CurSize)
      {
      if (!isalpha(m_Data[l_Index]))
        {
        if (m_Data[l_Index] != '\'')
          break;
        }
      l_TempStr.Append(m_Data[l_Index]);
      l_Index++;
      }

    l_TempStr.LowerCase();

    //--- Capitalize II,III,etc
    for (l_Index2=0; l_Index2 < l_TempStr.Size(); l_Index2++)
      {
      if (l_TempStr[0] != l_TempStr[l_Index2])
        break;
      }
    if (l_Index2 >= l_TempStr.Size())
      l_TempStr.UpperCase();
    else if (l_TempStr == "nj")
      l_TempStr.UpperCase();
    else if (l_TempStr == "pa")
      l_TempStr.UpperCase();
    else if ((l_TempStr[0] == 'm') &&
             (l_TempStr[1] == 'c') && (l_TempStr.Size() > 3))
      {
      l_TempStr[0] = toupper(l_TempStr[0]);
      l_TempStr[2] = toupper(l_TempStr[2]);
      }
    else
      l_TempStr[0] = toupper(l_TempStr[0]);

    l_Result = l_Result + l_TempStr;
    }
  l_Result.GetBytes(m_Data,m_CurSize);
  }


//---------------------------------------------------------------
//---                   Utility Methods                       ---
//---------------------------------------------------------------
void                    LB_String_T::Clear(void)
  {
  LB_ASSERT(m_ClassActive == LB_TRUE);

  m_Resize(0);
  }


void                    LB_String_T::DiagDisplay()
  {
  LB_Word_T     l_Index;

  printf("\nCurSize=%d, BlkSize=%d, Value is [",m_CurSize,m_BlkSize);

  for (l_Index=0; l_Index < m_CurSize; l_Index++)
    if (!iscntrl(m_Data[l_Index]))
      printf("%c",m_Data[l_Index]);
    else
      printf("!");

  printf("]");
  }


void                    LB_String_T::Display()
  {
  LB_Word_T     l_Index;

  for (l_Index=0; l_Index < m_CurSize; l_Index++)
    if (!iscntrl(m_Data[l_Index]))
      printf("%c",m_Data[l_Index]);
    else
      printf("!");
  }


void                    LB_String_T::Fill(const LB_Byte_T pa_FillByte)
  {
  memset(m_Data,pa_FillByte,m_CurSize);
  }


LB_Bool_T               LB_String_T::Filter(const LB_Byte_T pa_Byte)
  {
  LB_String_T   l_String;
  LB_Word_T     l_Index;

  for (l_Index=0; l_Index < m_CurSize; l_Index++)
    {
    if (m_Data[l_Index] != pa_Byte)
      {
      if (l_String.Append(m_Data[l_Index]) == LB_FALSE)
        return (LB_FALSE);
      }
    }

  if (m_Resize(l_String.Size()) == LB_FALSE)
    return (LB_FALSE);

  l_String.GetBytes(m_Data,m_CurSize);

  return (LB_TRUE);
  }


//
//      Originally published in the System V Application Binary Interface,
//      Unix Press, 1990. Second reference in Practical Algorithms for
//      Programmers, Addison-Wesley, 1995.
//
//      This is the hash implementation used in ELF object files under UNIX.
//
//      Portability: Requires a 32-bit return value.
//
LB_DWord_T              LB_String_T::HashKey()
  {
  LB_DWord_T    l_HashKey;
  LB_DWord_T    l_Temp;
  LB_Word_T     l_Index;

  l_HashKey = 0;

  for (l_Index=0; l_Index < m_CurSize; l_Index++)
    {
    l_HashKey = (l_HashKey << 4) + m_Data[l_Index];

    l_Temp = l_HashKey & 0xF0000000;
    if (l_Temp != 0)
      l_HashKey ^= l_Temp >> 24;

    l_HashKey &= ~l_Temp;
    }

  return (l_HashKey);
  }


LB_Bool_T               LB_String_T::IsAllSpaces()  const
  {
  LB_Word_T     l_Index;

  if (m_CurSize <= 0)
    return (LB_FALSE);

  for (l_Index=0; l_Index < m_CurSize; l_Index++)
    {
    if (m_Data[l_Index] != ' ')
      return (LB_FALSE);
    }

  return (LB_TRUE);
  }


LB_Bool_T         	LB_String_T::IsEmpty()  const
  {
  if (m_CurSize <= 0)
    return (LB_TRUE);
  else
    return (LB_FALSE);
  }


void                    LB_String_T::MakeNumeric()
  {
  LB_Word_T l_Index;

  for (l_Index=0; l_Index < m_CurSize; l_Index++)
    {
    switch(m_Data[l_Index])
      {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case '.':
        break;

      case '+':
      case '-':
        m_Data[0] = m_Data[l_Index];
        m_Data[l_Index] = '0';

      default:
        m_Data[l_Index] = '0';
      }
    }
  }


LB_Bool_T               LB_String_T::PadLeft(LB_Word_T pa_Size,
                                             LB_Byte_T pa_PadChar)
  {
  LB_Bool_T     l_Result;
  LB_Byte_T     *l_TempData;
  LB_Word_T     l_TempSize;

  //--- Already padded
  if (pa_Size == m_CurSize)
    return (LB_TRUE);

  //--- Save existing data
  l_TempSize = m_CurSize;
  l_TempData = new LB_Byte_T[l_TempSize];
  LB_ASSERT(l_TempData != NULL);
  LB_NEW_INITIALIZE(l_TempData,l_TempSize);
  if (l_TempData == NULL)
    return (LB_FALSE);
  memcpy(l_TempData,m_Data,l_TempSize);

  //--- Create new buffer
  l_Result = m_Resize(pa_Size);
  LB_ASSERT(l_Result == LB_TRUE);
  if (l_Result == LB_FALSE)
    return (LB_FALSE);

  //--- Pad with pad char
  Fill(pa_PadChar);

  //--- Restore buffer on right-hand side
  memcpy(&m_Data[m_CurSize-l_TempSize],l_TempData,l_TempSize);
  delete[] l_TempData;

  return (LB_TRUE);
  }


LB_Bool_T               LB_String_T::PadRight(LB_Word_T pa_Size,
                                              LB_Byte_T pa_PadChar)
  {
  LB_Word_T     l_TempSize;

  //--- Already padded
  if (pa_Size == m_CurSize)
    return (LB_TRUE);

  l_TempSize = m_CurSize;

  m_Resize(pa_Size);

  for (LB_Word_T i=l_TempSize; i < m_CurSize; i++)
    m_Data[i] = pa_PadChar;

  return (LB_TRUE);
  }


LB_String_T            LB_String_T::SubstrLeft(LB_Word_T pa_Length) const
  {
  LB_String_T   l_ReturnString;

  if (m_CurSize == 0)
    return (l_ReturnString);

  LB_ASSERT(m_Data != NULL);

  //--- Check if size in bounds
  if (pa_Length > m_CurSize)
    pa_Length = m_CurSize;

  l_ReturnString.SetBytes(m_Data,pa_Length);

  return (l_ReturnString);
  }


LB_String_T            LB_String_T::SubstrMid(LB_Word_T pa_Offset,
                                              LB_Word_T pa_Length) const
  {
  LB_String_T   l_ReturnString;

  if ((m_CurSize == 0) || (m_Data == NULL))
    return (l_ReturnString);

  //--- Check if size in bounds
  if (pa_Offset > m_CurSize)
    pa_Offset = m_CurSize;

  if ((pa_Offset + pa_Length) > m_CurSize)
    pa_Length = (m_CurSize - pa_Offset);

  l_ReturnString.SetBytes(&m_Data[pa_Offset],pa_Length);

  return (l_ReturnString);
  }


LB_String_T            LB_String_T::SubstrRight(LB_Word_T pa_Length) const
  {
  LB_String_T   ReturnString;
  LB_ZString_T  ZString;

  if (m_CurSize == 0)
    return (ReturnString);

  LB_ASSERT(m_Data != NULL);

  //--- Check if size in bounds
  if (pa_Length > m_CurSize)
    pa_Length = m_CurSize;

  //--- Generate temp string buffer
  ZString = new char[pa_Length + 1];
  LB_ASSERT(ZString);
  LB_NEW_INITIALIZE((LB_Byte_T *) ZString, pa_Length + 1);

  //--- New failed
  if (ZString == (char *) NULL)
    return (ReturnString);

  memcpy(ZString, &m_Data[m_CurSize - pa_Length], pa_Length);
  ZString[pa_Length] = '\0';

  ReturnString = ZString;

  LB_DEL_INITIALIZE((LB_Byte_T *) ZString, pa_Length+1);
  delete[] ZString;

  return (ReturnString);
  }


void                    LB_String_T::Trace(char * pa_SrcFileName,
                                           int    pa_SrcLineNumber)
{
  FILE *        TraceFilePtr;
  char          TimeBuffer[36];
  struct tm *   TimeNow;
  time_t        SecsNow;

  TraceFilePtr = fopen("tracelog.txt","a+");
  if (TraceFilePtr == (FILE *) NULL)
    return;

  //--- Write timestamp, source filename, source line number
  time(&SecsNow);
  TimeNow = localtime(&SecsNow);
  strftime(TimeBuffer, sizeof(TimeBuffer), "%m/%d/%Y at %H:%M:%S%p", TimeNow);
  fprintf(TraceFilePtr,"%s - %s, Line %d\n",TimeBuffer, pa_SrcFileName, pa_SrcLineNumber);

  //--- Write message
  fprintf(TraceFilePtr, "(%d bytes): '", m_CurSize);
  for (int i=0; i < m_CurSize; i++)
  {
    fprintf(TraceFilePtr,"%c", m_Data[i]);
  }
  fprintf(TraceFilePtr, "'\n");

  fclose(TraceFilePtr);
}
 

LB_Bool_T               LB_String_T::Trim()
  {
  LB_Bool_T l_Result;

  l_Result = TrimRight();
  LB_ASSERT(l_Result == LB_TRUE);

  if (l_Result == LB_TRUE)
    {
    l_Result = TrimLeft();
    LB_ASSERT(l_Result == LB_TRUE);
    if (l_Result == LB_TRUE)
      return (LB_TRUE);
    }

  return (LB_FALSE);
  }


LB_Bool_T         	LB_String_T::TrimLeft()
  {
  LB_Byte_T     *l_TempData;
  LB_Word_T     l_TempSize;
  LB_Word_T     l_SpaceCntr;
  LB_Bool_T     l_Result;

  if ((m_Data == NULL) || (m_CurSize <= 0))
    return (LB_TRUE);

  //--- Find count of spaces at start of string
  for (l_SpaceCntr=0;
       (isspace(m_Data[l_SpaceCntr]) && (l_SpaceCntr < m_CurSize));
       l_SpaceCntr++)
    NULL;

  if (l_SpaceCntr > 0)
    {
    //--- Save existing data
    l_TempSize = m_CurSize - l_SpaceCntr;
    l_TempData = new LB_Byte_T[l_TempSize];
    LB_ASSERT(l_TempData != NULL);
    LB_NEW_INITIALIZE(l_TempData,l_TempSize);
    if (l_TempData == NULL)
      return (LB_FALSE);
    //--- Copy everything after spaces
    memcpy(l_TempData,&m_Data[l_SpaceCntr],l_TempSize);

    //--- Create new buffer
    l_Result = m_Resize(l_TempSize);
    LB_ASSERT(l_Result == LB_TRUE);
    if (l_Result == LB_FALSE)
      return (LB_FALSE);

    //--- Restore saved data
    memcpy(m_Data,l_TempData,l_TempSize);
    delete[] l_TempData;
    }

  return (LB_TRUE);
  }


LB_Bool_T           LB_String_T::TrimRight()
  {
  LB_Word_T     l_NewSize;

  if ((m_Data == NULL) || (m_CurSize == 0))
    return (LB_TRUE);

  //--- Start from end of string, search backwards for first space
  for (l_NewSize = m_CurSize-1;
       (isspace(m_Data[l_NewSize]) && (l_NewSize != 0));
       l_NewSize--)
    NULL;

  //--- Array is zero relative, and we want a COUNT, not the POSITION
  if (! isspace(m_Data[l_NewSize]))
    l_NewSize = l_NewSize + 1;

  if (l_NewSize < m_CurSize)
    m_Resize(l_NewSize);

  return (LB_TRUE);
  }


LB_Bool_T               LB_String_T::TrimTo(LB_Word_T ByteCnt)
{
    if ((m_Data == NULL) || (m_CurSize == 0))
        return (LB_TRUE);

    if (ByteCnt > m_CurSize)
        return (LB_TRUE);    

    m_Resize(ByteCnt);

    return (LB_TRUE);
}


LB_Word_T               LB_String_T::Size()     const
  {
  return (m_CurSize);
  }


LB_Word_T               LB_String_T::BlockSize()     const
  {
  return (m_BlkSize);
  }
