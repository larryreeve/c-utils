/*  lbcblcmp.cpp
 *
 *  COBOL COMP Data Type Processing routines
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
 *            COPYRIGHT (C) 1995 UNITED COMPUTER SALES & SERVICE, INC.
 *
 *
 * Revision History:
 *
 *      Date            By      Notes
 *      --------        ---     ------------------------------------------
 *      11/25/95        LR      -Initial Release
 *      02/16/96        LR      -Added support for signed values
 *      03/22/96        LR      -Decode now inputs a string instead of
 *                              a byte array. Also, the string is copied
 *                              before it is manipulated, to prevent
 *                              changing the value and causing failures
 *                              on the second call to decode.
 *
 *
 * Notes:
 *
 *
**/


/*---------------------------------------------------------------
  ---                INCLUDE FILES                            ---
  ---------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include <time.h>
#include <lbport.h>
#include <lbdebug.hpp>
#include <lbcblcmp.hpp>

LB_ASSERTFILE(__FILE__)


/*---------------------------------------------------------------
  ---                   Private Methods                       ---
  ---------------------------------------------------------------*/
/*
 *
**/
void            LB_CobolComp_T::m_SwapBytes(LB_Byte_T     *pa_Bytes,
                                            LB_Size_T     pa_Size)
  {
  LB_Byte_T     *l_OrigPtr;
  LB_Size_T     l_BegBuf,l_EndBuf;

  LB_ASSERT(pa_Size > 0);

//  printf("\norig bytes   = ");
//  for (int i=0; i < pa_Size; i++)
//    printf("%02X/",pa_Bytes[i]);
//  printf(" -> %d bytes",pa_Size);

  //--* Allocate and clear to zeros
  l_OrigPtr = new LB_Byte_T[pa_Size];
  LB_ASSERT(l_OrigPtr != NULL);
  memset(l_OrigPtr,0,pa_Size);

  //--* Make copy of original byte array
  memcpy(l_OrigPtr,pa_Bytes,(LB_Size_T) pa_Size);
//  printf("\ncopied bytes = ");
//  for (int j=0; j < pa_Size; j++)
//    printf("%02X/",l_OrigPtr[j]);
//  printf(" -> %d bytes",pa_Size);

  //--* Reverse bytes
  for (l_EndBuf=pa_Size-1,l_BegBuf=0; l_BegBuf < pa_Size; l_BegBuf++,l_EndBuf--)
    pa_Bytes[l_BegBuf] = l_OrigPtr[l_EndBuf];

  delete l_OrigPtr;

//  printf("\nchange bytes = ");
//  for (int k=0; k < pa_Size; k++)
//    printf("%02X/",pa_Bytes[k]);
//  printf(" -> %d bytes",pa_Size);
//  getchar();
  }


/*---------------------------------------------------------------
  ---                   Public  Methods                       ---
  ---------------------------------------------------------------*/
