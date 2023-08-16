/*  lbhshlst.cpp
 *
 *  Hash List Processing routines
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
 *      03/21/96        LR      -Initial Release
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
#include <string.h>
#include <memory.h>
#include <lbport.h>
#include <lbdebug.hpp>
#include <lbhshlst.hpp>


LB_ASSERTFILE(__FILE__)


/*---------------------------------------------------------------
  ---                   Private Methods                       ---
  ---------------------------------------------------------------*/
/*
 *
**/
LB_Bool_T               LB_HashList_T::m_Search(LB_String_T& pa_KeyString,
                                                LB_Word_T    pa_KeyOffset,
                                                LB_Word_T    pa_KeyLength,
                                                LB_String_T& pa_Value)
  {
  struct HASHLIST_NODE_T *l_NodePtr;
  struct HASHLIST_NODE_T *l_PrevPtr;
  LB_String_T           l_CurrStr;
  LB_Word_T             l_HashKey;

  //--* Calculate HashKey from pa_String, which is the key
  l_HashKey = pa_KeyString.HashKey();
  l_HashKey = l_HashKey % HASHLIST_TABLE_SIZE;

  //--* If there are no entries at hash index, search is over
  if (m_HashTable[l_HashKey] == NULL)
    return (LB_FALSE);

  //--* Walk list at hashtable[hashkey] looking for match
  l_NodePtr = m_HashTable[l_HashKey];
  l_PrevPtr = NULL;

  while (l_NodePtr != NULL)
    {
    if (pa_KeyLength == 0)
      l_CurrStr = l_NodePtr->StringData.SubstrMid(pa_KeyOffset,l_NodePtr->StringData.Size());
    else
      l_CurrStr = l_NodePtr->StringData.SubstrMid(pa_KeyOffset,pa_KeyLength);

    if (l_CurrStr == pa_KeyString)
      {
      //--* Assign return value
      pa_Value = l_NodePtr->StringData;

      //--* Adjust list
      if (l_PrevPtr != NULL)
        {
        l_PrevPtr->NextPtr     = l_NodePtr->NextPtr;
        l_NodePtr->NextPtr     = m_HashTable[l_HashKey];
        m_HashTable[l_HashKey] = l_NodePtr;
        }

      return (LB_TRUE);
      }

    l_PrevPtr = l_NodePtr;
    l_NodePtr = l_NodePtr->NextPtr;
    }

  return (LB_FALSE);
  }


/*---------------------------------------------------------------
  ---                   Public  Methods                       ---
  ---------------------------------------------------------------*/
/*
 *
**/
void                    LB_HashList_T::DiagDisplay()
  {
  struct HASHLIST_NODE_T *l_NodePtr;
  LB_Word_T             l_LineCnt;
  LB_Word_T             l_Index;

  printf("\nCurrent list contents:");

  for (l_Index=0; l_Index < HASHLIST_TABLE_SIZE; l_Index++)
    {
    if (m_HashTable[l_Index] != NULL)
      {
      l_NodePtr = m_HashTable[l_Index];

      printf("\nTable entry[%d]",l_Index); l_LineCnt++;

      while (l_NodePtr != NULL)
        {
        printf("\n\t");
        l_NodePtr->StringData.Display();
        l_NodePtr = l_NodePtr->NextPtr;
        l_LineCnt++;
        if (l_LineCnt > 20)
          {
          l_LineCnt = 0;
          printf("\nPress a key for next page.");
          if (getchar() == 'x') exit(0);
          }
        }
      }
    }

  printf("\nListing complete. Press a key");
  if (getchar() == 'x') exit(0);
  }


/*
 *
**/
LB_HashList_T::LB_HashList_T()
  {
  LB_Word_T l_Index;

  for (l_Index=0; l_Index < HASHLIST_TABLE_SIZE; l_Index++)
    m_HashTable[l_Index] = NULL;
  }


/*
 *
**/
LB_HashList_T::~LB_HashList_T()
  {
  Clear();
  }


/*
 *
**/
LB_Bool_T               LB_HashList_T::Add(LB_String_T& pa_String,
                                           LB_Word_T    pa_KeyOffset,
                                           LB_Word_T    pa_KeyLength)
  {
  struct HASHLIST_NODE_T *l_NodePtr;
  LB_Word_T             l_HashKey;
  LB_String_T           l_KeyStr;

  //--* Create new node
  l_NodePtr = new (struct HASHLIST_NODE_T);
  if (l_NodePtr == NULL)
    return (LB_FALSE);
  l_NodePtr->NextPtr    = NULL;
  l_NodePtr->StringData = pa_String;

  //--* Form key string
  if (pa_KeyLength == 0)
    l_KeyStr = pa_String.SubstrMid(pa_KeyOffset,pa_String.Size());
  else
    l_KeyStr = pa_String.SubstrMid(pa_KeyOffset,pa_KeyLength);

  //--* Calculate HashKey from KeyString
  l_HashKey = l_KeyStr.HashKey();
  l_HashKey = l_HashKey % HASHLIST_TABLE_SIZE;

  //--* Insert at head of table[hash]
  l_NodePtr->NextPtr = m_HashTable[l_HashKey];
  m_HashTable[l_HashKey] = l_NodePtr;

  return (LB_TRUE);
  }


/*
 *
**/
void                    LB_HashList_T::Clear()
  {
  struct HASHLIST_NODE_T *l_NodePtr;
  struct HASHLIST_NODE_T *l_NextPtr;
  LB_Word_T             l_Index;

  for (l_Index=0; l_Index < HASHLIST_TABLE_SIZE; l_Index++)
    {
    l_NodePtr = m_HashTable[l_Index];

    while (l_NodePtr != NULL)
      {
      l_NextPtr = l_NodePtr->NextPtr;
      l_NodePtr->StringData.Clear();
      delete l_NodePtr;
      l_NodePtr = l_NextPtr;
      }

    m_HashTable[l_Index] = NULL;
    }
  }


/*
 *
 * Zero passed in for keylength means to use full length of
 * string in the list to compare against. Otherwise, only
 * keylength bytes are compared.
 *
**/
LB_Bool_T               LB_HashList_T::Exists(LB_String_T& pa_KeyString,
                                              LB_Word_T    pa_KeyOffset,
                                              LB_Word_T    pa_KeyLength)
  {
  LB_String_T  l_Value;

  return (m_Search(pa_KeyString,pa_KeyOffset,pa_KeyLength,l_Value));
  }


/*
 *
**/
LB_Bool_T               LB_HashList_T::Retrieve(LB_String_T& pa_KeyString,
                                                LB_Word_T    pa_KeyOffset,
                                                LB_Word_T    pa_KeyLength,
                                                LB_String_T& pa_Value)
  {
  return (m_Search(pa_KeyString,pa_KeyOffset,pa_KeyLength,pa_Value));
  }
