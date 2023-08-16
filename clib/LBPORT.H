#ifndef LB_PORTABLE
#define LB_PORTABLE

#define LB_DEBUG        // Turn assertions on

typedef unsigned short  LB_Bool_T;
typedef unsigned char   LB_Byte_T;
typedef unsigned short  LB_Word_T;
typedef unsigned long   LB_DWord_T;
typedef double          LB_FloatPt_T;
typedef unsigned short  LB_Size_T;
typedef const char *    LB_CZString_T;
typedef char *          LB_ZString_T;
typedef void *          LB_Pointer_T;

//ifdef PLATFORM_WIN16 || PLATFORM_DOS16
//--* Unsigned integers
//typedef unsigned short  LB_UINT8
//typedef unsigned int    LB_UINT16
//typedef unsigned long   LB_UINT32
//
////--* Signed integers
//typedef short           LB_INT8
//typedef int             LB_INT16
//typedef long            LB_INT32
//
//typedef LB_UINT8 *      LB_BytePtr_T
//#endif

#ifndef MAXPATHLEN
#define MAXPATHLEN      64
#endif
typedef char            path[MAXPATHLEN];
 
#define LB_BYTE(x)      ((x) & 0xff)
#define LB_HIBYTE(x)    (((x) >> 8) & 0xff)
#define LB_LOBYTE(x)    ((x) && 0xff)
#define LB_HIWORD(x)    (((x >> 16) & 0xffffL)
#define LB_LOWORD(x)    ((x) & 0xffffL)
#define LB_CHAR(x)      ((x) & 0x7f)
#define LB_WORD(x)      ((x) & 0xffffL)
#define LB_DECODE(x)    ((int) ((x) - '0'))

/* long unsigned right shift */
#define LB_LURSHIFT(n,b) (((long) (n) >> (b)) & (0x7fffffffL >> ((b) - 1)))

/* determine number of array elements */
#define LB_DIM(a)          (sizeof(a) / sizeof(*(a)))

// #define public
// #define private         static

#define LB_TRUE         1
#define LB_FALSE        0

#define LB_SUCCEED      0
#define LB_FAIL         (-1)

#define LB_EXIT_SUCCESS 0
#define LB_EXIT_FAILURE 1

#define LB_EOL          '\n'
#define LB_EOS          '\0'
#define LB_EOP          '\14'

#endif
