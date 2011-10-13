#if !defined PARSER_INCLUDED
#define      PARSER_INCLUDED

typedef struct parse_result_ {
	void * result;
	char const * rest;
} parse_result;

typedef struct substring {
	char const * str;
	size_t size;
} substring;

parse_result make_parse_result(void * result, char const * rest);
void free_parse_result(parse_result r);

substring fullstr(char const * str);
char * tostring(substring str);
substring take_string(size_t size, substring str);
substring take_if_string(bool (*pred)(char), substring str);

bool all_string(substring str, bool (*pred)(char));

int * make_int (int x);
bool * make_bool(bool b);

parse_result success_parser  (char const * str);
parse_result parse_int_dec   (char const * str);
parse_result parse_int_hex   (char const * str);
parse_result parse_int       (char const * str);
parse_result parse_bool      (char const * str);
parse_result parse_ipv4addr  (char const * str);
parse_result parse_read_file (char const * str);
parse_result parse_write_file(char const * str);

#endif    /* PARSER_INCLUDED */

