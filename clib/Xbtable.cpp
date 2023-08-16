/*  xbtable.cpp
 *
 *  X-Base Table Input/Output routines
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
 *      12/07/95        LR      -Initial Release
 *      01/04/96        LR      -Modified writecolumns to put columns into
 *                              buffer before writing, instead of writing
 *                              each column individually.
 *                              -Added Ctrl-Z eof marker on RowAdd
 *      03/22/97        LR      -Updated to match MicroFocus tables
 *      04/06/97        LR      -Updated with DB SDK library
**/


//---------------------------------------------------------------
//---                INCLUDE FILES                            ---
//---------------------------------------------------------------
#include <assert.h>
#include <mem.h>
#include <stdio.h>
#include <string.h>
#include <lbport.h>
#include <lbdate.hpp>
#include <lbdebug.hpp>
#include <lbfio.hpp>
#include <xbcolumn.hpp>
#include <xbtable.hpp>


LB_ASSERTFILE(__FILE__)


//---------------------------------------------------------------
//---                   Private Methods                       ---
//---------------------------------------------------------------
LB_Word_T XB_Table_T::InstanceCnt = 0;

LB_Bool_T XB_Table_T::m_ReadColumnData()
{
    int         Index;
    LB_String_T ColStr;
    char        FieldStr[256+1];

    //--- Clear column information
    for (Index=0; Index < m_NumColumns; Index++)
    {
        switch (m_Columns[Index].GetType())
        {
            case XB_COLFLG_STRING:
                if (DBGetFieldData(m_DataHandle, Index, FieldStr) != 0)
                    return (LB_FALSE);
                ColStr = FieldStr;
                m_Columns[Index].SetValue(ColStr);
                break;

            default:
                return (LB_FALSE);
        }
    }

    return (TRUE);
}


LB_Bool_T XB_Table_T::m_WriteColumnData()
{
    int         Index;
    LB_String_T ColStr;
    char        FieldStr[256+1];

    //--- Clear DB buffer area
    DBClearBuffer(m_DataHandle);

    //--- Clear column information
    for (Index=0; Index < m_NumColumns; Index++)
    {
        switch (m_Columns[Index].GetType())
        {
            case XB_COLFLG_STRING:
                m_Columns[Index].GetValue(ColStr);
                if (ColStr.Size() > sizeof(FieldStr)-1)
                {
                    LB_Debug_ErrorLog(__FILE__, __LINE__, "ColumnSize %d greater than %d", ColStr.Size(), sizeof(FieldStr)-1);
                    return (LB_FALSE);
                }
                ColStr.AsAsciiZ(FieldStr, sizeof(FieldStr)-1);
                if (DBPutFieldData(m_DataHandle, Index, FieldStr) != 0)
                {   
                    LB_Debug_ErrorLog(__FILE__, __LINE__, "DBPutFieldData failed, Index=%d (%s), LastErc=%d (%s)", Index, FieldStr, DBGetErrorCode(), DBGetErrorStr());
                    return (LB_FALSE);
                }
                break;

            default:
                LB_Debug_ErrorLog(__FILE__, __LINE__, "Unknown column type: %d", m_Columns[Index].GetType());
                return (LB_FALSE);
        }
    }

    return (TRUE);
}


//---------------------------------------------------------------
//---                   Public  Methods                       ---
//---------------------------------------------------------------
XB_Table_T::XB_Table_T()
{
    Clear();

    if (InstanceCnt == 0)
    {
        if (DBInitialize(20, 0) == 0)
        {
            m_ClassActive = LB_TRUE;
        }              
        else
        {
            m_ClassActive = LB_FALSE;
        }
    }
    else
        m_ClassActive = LB_TRUE;

    InstanceCnt++;
}


XB_Table_T::~XB_Table_T()
{
    LB_ASSERT(m_ClassActive == LB_TRUE);

    Clear();

    InstanceCnt--;

    if (InstanceCnt <= 0)
    {
        InstanceCnt = 0;
        DBTerminate();
    }

    m_ClassActive = LB_FALSE;
}


void XB_Table_T::Clear()
{
    int Index;

    //--- Clear column information
    for (Index=0; Index < XBTBL_MAX_COLUMNS; Index++)
        m_Columns[Index].Clear();

    //--- Clear header blocks
    memset(m_DbfFields, 0, sizeof(m_DbfFields));

    m_TableName.Clear();

    m_NumColumns  = 0;
    m_RowNumber   = 0;
    m_DataHandle  = 0;
    m_IndexHandle = 0;
    m_DbfStyle    = DBFOXPRO2;

    memset(m_LastMsg, '\0', sizeof(m_LastMsg));

    m_DataOpenFlag  = LB_FALSE;
    m_IndexOpenFlag = LB_FALSE;
}


