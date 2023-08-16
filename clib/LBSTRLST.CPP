/*  lbstrlst.cpp
 *
 *  String List Processing routines
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
 *      03/02/96        LR      -Initial Release
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
#include <lbstrlst.hpp>


/*---------------------------------------------------------------
  ---                   Private Methods                       ---
  ---------------------------------------------------------------*/
/*
 *
 * Zero passed in for keylength means to use full length of
 * string in the list to compare against. Otherwise, only
 * keylength bytes are compared.
 *
**/
LB_Bool_T               LB_StringList_T::m_Search(LB_String_T& pa_KeyString,
                                                  LB_Word_T    pa_KeyOffset,
                                                  LB_Word_T    pa_KeyLength,
                                                  LB_String_T& pa_Value)
  {
  struct STRLIST_NODE_T *l_PrevPtr;
  struct STRLIST_NODE_T *l_CurrPtr;
  LB_String_T           l_CurrStr;
  LB_Bool_T             l_FoundFlag;

  l_FoundFlag = LB_FALSE;

  l_PrevPtr = NULL;
  l_CurrPtr = m_HeadPtr;

  while ((l_CurrPtr != NULL) && (l_FoundFlag == LB_FALSE))
    {
    if (pa_KeyLength == 0)
      l_CurrStr = l_CurrPtr->StringData.SubstrMid(pa_KeyOffset,l_CurrPtr->StringData.Size());
    else
      l_CurrStr = l_CurrPtr->StringData.SubstrMid(pa_KeyOffset,pa_KeyLength);
    if (pa_KeyString == l_CurrStr)
      l_FoundFlag = LB_TRUE;
    else
      {
      l_PrevPtr = l_CurrPtr;
      l_CurrPtr = l_CurrPtr->NextPtr;
      }
    }

  //--* Adjust list
  if (l_FoundFlag == LB_TRUE)
    {
    pa_Value = l_CurrPtr->StringData;
    if ((m_SortedFlag == LB_FALSE) && (l_PrevPtr != NULL))
      {
      l_PrevPtr->NextPtr = l_CurrPtr->NextPtr;
      l_CurrPtr->NextPtr = m_HeadPtr;
      m_HeadPtr = l_CurrPtr;
      }
    }

  return (l_FoundFlag);
  }


/*---------------------------------------------------------------
  ---                   Public  Methods                       ---
  ---------------------------------------------------------------*/
/*
 *
**/
void                    LB_StringList_T::DiagDisplay()
  {
  struct STRLIST_NODE_T *l_CurrPtr;
  LB_Word_T             l_LineCnt;

  printf("\nCurrent list contents:");
  l_CurrPtr = m_HeadPtr;
  l_LineCnt = 0;
  while (l_CurrPtr != NULL)
    {
    printf("\nList Data->"); l_CurrPtr->StringData.Display();
    l_CurrPtr = l_CurrPtr->NextPtr;
    l_LineCnt++;
    if (l_LineCnt > 20)
      {
      l_LineCnt = 0;
      printf("\nPress a key for next page.");
      if (getchar() == 'x') exit(0);
      }
    }
  printf("\nListing complete. Press a key");
  if (getchar() == 'x') exit(0);
  }


/*
 *
**/
LB_StringList_T::LB_StringList_T()
  {
  m_HeadPtr    = NULL;
  m_GetPtr     = NULL;
  m_SortedFlag = LB_TRUE;
  }


/*
 *
**/
LB_StringList_T::~LB_StringList_T()
  {
  Clear();
  }


/*
 *
**/
LB_Bool_T               LB_StringList_T::Add(LB_String_T& pa_String,
                                             LB_Word_T    pa_KeyOffset,
                                             LB_Word_T    pa_KeyLength)
  {
  struct STRLIST_NODE_T *l_CurrPtr;
  struct STRLIST_NODE_T *l_PrevPtr;
  struct STRLIST_NODE_T *l_NodePtr;
  LB_String_T           l_InsertString;
  LB_String_T           l_ListString;

  //--* Create new node
  l_NodePtr = new (struct STRLIST_NODE_T);
  if (l_NodePtr == NULL)
    return (LB_FALSE);
  l_NodePtr->NextPtr    = NULL;
  l_NodePtr->StringData = pa_String;

  //--* If unsorted, insert at head
  if (m_SortedFlag == LB_FALSE)
    {
    l_NodePtr->NextPtr = m_HeadPtr;
    m_HeadPtr = l_NodePtr;
    return (LB_TRUE);
    }

  //--* Find insertion point
  l_CurrPtr = m_HeadPtr;
  l_PrevPtr = NULL;
  l_InsertString = pa_String.SubstrMid(pa_KeyOffset,pa_KeyLength);
  while (l_CurrPtr != NULL)
    {
    l_ListString = l_CurrPtr->StringData.SubstrMid(pa_KeyOffset,pa_KeyLength);
    if (l_InsertString < l_ListString)
      break;
    l_PrevPtr = l_CurrPtr;
    l_CurrPtr = l_CurrPtr->NextPtr;
    }

  //--* Add new node
  l_NodePtr->NextPtr = l_CurrPtr;
  if (l_PrevPtr == NULL)
    m_HeadPtr = l_NodePtr;
  else
    l_PrevPtr->NextPtr = l_NodePtr;

  return (LB_TRUE);
  }


/*
 *
**/
LB_Bool_T               LB_StringList_T::Exists(LB_String_T& pa_KeyString,
                                                LB_Word_T    pa_KeyOffset,
                                                LB_Word_T    pa_KeyLength)
  {
  LB_String_T l_String;

  return (m_Search(pa_KeyString,pa_KeyOffset,pa_KeyLength,l_String));
  }


/*
 *
**/
LB_Bool_T               LB_StringList_T::Retrieve(LB_String_T& pa_KeyString,
                                                  LB_Word_T    pa_KeyOffset,
                                                  LB_Word_T    pa_KeyLength,
                                                  LB_String_T& pa_Value)
  {
  return (m_Search(pa_KeyString,pa_KeyOffset,pa_KeyLength,pa_Value));
  }


/*
 *
 * Allows the list to be search adjusted so that the most frequently
 * accessed item is at the top.
 *
**/
void                    LB_StringList_T::SetAsUnsorted()
  {
  m_SortedFlag = LB_FALSE;
  }


/*
 *
**/
void                    LB_StringList_T::Clear()
  {
  struct STRLIST_NODE_T *l_NodePtr;
  struct STRLIST_NODE_T *l_NextPtr;

  //--* Delete List
  l_NodePtr = m_HeadPtr;

  while (l_NodePtr != NULL)
    {
    l_NextPtr = l_NodePtr->NextPtr;
    l_NodePtr->StringData.Clear();
    delete l_NodePtr;
    l_NodePtr = l_NextPtr;
    }

  m_HeadPtr = NULL;
  m_GetPtr  = NULL;

  m_SortedFlag = LB_TRUE;
  }


/*
 *
**/
LB_Bool_T               LB_StringList_T::GetNext(LB_String_T& pa_String)
  {
  if (m_GetPtr == NULL)
    return (LB_FALSE);

  pa_String = m_GetPtr->StringData;

  m_GetPtr = m_GetPtr->NextPtr;

  return (LB_TRUE);
  }


void                    LB_StringList_T::GetReset()
  {
  m_GetPtr = m_HeadPtr;
  }
