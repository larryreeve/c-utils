/*  lbcrypt.cpp
 *
 *  Encryption/Decryption routines
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
 *      03/11/96        LR      -Initial Release
 *
 *
 * Notes:
 *
 *      "The S-Coder algorithm is a variant of the exclusive-OR method.
 *      It diifers primarily in that it scrambles the key on the fly
 *      (characteristic of a stream cipher) to approach the security
 *      level of using a one-time random key."
 *
 *      Based on the article "S-CODER for Data Encryption", Robert B. Stout,
 *      Dr. Dobbs Journal, January 1990, pp. 52-58
 *
 *
**/


/*---------------------------------------------------------------
  ---                INCLUDE FILES                            ---
  ---------------------------------------------------------------*/
#include <stdio.h>
#include <memory.h>
#include <lbport.h>
#include <lbcrypt.hpp>


/*---------------------------------------------------------------
  ---                   Public  Methods                       ---
  ---------------------------------------------------------------*/

/*
 *
**/
LB_Crypt_T::LB_Crypt_T()
  {
  m_Key = "LARRYREEVELIKESJAVAIN1996";
  }


/*
 *
**/
LB_String_T             LB_Crypt_T::m_Crypt(LB_String_T& pa_InputStr)
  {
  LB_String_T   l_CrypKey;
  LB_String_T   l_CrypValue;
  LB_Word_T     l_CrypIndex;
  LB_Word_T     l_Index;

  //--* Initial values
  l_CrypKey   = m_Key;
  l_CrypIndex = 0;
  l_CrypValue = pa_InputStr;

  for (l_Index=0; l_Index < l_CrypValue.Size(); l_Index++)
    {
    l_CrypValue[l_Index] ^= l_CrypKey[l_CrypIndex] ^ (l_CrypKey[0] * l_CrypIndex);

    if (l_CrypIndex < (l_CrypKey.Size() - 1))
      l_CrypKey[l_CrypIndex] = l_CrypKey[l_CrypIndex] + l_CrypKey[l_CrypIndex+1];
    else
      l_CrypKey[l_CrypIndex] = l_CrypKey[l_CrypIndex] + l_CrypKey[0];

    if (!l_CrypKey[l_CrypIndex])
      l_CrypKey[l_CrypIndex] = l_CrypKey[l_CrypIndex] + 1;

    l_CrypIndex = l_CrypIndex + 1;
    if (l_CrypIndex >= l_CrypKey.Size())
      l_CrypIndex = 0;
    }

  return (l_CrypValue);
  }


/*
 *
**/
LB_String_T             LB_Crypt_T::Encrypt(LB_String_T& pa_InputStr)
  {
  LB_String_T   l_CrypValue;

  l_CrypValue = m_Crypt(pa_InputStr);

  return (l_CrypValue);
  }


/*
 *
**/
LB_String_T             LB_Crypt_T::Decrypt(LB_String_T& pa_InputStr)
  {
  LB_String_T   l_CrypValue;

  l_CrypValue = m_Crypt(pa_InputStr);

  return (l_CrypValue);
  }
