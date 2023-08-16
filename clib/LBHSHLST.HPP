#ifndef  LB_HASHLIST_HPP
#define  LB_HASHLIST_HPP


#include <lbport.h>
#include <lbstring.hpp>
#include <lbhshlst.hpp>

#define HASHLIST_TABLE_SIZE     1999


class LB_HashList_T
  {
  public:
    LB_HashList_T();

    ~LB_HashList_T();

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

  private:
    struct HASHLIST_NODE_T
      {
      struct HASHLIST_NODE_T    *NextPtr;
      LB_String_T               StringData;
      };

    struct HASHLIST_NODE_T*     m_HashTable[HASHLIST_TABLE_SIZE];

    LB_Bool_T           m_Search(LB_String_T& pa_KeyString,
                                 LB_Word_T    pa_KeyOffset,
                                 LB_Word_T    pa_KeyLength,
                                 LB_String_T& pa_Value);
  };
#endif
