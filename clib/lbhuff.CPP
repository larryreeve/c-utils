/*  lbhuff.cpp
 *
 *  Huffman Compression routines
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
 *            COPYRIGHT (C) 1995-1997 UNITED COMPUTER SALES & SERVICE, INC.
 *
 *
 * Revision History:
 *
 *      Date            By      Notes
 *      --------        ---     ------------------------------------------
 *      12/07/96        LR      -Initial Release
 *
 *
 * Notes:
 *
 *
 *
**/


//---------------------------------------------------------------
//---                INCLUDE FILES                            ---
//---------------------------------------------------------------
#include <errno.h>
#include <stdio.h>
#include <ctype.h>      //--* Needed for isalpha test
#include <memory.h>     //--* Needed for memcpy in constructor
#include <lbport.h>     //--* Portability Definitions
#include <lbhuff.hpp>   //--* Class Definition


//---------------------------------------------------------------
//---                   Private Methods                       ---
//---------------------------------------------------------------
LB_Bool_T               LB_Huffman_T::m_FileScan(void)
{
  LB_String_T IOBuffer;
  LB_Byte_T   ByteRead;

  m_BytesInp = 0;

  m_InpFile.Close();

  if (m_InpFile.Exists() == LB_FALSE)
    return (LB_FALSE);

  if (m_InpFile.Open(FIO_OM_RO_BUF) == LB_FALSE)
    return (LB_FALSE);

  IOBuffer.PadRight(1,' ');
  while (m_InpFile.Read(IOBuffer) > 0)
    {
    ByteRead = IOBuffer[0];
    m_HuffTree[ByteRead].Count++;
    m_BytesInp++;
    }

  m_InpFile.Close();
  return (LB_TRUE);
}


void                    LB_Huffman_T::m_TreeWalk(int pa_Node, int pa_Depth)
{
  int Dir;
  int BitNo;
  int Set;
  int Byte;
  int Bit;
  int W;
  int Trace;

  for (Dir=0; Dir < 2; Dir++)
    {
    if (m_HuffTree[pa_Node].Child[Dir] == (-1))
      {
      if (Dir == m_DIRLEFT)
        {
        if (!m_HuffTablePtr)
           m_HuffBytes += (pa_Depth + 7) / 8;
        else
          {
          m_HuffTree[pa_Node].BitCount = pa_Depth;
          m_HuffTree[pa_Node].HCode    = m_HuffTablePtr;
          BitNo = pa_Depth - 1;

          for (Trace = pa_Node, W = m_HuffTree[pa_Node].Parent;
               W;
               Trace = W, W = m_HuffTree[W].Parent)
            {
            if (m_HuffTree[W].Child[m_DIRLEFT] == Trace)
              Set = m_DIRLEFT;
            else
              Set = m_DIRRIGHT;

            if (Set)
              {
              Byte = BitNo / 8;
              Bit  = BitNo - Byte * 8;
              m_HuffTablePtr[Byte] |= 1 << (7 - Bit);
              }
            BitNo--;
            }
          m_HuffTablePtr += (pa_Depth + 7) / 8;
          }
        }
      }
    else m_TreeWalk(m_HuffTree[pa_Node].Child[Dir], pa_Depth+1);
    }
}


