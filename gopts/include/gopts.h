#if !defined GENERIZED_GOPTS_INCLUDED
#define      GENERIZED_GOPTS_INCLUDED

#include <strarray.h>
#include <bool.h>
#include <parser.h>

// function type, copy from 'src' to 'dst' and return dst
typedef void * (*copy_type)(void * dst, void const * src);

// function type
//  that parse the given string and construct a (boxed)value
//  and return it's address.
// return NULL if parse or ctor fail.
typedef parse_result (*parser_type)(char const*);

struct option_spec_;
typedef struct option_spec_ option_spec;

typedef
    enum { GOPT_SPEC_HAS_NO_VALUE, GOPT_SPEC_HAS_VALUE }
option_spec_type;

option_spec * make_option_spec(char const * name, void * addr, option_spec_type type
							   , parser_type parser, copy_type copyer, option_spec * tl);
void free_option_specs(option_spec * ss);
option_spec * append_option_spec(option_spec * xs, option_spec * ys);

option_spec * make_spec_int    (char const * name, int  * addr, option_spec * ss);
option_spec * make_spec_bool   (char const * name, bool * addr, option_spec * ss);
option_spec * make_spec_string (char const * name, char * addr, option_spec * ss);
option_spec * make_spec_novalue(char const * name, bool * addr, option_spec * ss);

void parse_options(int argc, char const * const * argv, option_spec * ss);


// It is returned whether the option was specified.
bool has_option(int argc, char const * const * argv, char const * abbr);
// It is returned whether the option was specified.
// the option take no values.
bool has_novalue_option(int argc, char const * const * argv, char const * opt);


////
// load_option_if_exist_*
//
// if option is specified with 'abbr', write the parsed value to 'dst'.
// It is returned that whether option is specified or not.
//   argc : number of elements of argv
//   argv : array of strings
//   abbr : name(abbreviation) of option
//

// it takes the parser and the copy function for a parsed value.
bool load_option_if_exist_generic(copy_type copy
							   , void * dst, int argc, char const * const * argv, char const * abbr, parser_type parser);
// load_option_if_exist_genericis is specialized to each types
bool load_option_if_exist_bool  (bool * dst, int argc, char const * const * argv, char const * abbr);
bool load_option_if_exist_int   (int  * dst, int argc, char const * const * argv, char const * abbr);
bool load_option_if_exist_string(char * dst, int argc, char const * const * argv, char const * abbr);
// same the int specialized version
bool load_option_if_exist       (int  * dst, int argc, char const * const * argv, char const * abbr);
////

////
// read_option_*
//
// return the pointer that point to a value
//   which is read from string that is specified with 'abbr'.
//
//   argc : number of elements of argv
//   argv : array of strings
//   abbr : name(abbreviation) of option

// specify the parser(ctor_from_str), that takes the value is associated with 'abbr'
// return null, if the option is not specified.
void * read_option_generic(int argc, char const * const * argv, char const * abbr, parser_type ctor_from_str);
//// generic version is specialized to each types
int  * read_option_int   (int argc, char const * const * argv, char const * abbr);
bool * read_option_bool  (int argc, char const * const * argv, char const * abbr);
char * read_option_string(int argc, char const * const * argv, char const * abbr);
// same the int specialized version
int  * read_option       (int argc, char const * const * argv, char const * abbr);
////

bool always_valid(char const * dummy);
bool is_sh_style_comment(char const * str);
bool is_lisp_style_comment(char const * str);
bool is_cplusplus_style_comment(char const * str);

#endif    /* GENERIZED_GOPTS_INCLUDED */
