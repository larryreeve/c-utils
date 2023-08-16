/*  lbdigsig.cpp
 *
 *  Digital Signature and Validation routines
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
 *      04/02/96        LR      -Initial Release
 *
 *
 * Notes:
 *      Based on "Practical Algorithms for Programmers", pp.564-570.
 *
**/


//---------------------------------------------------------------
//---                INCLUDE FILES                            ---
//---------------------------------------------------------------
#include <stdio.h>
#include <lbport.h>
#include <lbdebug.hpp>
#include <lbstring.hpp>
#include <lbdigsig.hpp>

LB_ASSERTFILE(__FILE__)

//---------------------------------------------------------------
//---                   Public  Methods                       ---
//---------------------------------------------------------------
LB_DigSig_T::LB_DigSig_T()
  {
  LB_Word_T l_Index;
  LB_Word_T l_Index2;

  //--- Create Crc32 Table
  for (l_Index=0; l_Index < 256; l_Index++)
    {
    m_Crc32Table[l_Index] = l_Index;
    for (l_Index2=8; l_Index2 > 0; l_Index2--)
      {
      if (m_Crc32Table[l_Index] & 1)
        m_Crc32Table[l_Index] = (m_Crc32Table[l_Index] >> 1) ^ 0xEDB88320L;
      else
        m_Crc32Table[l_Index] = (m_Crc32Table[l_Index] >> 1);
      }
    }
  }


LB_DigSig_T::~LB_DigSig_T()
  {
  }


void                    LB_DigSig_T::Crc32_Reset()
  {
  //--- Pre-Condition
  m_Crc32 = 0xFFFFFFFF;
  }

LB_DWord_T              LB_DigSig_T::Crc32()
  {
  //--- Post Condition
  m_Crc32 = ~m_Crc32;

  return (m_Crc32);
  }

void                    LB_DigSig_T::Crc32_Add(LB_Byte_T pa_Byte)
{
  int IntVal;

  IntVal = (m_Crc32 ^ pa_Byte) & 0x000000FFL;

  LB_ASSERT(IntVal >= 0); LB_ASSERT(IntVal <= 255);

  m_Crc32 = ((m_Crc32 >> 8) & 0x00FFFFFFL) ^ m_Crc32Table[IntVal];
}



LB_DWord_T              LB_DigSig_T::Crc32(LB_String_T& pa_InputStr)
  {
  LB_Word_T     Idx;
  int           IntVal;
  LB_DWord_T    Crc32;

  //--- Pre-Condition
  Crc32 = 0xFFFFFFFF;

  for (Idx=0; Idx < pa_InputStr.Size(); Idx++)
    {
    IntVal = (Crc32 ^ pa_InputStr[Idx]) & 0x000000FFL;

    LB_ASSERT(IntVal >= 0); LB_ASSERT(IntVal <= 255);

    Crc32 = ((Crc32 >> 8) & 0x00FFFFFFL) ^ m_Crc32Table[IntVal];
    }

  //--- Post Condition
  Crc32 = ~Crc32;

  return (Crc32);
  }
