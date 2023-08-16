#ifndef  XB_COLUMN_HPP
#define  XB_COLUMN_HPP


#include <lbport.h>
#include <lbstring.hpp>

#define XB_COLFLG_STRING        0x0001
#define XB_COLFLG_NUMERIC       0x0004
#define XB_COLFLG_TIMESTAMP     0x0008
#define XB_COLFLG_BLOB          0x0010
//#define XB_COLFLG_              0x0020
//#define XB_COLFLG_              0x0040
//#define XB_COLFLG_              0x0080
#define XB_COLFLG_SIGNED        0x0100
#define XB_COLFLG_0DEC          0x0200
#define XB_COLFLG_2DEC          0x0400
#define XB_COLFLG_4DEC          0x0800
#define XB_COLFLG_REQUIRED      0x1000
#define XB_COLFLG_INDEX         0x2000
//#define XB_COLFLG_              0x4000
//#define XB_COLFLG_              0x8000



class XB_Column_T
  {
  public:
    XB_Column_T(void);

    ~XB_Column_T(void);

    void                Clear();

    LB_Bool_T           Define(LB_String_T&  pa_Name,
                               LB_Word_T     pa_Size,
                               LB_Word_T     pa_Flags);

    LB_Bool_T           IsNull();
    LB_Bool_T           IsRequired();
    LB_Bool_T           IsIndex();

    LB_Bool_T           GetValue(LB_String_T& pa_Value);
    LB_Bool_T           SetValue(LB_String_T& pa_Value);

    void                GetName(LB_String_T& pa_Name);
    LB_Word_T           GetSize();
    LB_Word_T           GetType();


  private:
    LB_String_T         m_ColName;
    LB_String_T         m_ColData;
    LB_Word_T           m_ColSize;
    LB_Word_T           m_ColFlags;
  };

#endif
