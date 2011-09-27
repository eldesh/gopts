#if !defined STR_ARRAY_INCLUDED
#define      STR_ARRAY_INCLUDED

#include <stdarg.h>
#include <bool.h>

char * string_dup(char const * str);

void for_each_strs_mutable(size_t size, char * argv[], void (*f)(char*));
void for_each_strs(size_t size, char const * argv[], void (*f)(char const*));

char const * const * find_strs        (size_t size, char const * const * argv, char const * v);
char       *       * find_strs_mutable(size_t size, char       *       * argv, char const * v);

bool equals_strs(size_t size, char const * const * lhs, char const * const * rhs);

void print_str(char const * str);
void print_strs(size_t size, char const * strs[]);
void free_strs(size_t size, char * argv[]);

char ** make_strs(size_t n, ...);
char ** make_strsv(size_t n, va_list ap);

// returns the array of cstrings of file(fp). number of lines is asigned to *line.
char ** lines_file(FILE * fp, int * line, char *(*trans)(char const *), bool (*is_invalid)(char const *));

char * add_double_hyphen(char const * str);

#endif    /* STR_ARRAY_INCLUDED */