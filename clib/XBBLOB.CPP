/*  lbblob.cpp
 *
 *  X-Base Blob Input/Output routines
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
 *      01/09/96        LR      -Initial Release
**/


/*---------------------------------------------------------------
  ---                INCLUDE FILES                            ---
  ---------------------------------------------------------------*/
#include <assert.h>
#include <mem.h>
#include <stdio.h>
#include <string.h>
#include <lbport.h>
#include <lbfio.hpp>
#include <lbblob.hpp>


/*---------------------------------------------------------------
  ---                   Private Constants                     ---
  ---------------------------------------------------------------*/
#define BLOB_BLOCK_SIZE  512

/*---------------------------------------------------------------
  ---                   Private Variables                     ---
  ---------------------------------------------------------------*/
struct M_BLOB_HDR_T
  {
  LB_DWord_T  NextFreeBlock;
  LB_Word_T   Reserved1;
  LB_Word_T   BlockSize;
  LB_Byte_T   Reserved2[504];
  } m_BlobHdr;

struct M_BLOB_BLOCK_T
  {
  LB_Word_T     BlkSignature;           // 0=picture, 1=text
  LB_Word_T     BlkSize;
  LB_Byte_T     BlkData[BLOB_BLOCK_SIZE];
  } m_BlobBlock;


/*---------------------------------------------------------------
  ---                   Private Methods                       ---
  ---------------------------------------------------------------*/


/*---------------------------------------------------------------
  ---                   Public  Methods                       ---
  ---------------------------------------------------------------*/
/*
 *
**/
LB_Blob_T::LB_Blob_T()
  {
  Clear();
  }


/*
 *
**/
LB_Blob_T::~LB_Blob_T()
  {
  Clear();
  }


/*
 *
**/
void            	LB_Blob_T::Clear()
  {
  //--* Clear header blocks
  memset(&m_BlobHdr,   0, sizeof(m_BlobHdr));
  memset(&m_BlobBlock, 0, sizeof(m_BlobBlock));

  //--* Close Low-level file
  m_BlobFile.Close();
  }


/*
 *
**/
void                    LB_Blob_T::Close()
  {
  Clear();
  }


/*
 *
**/
LB_Bool_T         	LB_Blob_T::Create()
  {
  int           l_index;
  char          l_FileName[128];

  m_BlobFile.Close();

  //--* Get Filename
  if (m_FileName.Size() > sizeof(l_FileName))
    return (LB_FALSE);
  m_FileName.CopyToAsciiZ(l_FileName,sizeof(l_FileName));
  m_BlobFile.SetFileName(l_FileName);

  //--* Low-level create file
  if (m_BlobFile.Create() == LB_FALSE)
    return (LB_FALSE);

  //--* Open newly created file
  if (m_BlobFile.Open(FIO_OM_RW_BUF) == LB_FALSE)
    return (LB_FALSE);

  //--* Establish new header
  memset(&m_BlobHdr, 0, sizeof(m_BlobHdr));
  m_BlobHdr.NextFreeBlock = 0xFFFF;
  m_BlobHdr.BlockSize     = BLOB_BLOCK_SIZE;
  m_BlobFile.Write((LB_Byte_T *) &m_BlobHdr,sizeof(m_BlobHdr));

  return (LB_TRUE);
  }


/*
 *
**/
LB_Bool_T         	LB_Blob_T::Open()
  {
  char          l_FileName[128];
  LB_Word_T     l_CalcRowSize;
  LB_Word_T     l_Index;

  m_BlobFile.Close();

  //--* Get Filename
  if (m_FileName.Size() > sizeof(l_FileName))
    return (LB_FALSE);
  m_FileName.CopyToAsciiZ(l_FileName,sizeof(l_FileName));
  m_BlobFile.SetFileName(l_FileName);

  //--* Low-level open file
  if (m_BlobFile.Open(FIO_OM_RW_BUF) == LB_FALSE)
    return (LB_FALSE);

  //--* Read Header
  m_BlobFile.Read((LB_Byte_T *) &m_BlobHdr,sizeof(m_BlobHdr));

  //--* Check if block sizes match
  if (m_BlobHdr.BlockSize != BLOB_BLOCK_SIZE)
    {
    assert(m_BlobHdr.BlockSize == BLOB_BLOCK_SIZE);
    m_BlobFile.Close();
    return (LB_FALSE);
    }

  return (LB_TRUE);
  }


/*
 *
**/
void                    LB_Blob_T::SetFileName(LB_CZString_T pa_Filename)
  {
  if (pa_Filename != NULL)
    m_FileName.AssignBytes((LB_ZString_T) pa_Filename,strlen(pa_Filename));
  }
