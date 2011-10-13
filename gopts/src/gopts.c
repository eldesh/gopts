#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include <gopts.h>
#include <strarray.h>


struct option_spec_	{
	char const * name;	// name of the option
//	char const * desc;	// description of the option
	void       * value;
	option_spec_type type;
	parser_type parser;	// input string parser of value of type of option
	move_type  mover;   // move parsed value correspond to type
	struct option_spec_ * next;	// singly linked list
};


static char const * find_option(int argc, char const * const * argv, char const * opt);
static int abbr_to_optname(char * buff, char const * abbr);

// move_type
static void * move_bool(void * dsc, void * src);
static void * move_int (void * dsc, void * src);
static void * move_ptr (void * dsc, void * src);


static bool is_not_space(char c);
static char const * skip_space(char const * str);



option_spec * make_option_spec(char const * name, void * addr
							   , option_spec_type type
							   , parser_type parser
							   , move_type mover
							   , option_spec * tl)
{
	option_spec * spec=(option_spec*)calloc(1, sizeof(option_spec));
	if (!spec)
		return NULL;
	spec->name          = name;
	spec->value         = addr;
	spec->type          = type;
	spec->parser        = parser;
	spec->mover         = mover;
	spec->next          = tl;
	return spec;
}
void free_option_spec(option_spec * s) {
	if (s) {
		s->name         =NULL;
		s->value        =NULL;
		s->parser       =NULL;
		s->mover        =NULL;
		free(s);
	}
}
void free_option_specs(option_spec * os) {
	option_spec * p=NULL;
	option_spec * tmp=NULL;
	for (p=os; p; p=tmp) {
		tmp=p->next;
		free_option_spec(p);
	}
}

option_spec * append_option_spec(option_spec * xs, option_spec * ys) {
	if (!xs)
		return ys;
	{
		option_spec * p=xs;
		while (p->next)
			p=p->next;
		p->next = ys;
	}
	return xs;
}

option_spec * next_option(option_spec const * s) {
	assert(s);
	return s->next;
}


option_spec * make_spec_int   (char const * name, int  * addr, option_spec * ss) {
	return make_option_spec(name, (void*)addr, GOPT_SPEC_HAS_VALUE, parse_int, move_int, ss);
}
option_spec * make_spec_bool  (char const * name, bool * addr, option_spec * ss) {
	return make_option_spec(name, (void*)addr, GOPT_SPEC_HAS_VALUE, parse_bool, move_bool, ss);
}
option_spec * make_spec_string(char const * name, char ** addr, option_spec * ss) {
	return make_option_spec(name, (void*)addr, GOPT_SPEC_HAS_VALUE, success_parser, move_ptr, ss);
}
option_spec * make_spec_novalue(char const * name, bool * addr, option_spec * ss) {
	return make_option_spec(name, (void*)addr, GOPT_SPEC_HAS_NO_VALUE, parse_bool, move_bool, ss);
}

// parse option strings specified by spec.
static void parse_option(int argc, char const * const * argv, option_spec * ospec) {
	assert(ospec);
	if (ospec->type==GOPT_SPEC_HAS_VALUE) {
		if (ospec->value)
			load_option_if_exist_generic(ospec->mover, ospec->value, argc, argv, ospec->name, ospec->parser);
		else
			ospec->value = read_option_generic(argc, argv, ospec->name, ospec->parser);
	} else if (ospec->type==GOPT_SPEC_HAS_NO_VALUE) {
		bool * has=make_bool(has_novalue_option(argc, argv, ospec->name));
		if (ospec->value) // specifed load address
			ospec->mover(ospec->value, has);
		else
			ospec->value = has;
	}
}

void parse_options(int argc, char const * const * argv, option_spec * ss) {
	option_spec * p=ss;
	for (; p; p=p->next)
		parse_option(argc, argv, p);
}


static char const * find_option(int argc, char const * const * argv, char const * opt) {
	int i;
	for (i=0; i<argc; ++i) {
		if (!strncmp(opt, argv[i], strlen(opt)))
			return argv[i];
	}
	return NULL; // not found
}

// construct option-string from abbreviation(abbr), write it to buffer specified with 'buff'.
static int abbr_to_optname(char * buff, char const * abbr) {
	return sprintf(buff, "--%s=", abbr);
}

bool has_novalue_option(int argc, char const * const * argv, char const * opt) {
	char opt_name[256];
	sprintf(opt_name, "--%s", opt);
	return find_strs(argc, argv, opt_name)!=NULL;
}

bool has_option(int argc, char const * const * argv, char const * abbr) {
	if (has_novalue_option(argc, argv, abbr))
		return true;
	else { // it is not found for option that has no values
		char * r=read_option_string(argc, argv, abbr);
		if (r) { // a value is found for option that has a value
			free(r);
			return true;
		}
	}
	return false;
}

