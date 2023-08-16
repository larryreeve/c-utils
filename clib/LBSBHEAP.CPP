/*  lbsbheap.cpp
 *
 *  Sorted Binary Heap routines
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
 *      12/01/95        LR      Initial Release
 *
**/


/*---------------------------------------------------------------
  ---           Notes                       t                 ---
  ---------------------------------------------------------------*/
/*
 *      Based on the article "The Binary Heap"  by Nicholas Wilt,
 *      PC Techniques, June/July 1991,pp 46-50
 *
**/


/*---------------------------------------------------------------
  ---                INCLUDE FILES                            ---
  ---------------------------------------------------------------*/
#include <alloc.h>
#include <stdio.h>
#include <memory.h>
#include <lbport.h>
#include <lbsbheap.hpp>


/*---------------------------------------------------------------
  ---                   Module Constants                      ---
  ---------------------------------------------------------------*/



/*---------------------------------------------------------------
  ---                   Private Methods                       ---
  ---------------------------------------------------------------*/
/*
 *
**/
void LB_SBHeap_T::m_SiftUp()
  {
  int i = m_n - 1;

  while (i)
    {
    int p = (i-1) >> 1;
    void *temp;
    if ((*m_comp)(m_elms[p],m_elms[i]) <= 0)
      break;
    temp        = m_elms[i];
    m_elms[i]   = m_elms[p];
    m_elms[p]   = temp;
    i           = p;
    }
  }

/*
 *
**/
void LB_SBHeap_T::m_SiftDown()
  {
  int i=0;
  int c;

  while ((c = i+i+1) < m_n)
    {
    void *temp;
    if (c+1 < m_n)
      c += ((*m_comp)(m_elms[c+1], m_elms[c]) < 0);
    if ((*m_comp)(m_elms[i], m_elms[c]) <= 0)
      break;
    temp        = m_elms[i];
    m_elms[i]   = m_elms[c];
    m_elms[c]   = temp;
    i           = c;
    }
  }


/*---------------------------------------------------------------
  ---                   Public  Methods                       ---
  ---------------------------------------------------------------*/


/*
 * Class constructor
**/
LB_SBHeap_T::LB_SBHeap_T(int (*ComparisonFunction)(void *, void *))
  {
  // Allocate pointers for 10 elements
  m_elms        = (void **) malloc(10 * sizeof(void *));
  m_n           = 0;
  m_maxsize     = 10;
  m_comp        = ComparisonFunction;
  }


/*
 * Class destructor
**/
LB_SBHeap_T::~LB_SBHeap_T()
  {
  free (m_elms);
  }


/*
 *
**/
void *LB_SBHeap_T::Extract()
  {
  void *ret;

  if (!m_n)
    return ((void *) 0);

  ret = m_elms[0];
  m_elms[0] = m_elms[--m_n];
  m_SiftDown();
  return (ret);
  }


/*
 *
**/
void LB_SBHeap_T::Insert(void  *ptr)
  {
  if (++m_n >= m_maxsize)
    {
    m_maxsize <<= 1;
    m_elms = (void **) realloc(m_elms,m_maxsize * sizeof(void *));
    }

  m_elms[m_n-1] = ptr;
  m_SiftUp();
  }