LB_Bool_T               LB_Huffman_T::m_TreeBuild(void)
{
  int Idx;
  int Idx2;
  int Lo1;
  int Lo2;

  for (Idx=0; Idx < LBHUFF_MAXSYMBOLS; Idx++)
    {
    if (m_HuffTree[Idx].Count)
      m_NodeCount++;
    }

  if (m_NodeCount < 1)
    return (LB_FALSE);

  if (m_NodeCount == 1)
    {
    for (Idx=0; Idx < LBHUFF_MAXSYMBOLS; Idx++)
      {
      if (m_HuffTree[Idx].Count == 0)
        {
        m_HuffTree[Idx].Count++;
        m_NodeCount++;
        break;
        }
      }
    }

  m_TreeRoot = LBHUFF_MAXSYMBOLS + m_NodeCount - 2;

  for (Idx=LBHUFF_MAXSYMBOLS; Idx <= m_TreeRoot; Idx++)
    {
    for (Idx2=0; Idx2 < Idx; Idx2++)
      {
      if (m_HuffTree[Idx2].Count && !m_HuffTree[Idx2].Parent)
        {
        Lo1 = Idx2;
        Idx2++;
        break;
        }
      }

    for (; Idx2 < Idx; Idx2++)
      {
      if (m_HuffTree[Idx2].Count && !m_HuffTree[Idx2].Parent)
        {
        Lo2 = Idx2;
        Idx2++;
        break;
        }
      }

    for (; Idx2 < Idx; Idx2++)
      {
      if (m_HuffTree[Idx2].Count && !m_HuffTree[Idx2].Parent)
        {
        if (m_HuffTree[Idx2].Count < m_HuffTree[Lo1].Count)
          Lo1 = Idx2;
        else if (m_HuffTree[Idx2].Count < m_HuffTree[Lo2].Count)
          Lo2 = Idx2;
        }
      }

    m_HuffTree[Idx].Count = m_HuffTree[Lo1].Count +
                            m_HuffTree[Lo2].Count;
    m_HuffTree[Idx].Child [m_DIRLEFT]  = Lo1;
    m_HuffTree[Idx].Child [m_DIRRIGHT] = Lo2;
    m_HuffTree[Lo1].Parent             = Idx;
    m_HuffTree[Lo2].Parent             = Idx;
    }

  //--- Build bit codes
  m_HuffBytes     = 0;
  m_HuffTablePtr  = (LB_Byte_T *) NULL;
  m_TreeWalk(m_TreeRoot, 0);

  m_HuffTable = new LB_Byte_T[m_HuffBytes];
  if (m_HuffTable == (LB_Byte_T *) NULL)
    return (LB_FALSE);

  memset(m_HuffTable, 0, m_HuffBytes);

  m_HuffTablePtr = m_HuffTable;

  m_TreeWalk(m_TreeRoot, 0);

  return (LB_TRUE);
}


LB_Bool_T               LB_Huffman_T::m_Compress(void)
{
  LB_String_T IOBufferInp;
  LB_String_T IOBufferOut;
  LB_Word_T   Idx;
  LB_Byte_T   OutByte;
  LB_Byte_T   OutBit;
  LB_Byte_T   InpByte;

  //--- Create and Open output file
  m_OutFile.Close();

  if (m_OutFile.Create() == LB_FALSE)
    return (LB_FALSE);

  if (m_OutFile.Open(FIO_OM_WO_NOBUF) == LB_FALSE)
    return (LB_FALSE);

  //--- Write signature
  IOBufferOut.SetBytes(m_SIGNATURE, sizeof(m_SIGNATURE));
  if (m_OutFile.Write(IOBufferOut) != IOBufferOut.Size())
    {
    m_OutFile.Close();
    return (LB_FALSE);
    }

  IOBufferOut.SetBytes((LB_Byte_T *) &m_TreeRoot, sizeof(m_TreeRoot));
  if (m_OutFile.Write(IOBufferOut) != IOBufferOut.Size())
    {
    m_OutFile.Close();
    return (LB_FALSE);
    }

  //--- Write node count
  IOBufferOut.SetBytes((LB_Byte_T *) &m_NodeCount, sizeof(m_NodeCount));
  if (m_OutFile.Write(IOBufferOut) != IOBufferOut.Size())
    {
    m_OutFile.Close();
    return (LB_FALSE);
    }

  IOBufferOut.SetBytes((LB_Byte_T *) &m_BytesInp, sizeof(m_BytesInp));
  if (m_OutFile.Write(IOBufferOut) != IOBufferOut.Size())
    {
    m_OutFile.Close();
    return (LB_FALSE);
    }

  for (Idx=0; Idx <= m_TreeRoot; Idx++)
    {
    if (m_HuffTree[Idx].Count > 0)
      {
      m_Header.Index    = Idx;
      m_Header.Child[0] = m_HuffTree[Idx].Child[0];
      m_Header.Child[1] = m_HuffTree[Idx].Child[1];

      IOBufferOut.SetBytes((LB_Byte_T *) &m_Header, sizeof(m_Header));
      if (m_OutFile.Write(IOBufferOut) != IOBufferOut.Size())
        {
        m_OutFile.Close();
        return (LB_FALSE);
        }
      }
    }

  OutByte = 0;
  OutBit  = 0;

  //--- Open input file
  m_InpFile.Close();

  if (m_InpFile.Exists() == LB_FALSE)
    {
    m_OutFile.Close();
    return (LB_FALSE);
    }

  if (m_InpFile.Open(FIO_OM_RO_BUF) == LB_FALSE)
    {
    m_OutFile.Close();
    return (LB_FALSE);
    }

  IOBufferOut.Clear();  IOBufferOut.PadRight(1,0);
  IOBufferInp.Clear();  IOBufferInp.PadRight(1,0);

  while (LB_TRUE)
    {
    LB_Byte_T * SPtr;
    int k;
    int Count;
    int Byte;
    int Bit;
    int Set;

    if (m_InpFile.Read(IOBufferInp) == 0)
      break;

    InpByte = IOBufferInp[0];

    SPtr  = m_HuffTree[InpByte].HCode;
    Count = m_HuffTree[InpByte].BitCount;

    for (k=0; k < Count; k++)
      {
      Byte = k / 8;
      Bit  = k - Byte * 8;
      if (SPtr[Byte] & (1 << (7 - Bit)))
        Set = 1;
      else
        Set = 0;

      if (Set)
        OutByte |= 1 << (7 - OutBit);

      OutBit++;
      if (OutBit == 8)
        {
        IOBufferOut[0] = OutByte;
        if (m_OutFile.Write(IOBufferOut) != IOBufferOut.Size())
          {
          m_OutFile.Close();
          m_InpFile.Close();
          return (LB_FALSE);
          }
        OutByte = 0;
        OutBit  = 0;
        }
      }
    }

  if (OutBit)
    {
    IOBufferOut[0] = OutByte;
    if (m_OutFile.Write(IOBufferOut) != IOBufferOut.Size())
      {
      m_OutFile.Close();
      m_InpFile.Close();
      return (LB_FALSE);
      }
    }
  m_InpFile.Close();
  m_OutFile.Close();

  return (LB_TRUE);
}


