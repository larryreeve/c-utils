#ifndef  MF_COLUMN_HPP
#define  MF_COLUMN_HPP


#include <lbport.h>
#include <lbcblcmp.hpp>
#include <lbstring.hpp>

#define MF_COLFLG_STRING        0x0001
#define MF_COLFLG_COMP          0x0002
//#define MF_COLFLG_              0x0004
//#define MF_COLFLG_              0x0008
//#define MF_COLFLG_              0x0010
//#define MF_COLFLG_              0x0020
//#define MF_COLFLG_              0x0040
//#define MF_COLFLG_              0x0080
#define MF_COLFLG_SIGNED        0x0100
#define MF_COLFLG_0DEC          0x0200
#define MF_COLFLG_2DEC          0x0400
#define MF_COLFLG_4DEC          0x0800
#define MF_COLFLG_REQUIRED      0x1000
//#define MF_COLFLG_              0x2000
//#define MF_COLFLG_              0x4000
//#define MF_COLFLG_              0x8000


class MF_Column_T
  {
  public:
    MF_Column_T(void);

    ~MF_Column_T(void);

    void                Clear();

    LB_Bool_T           Define(LB_String_T&  pa_Name,
                               LB_Word_T     pa_Size,
                               LB_Word_T     pa_Flags);

    LB_Bool_T           IsNull();
    LB_Bool_T           IsRequired();
    LB_Bool_T           IsSigned();

    LB_Bool_T           GetValue(LB_String_T& pa_String);
    LB_Bool_T           SetValue(LB_String_T& pa_String);

    LB_Word_T           GetDecimalCount();
    void                GetName(LB_String_T& pa_Name);
    LB_Word_T           GetSize();
    LB_Word_T           GetType();


  private:
    LB_String_T         m_ColName;
    LB_Word_T           m_ColSize;
    LB_Word_T           m_ColFlags;
    LB_String_T         m_ColData;
    LB_CobolComp_T      m_CobComp;
  };
#endif
