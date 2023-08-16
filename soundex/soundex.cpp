/*  lbphonic.cpp
 *
 *  Phonetic Code Generation routines
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
 *    Soundex method:
 *      Generates soundex based on soundex algorithm based on Volume 3
 *      of Knuth's Algorithms.
 *
 *      Input string converted to form XYYY, where:
 *              X       = first letter
 *              YYY     = soundex values of next non-vowels. Doubled values
 *              of consecutive Ys are ignored.
 *
 *      The following are test values as documented on page 392:
 *
 *              Euler   - E460                  Gauss - G200
 *              Hilbert - H416                  Knuth - K530
 *              Lloyd   - L300
 *
 *
**/


/*---------------------------------------------------------------
  ---                INCLUDE FILES                            ---
  ---------------------------------------------------------------*/
#include <stdio.h>
#include <ctype.h>      //--* Needed for isalpha test
#include <memory.h>     //--* Needed for memcpy in constructor
#include <lbport.h>     //--* Portability Definitions
#include <lbphonic.hpp> //--* Class Definition


/*---------------------------------------------------------------
  ---                   Public  Methods                       ---
  ---------------------------------------------------------------*/

/*
 * The constructor simply initialized the soundex table with initial
 * values given by Knuth.
**/
LB_Phonics_T::LB_Phonics_T()
  {
  //--*                  ABCDEFGHIJKLMNOPQRSTUVWXYZ
  memcpy(m_SoundexTable,"01230120022455012623010202",sizeof(m_SoundexTable));
  }


/*
 *
 *
 *
**/
LB_String_T             LB_Phonics_T::Soundex(LB_String_T& pa_InputStr)
  {
  LB_String_T   l_Soundex;      //--* The result of the method
  LB_String_T   l_Value;        //--* The input string which is modified to uppercase
  LB_Word_T     l_ValueIndex;   //--* Index into value string
  LB_Word_T     l_SndxIndex;
  LB_Byte_T     l_PrevCode;     //--* Used to compare for adjacent codes
  LB_Byte_T     l_CurrCode;     //--* Used to compare for adjecent codes

  //--* Initial values
  l_Soundex = "0000";           //--* Set default response
  l_Value   = pa_InputStr;      //--* Store   user input string
  l_Value.UpperCase();          //--* Convert user input string to upper case

  //--* Check if zero length string (invalid)
  if (l_Value.Size() == 0)
     return (l_Soundex);

  //--* Step1: Skip past non-alpha characters to get to first letter in string
  l_ValueIndex = 0;
  while ((l_ValueIndex < l_Value.Size()) && (!isalpha(l_Value[l_ValueIndex])))
    l_ValueIndex++;

  //--* If the end of the user input string was reached before a non-alpha
  //--* character could be found, then return the default Soundex.
  if (l_ValueIndex >= l_Value.Size())
    return (l_Soundex);

  //--* Step 2: Set the first character of the Soundex string to the first
  //--* alpha character in the string.
  l_Soundex[0] = l_Value[l_ValueIndex];

  //--* Store the code stored in the Soundex result
  l_PrevCode   = m_SoundexTable[l_Soundex[0] - 'A'];

  //--* Skip past the first alpha character in the user input string
  l_ValueIndex = l_ValueIndex + 1;

  //--* Start the index into the Soundex result; since the initial
  //--* character has already been set, start at 1 and not 0.
  l_SndxIndex  = 1;

  //--* Step 3: Loop through building the Soundex result until
  //--* 1) the user input string has been exhausted or 2) 3 more
  //--* characters have been added to the Soundex result, for a
  //--* final total of 4 characters which is the result.
  while ((l_ValueIndex < l_Value.Size()) && (l_SndxIndex <= 3))
    {
    //--* Skip non-alpha characters
    if (isalpha(l_Value[l_ValueIndex]))
      {
      //--* Convert character into a Soundex value
      l_CurrCode = m_SoundexTable[l_Value[l_ValueIndex] - 'A'];
      //--* Drop vowels and characters H, W, and Y
      if (l_CurrCode != '0')
        {
        //-----* If adjacent codes match, then don't repeat
        if (l_CurrCode != l_PrevCode)
          {
          //--* Add Soundex value to Soundex result
          l_Soundex[l_SndxIndex] = l_CurrCode;
          l_SndxIndex = l_SndxIndex + 1;
          }
        }
      //--* Store the current code for the next comparison
      l_PrevCode = l_CurrCode;
      }
    //--* Get the next character in the user input string
    l_ValueIndex = l_ValueIndex + 1;
    }

  return (l_Soundex);
  }
