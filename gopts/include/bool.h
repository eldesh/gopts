#if !defined CLANG_BOOL_INCLUDED
#define      CLANG_BOOL_INCLUDED

#if defined _WIN32
   typedef enum { false=0, true } bool;
#elif __GNUC__
   typedef enum { false=0, true } bool;
#else
#  error
#endif

#endif    /* CLANG_BOOL_INCLUDED */