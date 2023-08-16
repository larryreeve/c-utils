#ifndef  LB_STRLST_HPP
#define  LB_STRLST_HPP


#include <lbport.h>
#include <lbstring.hpp>
#include <lbstrlst.hpp>


class LB_StringList_T
  {
  public:
    LB_StringList_T();

    ~LB_StringList_T();

    LB_Bool_T           Add(LB_String_T& pa_String,
                            LB_Word_T    pa_KeyOffset,
                            LB_Word_T    pa_KeyLength);

    void                Clear();

    void                DiagDisplay();


    LB_Bool_T           Exists(LB_String_T& pa_KeyString,
                               LB_Word_T    pa_KeyOffset,
                               LB_Word_T    pa_KeyLength);

    LB_Bool_T           Retrieve(LB_String_T& pa_KeyString,
                                 LB_Word_T    pa_KeyOffset,
                                 LB_Word_T    pa_KeyLength,
                                 LB_String_T& pa_Value);

    LB_Bool_T           GetNext(LB_String_T& pa_String);

    void                GetReset();

    void                SetAsUnsorted();

  private:
    //--* Variables
    LB_Bool_T           m_SortedFlag;

    struct STRLIST_NODE_T
      {
      struct STRLIST_NODE_T     *NextPtr;
      LB_String_T               StringData;
      };

    struct STRLIST_NODE_T       *m_HeadPtr;
    struct STRLIST_NODE_T       *m_GetPtr;

    //--* Functions
    LB_Bool_T           m_Search(LB_String_T& pa_KeyString,
                                 LB_Word_T    pa_KeyOffset,
                                 LB_Word_T    pa_KeyLength,
                                 LB_String_T& pa_Value);
  };
#endif