/*
 *
**/
LB_Bool_T       LB_CobolComp_T::Decode(LB_FloatPt_T  *pa_Result,
                                       LB_String_T&  pa_Bytes,
                                       LB_Word_T     pa_NumDecimals,
                                       LB_Bool_T     pa_SignFlag)
  {
  LB_FloatPt_T  l_Exp;
  LB_Bool_T     l_IsNegative;
  LB_Word_T	l_Index;
  LB_Word_T     l_Offset;
  LB_String_T   l_Bytes;

  *pa_Result = 0.0;

  l_Bytes = pa_Bytes;

  if (l_Bytes.Size() == 0)
    return (LB_FALSE);

  m_SwapBytes(&l_Bytes[0],l_Bytes.Size());

  //--* Check for neg value if signed
  if (pa_SignFlag == LB_TRUE)
    {
    if ((l_Bytes[l_Bytes.Size() - 1] & 128) == 128)
      l_IsNegative = LB_TRUE;
    else
      l_IsNegative = LB_FALSE;
    }

  for (l_Offset=0; l_Offset < l_Bytes.Size(); l_Offset++)
    {
    //--* Determine (l_Offset ** 256)
    l_Exp = 1.0;
    for (l_Index=l_Offset; l_Index > 0; l_Index--)
      l_Exp = l_Exp * 256.0;

    //--* Add in next byte's value
    if (l_IsNegative == LB_TRUE)
      //--* Two's complement
      *pa_Result = *pa_Result + (((0xFF - l_Bytes[l_Offset]) * 1.0) * l_Exp);
    else
      *pa_Result = *pa_Result + ((l_Bytes[l_Offset] * 1.0) * l_Exp);
    }

  if (l_IsNegative == LB_TRUE)
    {
    *pa_Result = *pa_Result + 1;
    *pa_Result = *pa_Result * (-1.00);
    }

  //--* Factor in decimal point
  for (l_Index=0; l_Index < pa_NumDecimals; l_Index++)
    *pa_Result = *pa_Result / 10.0;

// printf("\nDecode: Final Result = %.4f",*pa_Result);

  return (LB_TRUE);
  }



/*
 *
**/
LB_Bool_T       LB_CobolComp_T::Encode(LB_FloatPt_T   pa_Value,
				       LB_Byte_T      *pa_Bytes,
                                       LB_Size_T      pa_Size,
                                       LB_Word_T      pa_NumDecimals,
                                       LB_Bool_T      pa_SignFlag)
  {
  LB_FloatPt_T  l_Exp;
  LB_FloatPt_T  l_DivValue;
  LB_Size_T     l_Offset;
  LB_Word_T     l_IntValue;
  LB_Word_T	l_Index;
  LB_Bool_T     l_IsNegative;
  char          l_Buf[64];

  if (pa_Size == 0)
    return (LB_FALSE);

  memset(pa_Bytes,0,pa_Size);

  //--* Determine sign
  l_IsNegative = LB_FALSE;
  if (pa_Value < 0.00)
    {
    if (pa_SignFlag == LB_TRUE)
      l_IsNegative = LB_TRUE;
    pa_Value = pa_Value * (-1.00);
    }

  //--* Promote fractional part of number to whole number
  for (l_Index=0; l_Index < pa_NumDecimals; l_Index++)
    pa_Value = pa_Value * 10.0;

  for (l_Offset=pa_Size; l_Offset > 0; l_Offset--)
    {
    //--* Determine (256 ** l_Offset)
    l_Exp = pow(256,l_Offset-1);

    //--* Divide out
    l_DivValue = pa_Value / l_Exp;
    sprintf(l_Buf,"%12.8f",l_DivValue);
    l_IntValue = atoi(l_Buf);

// printf("\nBuf='%s',\tIntVaue=%3d",l_Buf,l_IntValue);
// printf("\nEncode: DivValue=%.04f,pa_Value=%.04f,Exp=%.04f, IntValue=%d",l_DivValue,pa_Value,l_Exp,l_IntValue);

    //--* Account for sign
    if (l_IsNegative == LB_FALSE)
      pa_Bytes[l_Offset-1] = l_IntValue;
    else
      {
      //--* Two's complement
      pa_Bytes[l_Offset-1] = (0xFF - l_IntValue);
      }

    //--* Reduce value
    pa_Value = pa_Value - ((l_IntValue * 1.0) * l_Exp);
    }

    if (l_IsNegative == LB_TRUE)
      pa_Bytes[0] = pa_Bytes[0] + 1;

//  printf("\nEncode: Bytes values are: ");
//  for (l_Index=0; l_Index < pa_Size; l_Index++)
//    printf("%02X-",pa_Bytes[l_Index]);
//  printf("\nEncode: Value is %.4f (should be 0.00)",pa_Value);


  m_SwapBytes(pa_Bytes,pa_Size);

  return (LB_TRUE);
  }
