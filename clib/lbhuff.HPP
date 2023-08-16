#ifndef  LB_HUFFMAN_HPP
#define  LB_HUFFMAN_HPP

#include <lbport.h>
#include <lbfio.hpp>
#include <lbstring.hpp>

class LB_Huffman_T
  {
  public:
    LB_Huffman_T();

    ~LB_Huffman_T();

    LB_Bool_T           Compress(LB_String_T& pa_InpFileName,
                                 LB_String_T& pa_OutFileName);

    LB_Bool_T           Decompress(LB_String_T& pa_InpFileName,
                                   LB_String_T& pa_OutFileName);

    void                GetErrorMessage(LB_String_T& pa_ErrMsg);

  private:
#define LBHUFF_MAXSYMBOLS 256
#define LBHUFF_MAXNODES   (LBHUFF_MAXSYMBOLS * 2 - 1)
    LB_Word_T           m_DIRLEFT;
    LB_Word_T           m_DIRRIGHT;
    LB_Byte_T           m_SIGNATURE[6];
    LB_DWord_T          m_BytesInp;
    LB_DWord_T          m_BytesOut;
    int                 m_TreeRoot;
    int                 m_NodeCount;
    LB_Byte_T *         m_HuffTable;
    LB_Byte_T *         m_HuffTablePtr;
    LB_Word_T           m_HuffBytes;
    LB_Fio_T            m_InpFile;
    LB_Fio_T            m_OutFile;
    LB_String_T         m_ErrMsg;

    struct HEADER_T
      {
      int  Index;
      int  Child[2];
      } m_Header;

    struct HUFFTREE_T
      {
      LB_DWord_T  Count;
      int         Child[2];
      int         Parent;
      LB_Byte_T * HCode;
      int         BitCount;
      } m_HuffTree[LBHUFF_MAXNODES];

    LB_Bool_T           m_FileScan(void);
    LB_Bool_T           m_TreeBuild(void);
    void                m_TreeWalk(int pa_Node, int pa_Depth);
    LB_Bool_T           m_Compress(void);
    LB_Bool_T           m_Decode(void);
    LB_Bool_T           m_Encode(void);
  };
#endif
