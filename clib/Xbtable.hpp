#ifndef  XB_TABLE_HPP
#define  XB_TABLE_HPP

#include <lbport.h>
#include <xbcolumn.hpp>
#include <lbfio.hpp>
#include <lbstring.hpp>
#include <xbtable.hpp>

#include <dblib.h>

//--- Table Limits
#define XBTBL_MAX_COLUMNS         255
#define XBTBL_MAX_ROWSIZE         4000
#define XBTBL_MAX_IDXTAGS         255

class XB_Table_T
{
    public:
        XB_Table_T();

        ~XB_Table_T();

        void            Clear();

        LB_Word_T       ColumnDefine(LB_CZString_T pa_Name,
                                     LB_Word_T     pa_Size,
                                     LB_Word_T     pa_Flags);

        LB_Bool_T       GetColumnValue(LB_Word_T    pa_ColID,   LB_String_T& pa_String);
        LB_Bool_T       GetColumnValue(LB_String_T& pa_ColName, LB_String_T& pa_String);

        LB_Bool_T       SetColumnValue(LB_Word_T    pa_ColID, LB_String_T& pa_String);
        LB_Bool_T       SetColumnValue(LB_String_T& pa_Name,  LB_String_T& pa_String);

        LB_Bool_T       Open();
        LB_Bool_T       Create();
        LB_Bool_T       Remove();
        void            Close();

        LB_Bool_T       RowAdd();
        LB_Bool_T       RowDelete();
        LB_Bool_T       RowModify();
        LB_Bool_T       RowMoveNext();
        LB_Bool_T       RowMovePrevious();
        LB_Bool_T       RowMoveBottom();
        LB_Bool_T       RowMoveTop();
        LB_Bool_T       RowLock();
        void            RowUnlock();

        LB_CZString_T   GetLastMessage();

        LB_Bool_T       IsTableAtBottom();
        LB_Bool_T       IsTableAtTop();
        LB_Bool_T       IsRowDeleted();

        LB_Bool_T       SetTableName(LB_String_T& pa_TableName);

        LB_Bool_T       TableLock();
        void            TableUnlock();

    private:
        static LB_Word_T    InstanceCnt;

        LB_Bool_T       m_ClassActive;
        LB_Bool_T       m_DataOpenFlag;
        LB_Bool_T       m_IndexOpenFlag;
        LB_Word_T       m_NumColumns;
        LB_Word_T       m_NumIdxTags;
        LB_String_T     m_TableName;
        XB_Column_T     m_Columns[XBTBL_MAX_COLUMNS];
        DBFIELD         m_DbfFields[XBTBL_MAX_COLUMNS];
        long            m_RowNumber;
        int             m_DataHandle;
        int             m_IndexHandle;
        int             m_DbfStyle;
        char            m_LastMsg[256];
        long            m_IdxTagHandles[XBTBL_MAX_IDXTAGS];

        LB_Bool_T       m_ReadColumnData();
        LB_Bool_T       m_WriteColumnData();
  };

#endif