void * read_option_generic(int argc, char const * const * argv, char const * abbr, parser_type ctor_from_str) {
	char opt_name[256];
	char const * optstr=NULL;
	abbr_to_optname(opt_name, abbr);
	optstr = find_option(argc, argv, opt_name);
	if (optstr) {
		// pass tail of options to given parser
		parse_result r=ctor_from_str(&optstr[strlen(opt_name)]);
		if (r.result && r.rest && *r.rest=='\0')
			return r.result;
		else
			return NULL;
	} else
		return NULL;
}

// read int from the string specified with option that is associated with `abbr`
// if any string is not specified or non digit is used, return NULL.
int * read_option_int(int argc, char const * const * argv, char const * abbr) {
	return read_option_generic(argc, argv, abbr, parse_int);
}

bool * read_option_bool(int argc, char const * const * argv, char const * abbr) {
	return read_option_generic(argc, argv, abbr, parse_bool);
}

static void * unwrap_ptr_ptr (void ** ptr) {
	if (ptr) {
		void * p = *ptr;
		free(ptr);
		return p;
	}
	return NULL;
}
char * read_option_string(int argc, char const * const * argv, char const * abbr) {
	return unwrap_ptr_ptr(read_option_generic(argc, argv, abbr, success_parser));
}
FILE * read_option_read_file(int argc, char const * const * argv, char const * abbr) {
	return unwrap_ptr_ptr(read_option_generic(argc, argv, abbr, parse_read_file));
}
FILE * read_option_write_file(int argc, char const * const * argv, char const * abbr) {
	return unwrap_ptr_ptr(read_option_generic(argc, argv, abbr, parse_write_file));
}

int * read_option(int argc, char const * const * argv, char const * abbr) {
	return read_option_int(argc, argv, abbr); // default is `int`
}

/// mover
static void * move_bool(void * dst, void * src) {
	(*(bool*)dst)=*(bool*)src;
	free(src);
	return dst;
}
static void * move_int(void * dst, void * src) {
	(*(int*)dst)=*(int*)src;
	free(src);
	return dst;
}

static void * move_ptr(void * dst, void * src) {
	void ** dst_ = (void**)dst;
	void ** src_ = (void**)src;
	*dst_ = *src_;
	free(src_);
	return dst_;
}

// if option is specified with "abbr", write the value to des.
bool load_option_if_exist(int * des, int argc, char const * const * argv, char const * abbr) {
	return load_option_if_exist_int(des, argc, argv, abbr);
}

bool load_option_if_exist_generic(move_type move
				, void * dst, int argc, char const * const * argv, char const * abbr, parser_type parser) {
	void * v=read_option_generic(argc, argv, abbr, parser);
	if (v) {
		if (dst)
			move(dst, v);
		return true;
	} else
		return false;
}

bool load_option_if_exist_bool(bool * dst, int argc, char const * const * argv, char const * abbr) {
	return load_option_if_exist_generic(move_bool
		, (void*)dst, argc, argv, abbr, parse_bool);
}
bool load_option_if_exist_int (int * dst, int argc, char const * const * argv, char const * abbr) {
	return load_option_if_exist_generic(move_int
		, (void*)dst, argc, argv, abbr, parse_int);
}

#define LOAD_OPTION_IF_EXIST_PTR_TYPE(parser) \
	load_option_if_exist_generic(move_ptr , (void*)dst, argc, argv, abbr, parser)

bool load_option_if_exist_string(char ** dst, int argc, char const * const * argv, char const * abbr) {
	return LOAD_OPTION_IF_EXIST_PTR_TYPE(success_parser);
}
bool load_option_if_exist_read_file(FILE ** dst, int argc, char const * const * argv, char const * abbr) {
	return LOAD_OPTION_IF_EXIST_PTR_TYPE(parse_read_file);
}
bool load_option_if_exist_write_file(FILE ** dst, int argc, char const * const * argv, char const * abbr) {
	return LOAD_OPTION_IF_EXIST_PTR_TYPE(parse_write_file);
}
#undef LOAD_OPTION_IF_EXIST_PTR_TYPE

static bool is_not_space(char c) {
	return !isspace((int)c);
}

char const * find_if_string(char const * str, bool (*pred)(char)) {
	size_t i;
	for (i=0; i<strlen(str); ++i) {
		if (pred(str[i]))
			return &str[i];
	}
	return &str[i]; // str[i]=='\0'
}

static char const * skip_space(char const * str) {
	return find_if_string(str, is_not_space);
}

bool always_valid(char const * _) { return true; }

bool is_cplusplus_style_comment(char const * str) {
	return strstr(skip_space(str), "//")!=NULL;
}

bool is_sh_style_comment(char const * str) {
	return strstr(skip_space(str), "#")!=NULL;
}

bool is_lisp_style_comment(char const * str) {
	return strstr(skip_space(str), ";")!=NULL;
}