void XB_Table_T::Close()
{
    int TagIndex;

    if (m_DataOpenFlag == LB_TRUE)
    {
        DBCloseFile(m_DataHandle);
        m_DataOpenFlag = LB_FALSE;
    }

    if (m_IndexOpenFlag == LB_TRUE)
    {
        for (TagIndex=0; TagIndex < m_NumIdxTags; TagIndex++)
            DBCloseTag(m_IdxTagHandles[TagIndex]);

        DBCloseIndex(m_IndexHandle);
        m_IndexOpenFlag = LB_FALSE;
    }
}


//--- Declare a column within a table. If valid column, return a
//--- column handle. Column handles range from 0..XBTBL_MAX_COLUMNS-1
LB_Word_T XB_Table_T::ColumnDefine(LB_CZString_T pa_Name,
                                   LB_Word_T     pa_Size,
                                   LB_Word_T     pa_Flags)
{
    LB_String_T ColName;

    LB_ASSERT(m_NumColumns < XBTBL_MAX_COLUMNS);
  
    ColName = pa_Name;
    m_Columns[m_NumColumns].Clear();
    m_Columns[m_NumColumns].Define(ColName, pa_Size, pa_Flags);

    //------ Set Field Name
    memset(&m_DbfFields[m_NumColumns], 0, sizeof(DBFIELD));
    strncpy(m_DbfFields[m_NumColumns].name, pa_Name, 10);

    //--- Set Field Type
    if (pa_Flags & XB_COLFLG_STRING)
    {
        m_DbfFields[m_NumColumns].type  = DBCHARACTERFIELD;
        m_DbfFields[m_NumColumns].len   = (int) pa_Size;
        m_DbfFields[m_NumColumns].decpl = 0;
    }

    if (pa_Flags & XB_COLFLG_INDEX)
    {
        m_DbfStyle = DBFOXPRO2 | DBPRODUCTIONINDEX;
    }

    m_NumColumns++;
  
    return (m_NumColumns - 1);
}


LB_Bool_T XB_Table_T:: GetColumnValue(LB_Word_T    pa_ColID,
                                      LB_String_T& pa_String)
{
    if (pa_ColID >= m_NumColumns)
        return (LB_FALSE);

    return (m_Columns[pa_ColID].GetValue(pa_String));
}


LB_Bool_T XB_Table_T::GetColumnValue(LB_String_T& pa_ColName,
                                     LB_String_T& pa_String)
{
    LB_String_T  ColName;

    for (int Index=0; Index < m_NumColumns; Index++)
    {
        m_Columns[Index].GetName(ColName);
        if (pa_ColName == ColName)
            break;
    }

    if (Index < m_NumColumns)
        return (m_Columns[Index].GetValue(pa_String));
    else
        return (LB_FALSE);
}


LB_Bool_T XB_Table_T::SetColumnValue(LB_Word_T    pa_ColID,
                                     LB_String_T& pa_String)
{
    if (pa_ColID >= m_NumColumns)
        return (LB_FALSE);

    return (m_Columns[pa_ColID].SetValue(pa_String));
}


LB_CZString_T   XB_Table_T::GetLastMessage()
{
    return ((LB_CZString_T) m_LastMsg);
}


LB_Bool_T XB_Table_T::SetColumnValue(LB_String_T& pa_ColName,
                                     LB_String_T& pa_String)
{
    LB_String_T  ColName;

    for (int Index=0; Index < m_NumColumns; Index++)
    {
        m_Columns[Index].GetName(ColName);
        if (pa_ColName == ColName)
            break;
    }

    if (Index < m_NumColumns)
        return (m_Columns[Index].SetValue(pa_String));
    else
        return (LB_FALSE);
}