LB_Bool_T               LB_Huffman_T::m_Encode(void)
{
  LB_Word_T Idx;

  m_TreeRoot  = 0;
  m_NodeCount = 0;

  memset(m_HuffTree, 0, sizeof(m_HuffTree));

  for (Idx=0; Idx < LBHUFF_MAXSYMBOLS; Idx++)
    {
    m_HuffTree[Idx].Count          = 0;
    m_HuffTree[Idx].Child[m_DIRLEFT]  = (-1);
    m_HuffTree[Idx].Child[m_DIRRIGHT] = (-1);
    m_HuffTree[Idx].Parent         = 0;
    m_HuffTree[Idx].HCode          = (LB_Byte_T *) NULL;
    m_HuffTree[Idx].BitCount       = 0;
    }

  if (m_FileScan() == LB_FALSE)
    return (LB_FALSE);

  if (m_TreeBuild() == LB_FALSE)
    return (LB_FALSE);

  if (m_Compress() == LB_FALSE)
    return (LB_FALSE);

  return (LB_TRUE);
}



LB_Bool_T               LB_Huffman_T::m_Decode(void)
{
  LB_String_T IOBuffer;
  LB_Byte_T   Signature[sizeof(m_SIGNATURE)];
  int         Idx;
  LB_Byte_T   InpByte;
  int         BitNo;
  int         Mask;
  int         Set;
  LB_DWord_T  BytesOut;

  memset(m_HuffTree, 0, sizeof(m_HuffTree));

  if (m_InpFile.Exists() == LB_FALSE)
    return (LB_FALSE);

  if (m_InpFile.Open(FIO_OM_RO_BUF) == LB_FALSE)
    return (LB_FALSE);

  //--- Verify signature
  IOBuffer.PadRight(sizeof(m_SIGNATURE),' ');
  if (m_InpFile.Read(IOBuffer) != sizeof(m_SIGNATURE))
    {
    m_InpFile.Close();
    return (LB_FALSE);
    }
  IOBuffer.GetBytes(Signature, sizeof(Signature));
  if (memcmp(Signature, m_SIGNATURE, sizeof(Signature)) != 0)
    {
    m_InpFile.Close();
    return (LB_FALSE);
    }

  //--- Get tree root
  IOBuffer.Clear();
  IOBuffer.PadRight(sizeof(m_TreeRoot), 0);
  if (m_InpFile.Read(IOBuffer) != IOBuffer.Size())
    {
    m_InpFile.Close();
    return (LB_FALSE);
    }
  IOBuffer.GetBytes((LB_Byte_T *) &m_TreeRoot, sizeof(m_TreeRoot));

  //--- Get node count
  IOBuffer.Clear();
  IOBuffer.PadRight(sizeof(m_NodeCount), 0);
  if (m_InpFile.Read(IOBuffer) != IOBuffer.Size())
    {
    m_InpFile.Close();
    return (LB_FALSE);
    }
  IOBuffer.GetBytes((LB_Byte_T *) &m_NodeCount, sizeof(m_NodeCount));

  //--- Get file size
  IOBuffer.Clear();
  IOBuffer.PadRight(sizeof(m_BytesInp), 0);
  if (m_InpFile.Read(IOBuffer) != IOBuffer.Size())
    {
    m_InpFile.Close();
    return (LB_FALSE);
    }
  IOBuffer.GetBytes((LB_Byte_T *) &m_BytesInp, sizeof(m_BytesInp));

  for (Idx=0; Idx < m_NodeCount + (m_TreeRoot - LBHUFF_MAXSYMBOLS + 1); Idx++)
    {
    IOBuffer.Clear();
    IOBuffer.PadRight(sizeof(m_Header), 0);
    if (m_InpFile.Read(IOBuffer) != IOBuffer.Size())
      {
      m_InpFile.Close();
      return (LB_FALSE);
      }
    IOBuffer.GetBytes((LB_Byte_T *) &m_Header, sizeof(m_Header));

    m_HuffTree[m_Header.Index].Child[0] = m_Header.Child[0];
    m_HuffTree[m_Header.Index].Child[1] = m_Header.Child[1];
    }


  //--- Create and Open output file
  m_OutFile.Close();

  if (m_OutFile.Create() == LB_FALSE)
    {
    m_InpFile.Close();
    return (LB_FALSE);
    }

  if (m_OutFile.Open(FIO_OM_WO_NOBUF) == LB_FALSE)
    {
    m_InpFile.Close();
    return (LB_FALSE);
    }

  BitNo = 8;
  for (BytesOut=0; BytesOut < m_BytesInp; BytesOut++)
    {
    Idx = m_TreeRoot;

    for (;;)
      {
      if (BitNo > 7)
        {
        IOBuffer.Clear();
        IOBuffer.PadRight(1, 0);
        if (m_InpFile.Read(IOBuffer) != IOBuffer.Size())
          {
          m_InpFile.Close();
          m_OutFile.Close();
          return (LB_FALSE);
          }
        InpByte = IOBuffer[0];
        BitNo   = 0;
        Mask    = 0x80;
        }

      if (InpByte & Mask)
        Set = 1;
      else
        Set = 0;
      BitNo++;
      Mask >>= 1;

      Idx = m_HuffTree[Idx].Child[Set];

      if (m_HuffTree[Idx].Child[0] == (-1))
        {
        IOBuffer.Clear();
        IOBuffer.Append((LB_Byte_T) Idx);
        if (m_OutFile.Write(IOBuffer) != IOBuffer.Size())
          {
          m_InpFile.Close();
          m_OutFile.Close();
          return (LB_FALSE);
          }
        break;
        }
      }
    }
  m_InpFile.Close();
  m_OutFile.Close();

  return (LB_TRUE);
}



