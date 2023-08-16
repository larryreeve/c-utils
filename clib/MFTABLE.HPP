#ifndef  MF_TABLE_HPP
#define  MF_TABLE_HPP


#include <lbport.h>
#include <mfcolumn.hpp>
#include <lbfio.hpp>
#include <lbos.hpp>
#include <lbstring.hpp>
#include <lbhshlst.hpp>
#include <mftable.hpp>

//--- Column Types
#define MFTBL_MAX_COLUMNS       256
#define MFTBL_MAX_ROWSIZE       2048


class MF_Table_T
  {
  public:
    MF_Table_T();

    ~MF_Table_T();

     void               Clear();

     LB_Word_T          ColumnDefine(LB_CZString_T pa_Name,
                                     LB_Word_T     pa_Size,
                                     LB_Word_T     pa_Flags);


     LB_Bool_T          GetColumnValue(LB_Word_T    pa_ColID,   LB_String_T& pa_String);
     LB_Bool_T          GetColumnValue(LB_String_T& pa_ColName, LB_String_T& pa_String);

     LB_Bool_T          SetColumnValue(LB_Word_T    pa_ColID, LB_String_T& pa_String);
     LB_Bool_T          SetColumnValue(LB_String_T& pa_Name,  LB_String_T& pa_String);

     LB_Bool_T          Open();
     LB_Bool_T          Create();
     void               Close();

     LB_Bool_T          RowAdd();
     LB_Bool_T          RowDelete();
     LB_Bool_T          RowModify();
     LB_Bool_T          RowMoveNext();
     LB_Bool_T          RowMovePrevious();
     LB_Bool_T          RowMoveBottom();
     LB_Bool_T          RowMoveTop();
     LB_Bool_T          RowLock();
     void               RowUnlock();

     LB_Bool_T          IsTableAtBottom();
     LB_Bool_T          IsTableAtTop();
     LB_Bool_T          IsRowDeleted();

     void               SetTableName(LB_String_T& pa_TableName);

     LB_Bool_T          TableLock();
     void               TableUnlock();


  private:
    LB_Bool_T           m_ClassActiveFlag;
    LB_Word_T           m_NumColumns;
    LB_DWord_T          m_FilePos;
    LB_Bool_T           m_RowDeleted;
    LB_Fio_T            m_DataFile;
    LB_String_T         m_TableName;
    LB_String_T         m_IOString;
    MF_Column_T         m_Columns[MFTBL_MAX_COLUMNS];
    LB_HashList_T       m_SearchList;
    LB_OS_T             m_OS;

    struct MFISAM_DATAFILE_HDR_T
      {
      LB_DWord_T        HdrLen;
      LB_Word_T         DBSeq;
      LB_Word_T         Integrity;
      LB_Byte_T         CreateStamp[14];
      LB_Byte_T         Reserved1[14];   // modifystamp[14];
      LB_Byte_T         Reserved2[2];
      LB_Byte_T         Reserved3;
      LB_Byte_T         Organization;    // 1=sequential,2=idx,3=relative
      LB_Byte_T         Reserved4;
      LB_Byte_T         DataCompress;
      LB_Byte_T         Reserved5;
      LB_Byte_T         FileFormat;      // 0=def,1=cisam,2=II,3=defidx,4=idx4
      LB_Byte_T         Reserved6[4];
      LB_Byte_T         RecordMode;      // 0=fixed,1=variable
      LB_Byte_T         Reserved7[5];
      LB_Byte_T         Reserved8[2];
      LB_Word_T         MaxRecSize;
      LB_Byte_T         Reserved9[2];
      LB_Word_T         MinRecSize;
      LB_Byte_T         Reserved10[46];
      LB_DWord_T        CreateIdxVer;
      LB_Byte_T         Reserved11[16];
      } m_DataFileHdr;

    LB_DWord_T          m_RowSize;
    void                m_GotoCurrentRow();
    LB_Bool_T           m_ReadColumns();
    LB_Bool_T           m_WriteColumns();
    LB_Word_T           m_SwapWord(LB_Word_T pa_w);
  };
#endif