LB_Bool_T XB_Table_T::Create()
{
    char  FileNameStr[FIO_MAX_PATH];
    int   Index;

    //--- Check for no filename
    if (m_TableName.IsEmpty() == LB_TRUE)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "TableName is empty.");
        return(LB_FALSE);
    }

    //--- Check for no columns
    if (m_NumColumns == 0)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "No columns defined");
        return (LB_FALSE);
    }

    //--- Set data filename
    m_TableName.AsAsciiZ(FileNameStr, sizeof(FileNameStr));
    strcat(FileNameStr, ".dbf");

    //--- Create empty data file
    if (DBCreateFile(FileNameStr, m_NumColumns, m_DbfFields, m_DbfStyle) != 0)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "DBCreateFile failed, %s", FileNameStr);
        return (LB_FALSE);
    }

    //---- Index creation
    for (Index=0; Index < m_NumColumns; Index++)
    {
        if (m_Columns[Index].IsIndex() == LB_TRUE)
        {
            //--- Create and Open Index File
            if (m_IndexOpenFlag == LB_FALSE)
            {
                //--- Set IndexFilename
                m_TableName.AsAsciiZ(FileNameStr, sizeof(FileNameStr));
                strcat(FileNameStr, ".cdx");

                if (DBCreateIndex(FileNameStr, 2, DBFOXPRO2 | DBCOMPOUNDINDEX | DBPRODUCTIONINDEX) != 0)
                {
                    LB_Debug_ErrorLog(__FILE__, __LINE__, "CreateIndex failed, LastErc=%d (%s)", DBGetErrorCode(), DBGetErrorStr());
                }

                if (DBOpenIndex(&m_IndexHandle, FileNameStr, DBOPENREADWRITE | DBOPENSHARED | DBOPENSYSBUFFERED | DBOPENAUTOLOCKS | DBFOXPRO2 | DBCOMPOUNDINDEX | DBPRODUCTIONINDEX) == 0)
                {
                    m_IndexOpenFlag = LB_TRUE;
                }
                else
                {
                    LB_Debug_ErrorLog(__FILE__, __LINE__, "OpenIndex failed, LastErc=%d (%s)", DBGetErrorCode(), DBGetErrorStr());
                }
            }

            //--- Create new tag
            if (m_IndexOpenFlag == LB_TRUE)
            {
                if (DBCreateTag(m_IndexHandle,
                                m_DbfFields[Index].name,
                                DBCHARACTERKEY | DBUNIQUEINDEX,
                                m_DbfFields[Index].name,
                                m_DbfFields[Index].len,
                                0,
                                NULL) != 0)
                {
                    LB_Debug_ErrorLog(__FILE__, __LINE__, "CreateTag failed, Index=%d (%s), LastErc=%d (%s)", Index, m_DbfFields[Index].name, DBGetErrorCode(), DBGetErrorStr());
                    return (LB_FALSE);
                }
            }
        }
    }

    if (m_IndexOpenFlag == LB_TRUE)
    {
        DBCloseIndex(m_IndexHandle);
        m_IndexOpenFlag = LB_FALSE;
    }

    return (LB_TRUE);
}


LB_Bool_T               XB_Table_T::IsTableAtBottom()
{
    if (m_DataOpenFlag == LB_FALSE)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "DataFile not opened");
        return (LB_FALSE);
    }

    if (DBIsEndOfFile(m_DataHandle) == DBTRUE)
        return (LB_TRUE);
    else
        return (LB_FALSE);
}


LB_Bool_T               XB_Table_T::IsTableAtTop()
{
    if (m_DataOpenFlag == LB_FALSE)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "DataFile not opened");
        return (LB_FALSE);
    }

    if (DBIsBeginningOfFile(m_DataHandle) == DBTRUE)
        return (LB_TRUE);
    else
        return (LB_FALSE);
}


LB_Bool_T               XB_Table_T::IsRowDeleted()
{
    if (m_DataOpenFlag == LB_FALSE)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "DataFile not opened");
        return (LB_FALSE);
    }

    if (DBIsRecordDeleted(m_DataHandle))
        return(LB_TRUE);
    else
        return(LB_FALSE);
}


