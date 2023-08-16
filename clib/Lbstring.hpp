#ifndef LB_STRING_HPP
#define LB_STRING_HPP


#include <stdlib.h>

#include <lbport.h>

class LB_String_T
  {
  public:
    LB_String_T(void);
    LB_String_T(const LB_String_T& pa_Copy);   // copy constructor
    LB_String_T(LB_Word_T pa_InitialSize);     // set initial string size
    LB_String_T(LB_ZString_T pa_InString);     // initialize to const string

    ~LB_String_T();

    LB_String_T         operator +  (const LB_String_T&  pa_String) const;
    LB_String_T         operator +  (LB_ZString_T  pa_String);
    void                operator ++ (int);
    LB_String_T&        operator =  (LB_ZString_T  pa_String);
    LB_String_T&        operator =  (LB_CZString_T pa_String);
    LB_String_T&        operator =  (const LB_String_T& pa_String);
    LB_String_T&        operator =  (const LB_Word_T    pa_Value);
    LB_String_T&        operator =  (const LB_DWord_T   pa_Value);
    LB_String_T&        operator =  (const LB_FloatPt_T pa_Value);
    LB_Bool_T           operator == (LB_String_T&  pa_String);
    LB_Bool_T           operator == (LB_ZString_T  pa_String);
    LB_Bool_T           operator != (LB_String_T&  pa_String);
    LB_Bool_T           operator != (LB_ZString_T  pa_String);
    LB_Bool_T           operator <  (LB_String_T&  pa_String);
    LB_Byte_T&          operator[]  (LB_Word_T     pa_Position);


    //--* Input-Output Functions
    LB_Bool_T           Append(LB_CZString_T    pa_ZString);
    LB_Bool_T           Append(LB_String_T&     pa_String);
    LB_Bool_T           Append(const LB_Byte_T  pa_Byte);
    LB_Bool_T           AppendToAsciiZ(LB_ZString_T pa_InString, LB_Word_T pa_MaxSize);
    LB_Bool_T           AssignMoney(const LB_FloatPt_T pa_Value);
    LB_Bool_T           Insert(LB_CZString_T pa_ZString);
    LB_Bool_T           Insert(LB_String_T& pa_String);
    LB_Bool_T           SetBytes(LB_Byte_T *pa_Bytes, LB_Word_T pa_Size);
    LB_Bool_T           GetBytes(LB_Byte_T *pa_Bytes, LB_Word_T pa_MaxSize);


    //--* Conversion Functions
    LB_FloatPt_T        AsFloatPt();
    LB_Word_T           AsWord();
    LB_Bool_T           AsAsciiZ(LB_ZString_T pa_InString, LB_Word_T pa_MaxSize);
    void                UpperCase();
    void                LowerCase();
    void                Proper();

    //--* Utility Functions
    void                Clear(void);
    void                Display();
    void                DiagDisplay();
    void                Fill(const LB_Byte_T pa_FillByte);
    LB_Bool_T           Filter(const LB_Byte_T pa_Byte);
    LB_DWord_T          HashKey();
    LB_Bool_T           IsAllSpaces()   const;
    LB_Bool_T           IsEmpty()       const;
    void                MakeNumeric();
    LB_Bool_T           PadLeft(LB_Word_T pa_Size,  LB_Byte_T pa_PadChar);
    LB_Bool_T           PadRight(LB_Word_T pa_Size, LB_Byte_T pa_PadChar);
    LB_Word_T           Size()          const;
    LB_Word_T           BlockSize()     const;
    LB_Bool_T           Search(LB_String_T& pa_SearchString,
                               LB_Bool_T    pa_IgnoreCase);
    LB_String_T         SubstrLeft(LB_Word_T pa_Length) const;
    LB_String_T         SubstrMid(LB_Word_T pa_Offset, LB_Word_T pa_Length) const;
    LB_String_T         SubstrRight(LB_Word_T pa_Length) const;
    void                Trace(char * pa_SrcFileName, int pa_SrcLineNumber);
    LB_Bool_T           Trim();
    LB_Bool_T           TrimLeft();
    LB_Bool_T           TrimRight();
    LB_Bool_T           TrimTo(LB_Word_T ByteCnt);

  private:
    //--* Variables
    LB_Bool_T           m_ClassActive;
    LB_Byte_T           *m_Data;
    LB_Word_T           m_CurSize;
    LB_Word_T           m_BlkSize;

   //--* Functions
   LB_Bool_T            m_Resize(LB_Word_T pa_NewSize);
  };

#endif
