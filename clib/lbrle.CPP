/*  lbrle.cpp
 *
 *  Run Length Encoding Compression routines
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
 *      12/08/96        LR      -Initial Release
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
#include <memory.h>
#include <stdio.h>
#include <lbport.h>    //--- Portability Definitions
#include <lbrle.hpp>   //--- Class Definition


//---------------------------------------------------------------
//---                   Public  Methods                       ---
//---------------------------------------------------------------
LB_RLE_T::LB_RLE_T()
{
  //--- Initialize Constants
  m_SENTINEL  = 0xF0;
  m_SIGNATURE = "RLEA";

  //--- Initialize variables
  m_BytesInp = 0;
  m_BytesOut = 0;
}


LB_RLE_T::~LB_RLE_T()
{
  m_InpFile.Close();
  m_OutFile.Close();
}


LB_Bool_T               LB_RLE_T::Compress(LB_String_T& pa_InpFileName,
                                           LB_String_T& pa_OutFileName)
{
  LB_String_T IOBufferInp;
  LB_String_T IOBufferOut;
  LB_Byte_T   InpByte;
  LB_Byte_T   PrevByte;
  LB_Byte_T   Count;
  LB_Bool_T   FirstTimeFlag = LB_TRUE;

  m_InpFile.SetFileName(pa_InpFileName);
  m_OutFile.SetFileName(pa_OutFileName);

  //--- Open input file
  if (m_InpFile.Exists() == LB_FALSE)
    {
    m_ErrMsg = "Input file does not exist";
    return (LB_FALSE);
    }
  if (m_InpFile.Open(FIO_OM_RO_BUF) == LB_FALSE)
    {
    m_ErrMsg = "Input file could not be opened";
    return (LB_FALSE);
    }


  //--- Create and Open output file
  if (m_OutFile.Create() == LB_FALSE)
    {
    m_ErrMsg = "Output file could not be created";
    m_InpFile.Close();
    return (LB_FALSE);
    }

  if (m_OutFile.Open(FIO_OM_WO_BUF) == LB_FALSE)
    {
    m_ErrMsg = "Output file could not be opened";
    m_InpFile.Close();
    return (LB_FALSE);
    }

  //--- Write signature
  IOBufferOut = m_SIGNATURE;
  if (m_OutFile.Write(IOBufferOut) != IOBufferOut.Size())
    {
    m_ErrMsg = "Could not write signature";
    m_InpFile.Close();
    m_OutFile.Close();
    return (LB_FALSE);
    }

  //--- Write sentinel
  IOBufferOut.Clear();
  IOBufferOut.Append(m_SENTINEL);
  if (m_OutFile.Write(IOBufferOut) != IOBufferOut.Size())
    {
    m_ErrMsg = "Could not write sentinel";
    m_InpFile.Close();
    m_OutFile.Close();
    return (LB_FALSE);
    }

  //--- Compress input file to output file
  IOBufferInp.Clear();
  IOBufferInp.PadRight(1,0);
  while (LB_TRUE)
    {
    if (m_InpFile.Read(IOBufferInp) == 0)
      break;

    InpByte = IOBufferInp[0];

    if (FirstTimeFlag == LB_TRUE)
      {
      PrevByte      = InpByte;
      Count         = 1;
      FirstTimeFlag = LB_FALSE;
      continue;
      }

    //---Same char; add it to total
    if (InpByte == PrevByte)
      {
      Count++;
      if (Count == 255)
        {
        IOBufferOut.Clear();
        IOBufferOut.Append(m_SENTINEL);
        IOBufferOut.Append((LB_Byte_T) Count);
        IOBufferOut.Append(PrevByte);
        if (m_OutFile.Write(IOBufferOut) != IOBufferOut.Size())
          {
          m_ErrMsg = "Could not write output file data";
          m_InpFile.Close();
          m_OutFile.Close();
          return (LB_FALSE);
          }
        Count = 0;
        }
      continue;
      }

    //--- New char; write out old char
    if (Count < 3)
      {
      if (PrevByte == m_SENTINEL)
        {
        IOBufferOut.Clear();
        IOBufferOut.Append(m_SENTINEL);
        IOBufferOut.Append((LB_Byte_T) Count);
        if (m_OutFile.Write(IOBufferOut) != IOBufferOut.Size())
          {
          m_ErrMsg = "Could not write output file data";
          m_InpFile.Close();
          m_OutFile.Close();
          return (LB_FALSE);
          }
        }
      else
        {
        IOBufferOut.Clear();
        IOBufferOut.PadRight(Count, PrevByte);
        if (m_OutFile.Write(IOBufferOut) != IOBufferOut.Size())
          {
          m_ErrMsg = "Could not write output file data";
          m_InpFile.Close();
          m_OutFile.Close();
          return (LB_FALSE);
          }
        }
      }
    else
      {
      IOBufferOut.Clear();
      IOBufferOut.Append(m_SENTINEL);
      IOBufferOut.Append((LB_Byte_T) Count);
      IOBufferOut.Append((LB_Byte_T) PrevByte);
      if (m_OutFile.Write(IOBufferOut) != IOBufferOut.Size())
        {
        m_ErrMsg = "Could not write output file data";
        m_InpFile.Close();
        m_OutFile.Close();
        return (LB_FALSE);
        }
      }

    PrevByte = InpByte;
    Count    = 1;
    }

  if (Count < 3)
    {
    if (PrevByte == m_SENTINEL)
      {
      IOBufferOut.Clear();
      IOBufferOut.Append(m_SENTINEL);
      IOBufferOut.Append((LB_Byte_T) Count);
      if (m_OutFile.Write(IOBufferOut) != IOBufferOut.Size())
        {
        m_ErrMsg = "Could not write output file data";
        m_InpFile.Close();
        m_OutFile.Close();
        return (LB_FALSE);
        }
      }
    else
      {
      IOBufferOut.Clear();
      IOBufferOut.PadRight(Count, PrevByte);
      if (m_OutFile.Write(IOBufferOut) != IOBufferOut.Size())
        {
        m_ErrMsg = "Could not write output file data";
        m_InpFile.Close();
        m_OutFile.Close();
        return (LB_FALSE);
        }
      }
    }
  else
    {
    IOBufferOut.Clear();
    IOBufferOut.Append(m_SENTINEL);
    IOBufferOut.Append((LB_Byte_T) Count);
    IOBufferOut.Append((LB_Byte_T) PrevByte);
    if (m_OutFile.Write(IOBufferOut) != IOBufferOut.Size())
      {
      m_ErrMsg = "Could not write output file data";
      m_InpFile.Close();
      m_OutFile.Close();
      return (LB_FALSE);
      }
    }

  m_InpFile.Close();
  m_OutFile.Close();

  return (LB_TRUE);
}


LB_Bool_T               LB_RLE_T::Decompress(LB_String_T& pa_InpFileName,
                                             LB_String_T& pa_OutFileName)
{
  LB_String_T IOBufferInp;
  LB_String_T IOBufferOut;
  LB_Byte_T   InpByte;
  LB_Byte_T   Count;
  LB_Byte_T   Sentinel;

  m_InpFile.SetFileName(pa_InpFileName);
  m_OutFile.SetFileName(pa_OutFileName);

  //--- Open input file
  if (m_InpFile.Exists() == LB_FALSE)
    {
    m_ErrMsg = "Input file does not exist";
    return (LB_FALSE);
    }

  if (m_InpFile.Open(FIO_OM_RO_BUF) == LB_FALSE)
    {
    m_ErrMsg = "Could not open input file";
    return (LB_FALSE);
    }

  //--- Create and open output file
  if (m_OutFile.Create() == LB_FALSE)
    {
    m_ErrMsg = "Could not create output file";
    m_InpFile.Close();
    return (LB_FALSE);
    }

  if (m_OutFile.Open(FIO_OM_WO_BUF) == LB_FALSE)
    {
    m_ErrMsg = "Could not open output file";
    m_InpFile.Close();
    return (LB_FALSE);
    }

  //--- Verify signature
  IOBufferInp.PadRight(m_SIGNATURE.Size(), 0);
  if (m_InpFile.Read(IOBufferInp) != IOBufferInp.Size())
    {
    m_ErrMsg = "Could not read input file signature";
    m_InpFile.Close();
    m_OutFile.Close();
    return (LB_FALSE);
    }
  if (IOBufferInp != m_SIGNATURE)
    {
    m_ErrMsg = "Input file has an invalid signature";
    m_InpFile.Close();
    m_OutFile.Close();
    return (LB_FALSE);
    }

  //--- Read sentinel
  IOBufferInp.Clear();
  IOBufferInp.PadRight(1,0);
  if (m_InpFile.Read(IOBufferInp) != IOBufferInp.Size())
    {
    m_ErrMsg = "Could not read input file sentinel value";
    m_InpFile.Close();
    m_OutFile.Close();
    return (LB_FALSE);
    }
  Sentinel = IOBufferInp[0];

  IOBufferInp.Clear();
  IOBufferInp.PadRight(1,0);
  while (LB_TRUE)
    {
    if (m_InpFile.Read(IOBufferInp) == 0)
      break;

    InpByte = IOBufferInp[0];

    IOBufferOut.Clear();
    if (InpByte != Sentinel)
      {
      IOBufferOut.Append(InpByte);
      if (m_OutFile.Write(IOBufferOut) != IOBufferOut.Size())
        {
        m_ErrMsg = "Could not write output file";
        m_InpFile.Close();
        m_OutFile.Close();
        return (LB_FALSE);
        }
      }
    else
      {
      //--- Read count field
      if (m_InpFile.Read(IOBufferInp) == 0)
        {
        m_ErrMsg = "Could not read input file";
        m_InpFile.Close();
        m_OutFile.Close();
        return (LB_FALSE);
        }
      Count = IOBufferInp[0];

      switch(Count)
        {
        //--- Error in stream
        case 0:
          m_ErrMsg = "Input file has encoded count of zero";
          m_InpFile.Close();
          m_OutFile.Close();
          return (LB_FALSE);

        case 1:
          IOBufferOut.Append(InpByte);
          if (m_OutFile.Write(IOBufferOut) != IOBufferOut.Size())
            {
            m_ErrMsg = "Could not write output file";
            m_InpFile.Close();
            m_OutFile.Close();
            return (LB_FALSE);
            }
          break;

        case 2:
          IOBufferOut.Append(Sentinel);
          IOBufferOut.Append(Sentinel);
          if (m_OutFile.Write(IOBufferOut) != IOBufferOut.Size())
            {
            m_ErrMsg = "Could not write output file";
            m_InpFile.Close();
            m_OutFile.Close();
            return (LB_FALSE);
            }
          break;

        default:
          //--- Read count field
          if (m_InpFile.Read(IOBufferInp) == 0)
            {
            m_ErrMsg = "Could not read count field";
            m_InpFile.Close();
            m_OutFile.Close();
            return (LB_FALSE);
            }

          InpByte = IOBufferInp[0];
          IOBufferOut.PadRight(Count, InpByte);
          if (m_OutFile.Write(IOBufferOut) != IOBufferOut.Size())
            {
            m_ErrMsg = "Could not write output file";
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


void                    LB_RLE_T::GetErrorMessage(LB_String_T& pa_ErrMsg)
{
  pa_ErrMsg = m_ErrMsg;
}