LB_Bool_T               XB_Table_T::Open()
{
    char FileNameStr[FIO_MAX_PATH];
    int  Index;
    int  Index2;

    //--- Test if table is already open
    if (m_DataOpenFlag == LB_TRUE)
    {
        return (LB_TRUE);
    }

    //--- Check for no columns
    if (m_NumColumns == 0)
        return (LB_FALSE);

    //--- Check for no filename
    if (m_TableName.IsEmpty() == LB_TRUE)
        return(LB_FALSE);

    //--- Set data filename
    m_TableName.AsAsciiZ(FileNameStr, sizeof(FileNameStr));
    strcat(FileNameStr, ".dbf");

    //--- Open data file
    if (DBOpenFile(&m_DataHandle, FileNameStr, DBOPENREADWRITE | DBOPENSHARED | DBOPENSYSBUFFERED | DBOPENAUTOLOCKS | DBFOXPRO2) != 0)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "XBTable::Open failed, LastErc=%d (%s)", DBGetErrorCode(), DBGetErrorStr());
        return (LB_FALSE);
    }
    m_DataOpenFlag = LB_TRUE;

    //--- Open index file
    //------ See if index is required
    for (Index=0; Index < m_NumColumns; Index++)
    {
        if (m_Columns[Index].IsIndex() == LB_TRUE)
        {
            //------ Set index filename
            m_TableName.AsAsciiZ(FileNameStr, sizeof(FileNameStr));
            strcat(FileNameStr, ".cdx");
            if (DBOpenIndex(&m_IndexHandle, FileNameStr, DBOPENREADWRITE | DBOPENSHARED | DBOPENSYSBUFFERED | DBOPENAUTOLOCKS | DBFOXPRO2 | DBCOMPOUNDINDEX | DBPRODUCTIONINDEX) != 0)
            {
                LB_Debug_ErrorLog(__FILE__, __LINE__, "DBOpenIndex failed, LastErc=%d (%s)", DBGetErrorCode(), DBGetErrorStr());
                break;                    
            }

            m_IndexOpenFlag = LB_TRUE;
            //------ Open all index tags
            m_NumIdxTags = 0;
            for (Index2=0; Index2 < XBTBL_MAX_IDXTAGS; Index2++)
            {
                //------ Open Index Tag
                if (DBOpenTag(&m_IdxTagHandles[m_NumIdxTags], m_IndexHandle, Index2) == 0)
                {
                    DBSetDefaultKeyType(m_IdxTagHandles[m_NumIdxTags], DBCHARACTERKEY);
                    m_NumIdxTags++;
                }
                else
                {
                    break;
                }
            }
            break;
        }
    }
    return (LB_TRUE);
}


LB_Bool_T               XB_Table_T::Remove()
{
    LB_Fio_T    F;
    LB_String_T FileName;

    if (m_TableName.IsEmpty() == LB_TRUE)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "TableName is empty");
        return(LB_FALSE);
    }

    //--- Remove data file
    FileName = m_TableName;
    FileName = FileName + ".dbf";
    F.SetFileName(FileName);
    if (F.Exists() == LB_TRUE)
    {
        if (F.Remove() == LB_FALSE)
            return (LB_FALSE);
    }

    //--- Remove index file
    FileName = m_TableName;
    FileName = FileName + ".cdx";
    F.SetFileName(FileName);
    if (F.Exists() == LB_TRUE)
    {
        if (F.Remove() == LB_FALSE)
            return (LB_FALSE);
    }

    return (LB_TRUE);
}


LB_Bool_T               XB_Table_T::RowAdd()
{
    int         Index;
    int         TagIndex;
    LB_String_T ColData;
    LB_String_T ColName;
    char        ColDataStr[256+1];
    char        ColNameStr[16+1];
    DBTAGINFO   DBTagInfo;

    //--- Test if file is opened
    if (m_DataOpenFlag == LB_FALSE)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "DataFile not opened");
        return (LB_FALSE);
    }

    //--- Put data into record buffer
    if (m_WriteColumnData() == LB_FALSE)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "WriteColumnData failed");
        return (LB_FALSE);
    }

    //--- Write record buffer to datafile
    if (DBPutRecord(m_DataHandle, &m_RowNumber, DBAPPENDRECORD) != 0)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "DBPutRecord failed for row %ld, LastErc=%d (%s)", m_RowNumber, DBGetErrorCode(), DBGetErrorStr());
        return(LB_FALSE);
    }

    //--- Insert record key into tag
    for (Index=0; Index < m_NumColumns; Index++)
    {
        if (m_Columns[Index].IsIndex() == LB_TRUE)
        {
            m_Columns[Index].GetValue(ColData);
            if (ColData.Size() > sizeof(ColDataStr)-1)
                return (LB_FALSE);
            ColData.AsAsciiZ(ColDataStr, sizeof(ColDataStr)-1);

            //--- Find tag for this column
            for (TagIndex=0; TagIndex < m_NumIdxTags; TagIndex++)
            {
                if (DBGetTagInfo(m_IdxTagHandles[TagIndex], &DBTagInfo) != 0)
                {
                    return (LB_FALSE);
                }

                m_Columns[Index].GetName(ColName);
                ColName.AsAsciiZ(ColNameStr, sizeof(ColNameStr)-1);
                if (strcmp(DBTagInfo.tname, ColNameStr) == 0)
                {
                    break;
                }
            }

            if (TagIndex >= m_NumIdxTags)
            {
                return (LB_FALSE);
            } 

            if (DBAddKey(m_IdxTagHandles[TagIndex], ColDataStr, m_RowNumber) != 0)
                return (LB_FALSE);

        }
    }

    return (LB_TRUE);
}


