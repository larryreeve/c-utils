#ifndef  LB_BLOB_HPP
#define  LB_BLOB_HPP


#include <lbport.h>
#include <lbfio.hpp>
#include <lbstring.hpp>


class LB_Blob_T
  {
  public:
                LB_Blob_T();

                ~LB_Blob_T();

		void            Clear();

                LB_Bool_T       GetValue(LB_Byte_T *pa_Buf, LB_Word_T pa_MaxSize);

                LB_Bool_T       SetValue(LB_Byte_T *pa_Buf, LB_Word_T pa_Size);

		LB_Bool_T       Open();

		LB_Bool_T       Create();

		void            Close();

                void            SetFileName(LB_CZString_T pa_Filename);

  private:
                LB_Fio_T        m_BlobFile;
                LB_String_T     m_FileName;
  };
#endif