//---------------------------------------------------------------
//---                   Public  Methods                       ---
//---------------------------------------------------------------
LB_Huffman_T::LB_Huffman_T()
{
  //--- Initialize Constants
  m_DIRLEFT  = 0;
  m_DIRRIGHT = 1;
  memcpy(m_SIGNATURE,"LBHUFF",6);

  //--- Initialize variables
  m_BytesInp     = 0;
  m_BytesOut     = 0;
  m_TreeRoot     = 0;
  m_NodeCount    = 0;
  m_HuffTable    = (LB_Byte_T *) NULL;
  m_HuffTablePtr = (LB_Byte_T *) NULL;;
  m_HuffBytes    = 0;

}


LB_Huffman_T::~LB_Huffman_T()
{
  m_InpFile.Close();
  m_OutFile.Close();

  if (m_HuffTablePtr != (LB_Byte_T *) NULL)
    {
    delete[] m_HuffTable;
    m_HuffTable    = (LB_Byte_T *) NULL;
    m_HuffTablePtr = (LB_Byte_T *) NULL;
    }
}


LB_Bool_T               LB_Huffman_T::Compress(LB_String_T& pa_InpFileName,
                                               LB_String_T& pa_OutFileName)
{
  LB_Bool_T Result;

  m_InpFile.SetFileName(pa_InpFileName);
  m_OutFile.SetFileName(pa_OutFileName);

  Result = m_Encode();

  m_InpFile.Close();
  m_OutFile.Close();

  return (Result);
}


LB_Bool_T               LB_Huffman_T::Decompress(LB_String_T& pa_InpFileName,
                                                 LB_String_T& pa_OutFileName)
{
  LB_Bool_T Result;

  m_InpFile.SetFileName(pa_InpFileName);
  m_OutFile.SetFileName(pa_OutFileName);

  Result = m_Decode();

  m_InpFile.Close();
  m_OutFile.Close();

  return (Result);
}

void                    LB_Huffman_T::GetErrorMessage(LB_String_T& pa_ErrMsg)
{
  pa_ErrMsg = m_ErrMsg;
}