LB_Bool_T               XB_Table_T::RowDelete()
{
    if (m_DataOpenFlag == LB_FALSE)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "DataFile not opened");
        return (LB_FALSE);
    }

    if (DBMarkRecordInActive(m_DataHandle, m_RowNumber) == 0)
        return (LB_TRUE);
    else
        return (LB_FALSE);
}


LB_Bool_T               XB_Table_T::RowModify()
{
    //--- Test if file is opened
    if (m_DataOpenFlag == LB_FALSE)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "DataFile not opened");
        return (LB_FALSE);
    }

    //--- Put data into record buffer
    if (m_WriteColumnData() == LB_FALSE)
        return (LB_FALSE);

    //--- Write record buffer
    if (DBPutRecord(m_DataHandle, &m_RowNumber, DBUPDATERECORD) == 0)
        return(LB_TRUE);
    else
        return(LB_FALSE);
}


LB_Bool_T               XB_Table_T::RowMoveNext()
{
    //--- Test if file is opened
    if (m_DataOpenFlag == LB_FALSE)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "DataFile not opened");
        return (LB_FALSE);
    }

    m_RowNumber++;

    if (DBGetRecord(m_DataHandle, m_RowNumber) == 0)
    {
        //--- Get data from record buffer
        if (m_ReadColumnData() == LB_FALSE)
            return (LB_FALSE);
        else
            return (LB_TRUE);
    }
    else
        return (LB_FALSE);
}


LB_Bool_T               XB_Table_T::RowMovePrevious()
{
    //--- Test if file is opened
    if (m_DataOpenFlag == LB_FALSE)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "DataFile not opened");
        return (LB_FALSE);
    }

    if ((m_RowNumber - 1) < 1)
        return (LB_FALSE);

    m_RowNumber--;

    if (DBGetRecord(m_DataHandle, m_RowNumber) == 0)
    {
        //--- Get data from record buffer
        if (m_ReadColumnData() == LB_FALSE)
            return (LB_FALSE);
        else
            return (LB_TRUE);
    }
    else
        return (LB_FALSE);
}


LB_Bool_T               XB_Table_T::RowMoveBottom()
{
    //--- Test if file is opened
    if (m_DataOpenFlag == LB_FALSE)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "DataFile not opened");
        return (LB_FALSE);
    }

    if (DBGetRecordCount(m_DataHandle, &m_RowNumber) != 0)
        return(LB_FALSE);

    if (DBGetRecord(m_DataHandle, m_RowNumber) == 0)
    {
        //--- Get data from record buffer
        if (m_ReadColumnData() == LB_FALSE)
            return (LB_FALSE);
        else
            return (LB_TRUE);
    }
    else
        return (LB_FALSE);
}


LB_Bool_T               XB_Table_T::RowMoveTop()
{
    if (m_DataOpenFlag == LB_FALSE)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "DataFile not opened");
        return (LB_FALSE);
    }

    m_RowNumber = 1;

    if (DBGetRecord(m_DataHandle, m_RowNumber) == 0)
    {
        //--- Get data from record buffer
        if (m_ReadColumnData() == LB_FALSE)
            return (LB_FALSE);
        else
            return (LB_TRUE);
    }
    else
        return (LB_FALSE);
}


LB_Bool_T               XB_Table_T::SetTableName(LB_String_T& pa_TableName)
{
    m_TableName = pa_TableName;

    return (LB_TRUE);
}


LB_Bool_T               XB_Table_T::TableLock()
{
    if (m_DataOpenFlag == LB_FALSE)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "DataFile not opened");
        return (LB_FALSE);
    }

    if (DBLockFile(m_DataHandle) == 0)
        return(LB_TRUE);
    else
        return(LB_FALSE);
}


void                    XB_Table_T::TableUnlock()
{
    if (m_DataOpenFlag == LB_FALSE)
    {
        LB_Debug_ErrorLog(__FILE__, __LINE__, "DataFile not opened");
        return;
    }

    DBUnLockAll(m_DataHandle);
}
