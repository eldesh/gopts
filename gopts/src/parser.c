#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>

#include <bool.h>
#include <strarray.h>
#include <parser.h>

parse_result make_parse_result(void * result, char const * rest) {
	parse_result re;
	re.result = result;
	re.rest = rest;
	return re;
}
void free_parse_result(parse_result r) {
	free(r.result);
	r.result=NULL;
}
substring fullstr(char const * str) {
	substring sub;
	sub.str = str;
	sub.size = strlen(str);
	return sub;
}
char * tostring(substring str) {
	char * buff=(char*)calloc(str.size+1, sizeof(char));
	return strncpy(buff, str.str, str.size+1);
}

substring take_string(size_t size, substring str) {
	substring sub;
	sub.str = str.str;
	sub.size = size;
	return sub;
}
substring take_if_string(bool (*pred)(char), substring str) {
	substring r;
	size_t i;
	for (i=0; i<str.size && pred(str.str[i]); ++i)
		;
	r.size = i;
	r.str = str.str;
	return r;
}

bool all_string(substring str, bool (*pred)(char)) {
	size_t i;
	for (i=0; i<str.size; ++i) {
		if (!pred(str.str[i]))
			return false;
	}
	return true;
}

int * make_int (int x) {
	int * p = calloc(1, sizeof(int));
	*p = x;
	return p;
}
bool * make_bool(bool b) {
	bool * p = calloc(1, sizeof(bool));
	*p = b;
	return p;
}
static bool is_digit(char c) {
	return isdigit((int)c) != 0;
}
static bool is_hex(char c) {
	return isdigit((int)c) || strchr("abcdefABCDEF", c);
}
static char const * last_char(char const * str) {
	assert(str);
	while (*str!='\0')
		++str;
	return str;
}
static void ** make_ptr(void * p) {
	void ** vp = (void**)malloc(sizeof(p));
	assert(vp);
	*vp = p;
	return vp;
}

parse_result success_parser(char const * str) {
	return make_parse_result(make_ptr(string_dup(str)), last_char(str));
}

parse_result parse_int_dec(char const * str) {
	if (!str)
		return make_parse_result(NULL, str);
	{
		substring pos=take_if_string(is_digit, fullstr(str));
		substring neg;
		if (0<pos.size)
			return make_parse_result(make_int(atoi(str)), pos.str+pos.size);
		neg=take_if_string(is_digit, fullstr(str+1));
		if (str[1]=='-' && 0<neg.size)
			return make_parse_result(make_int(atoi(str)), neg.str+neg.size+1);
	}
	return make_parse_result(NULL, str);
}
parse_result parse_int_hex(char const * str) {
	int x;
	if (!str)
		return make_parse_result(NULL, str);

	if (!strncmp(str, "0x", 2)) {
		substring hex=take_if_string(is_hex, fullstr(str+2));
		if (0<hex.size) {
			sscanf(str, "%x", &x);
			return make_parse_result(make_int(x), hex.str+hex.size);
		}
	}
	return make_parse_result(NULL, str);
}
parse_result parse_int(char const * str) {
	parse_result r=parse_int_hex(str);
	if (r.result)
		return r;
	else
		return parse_int_dec(str);
}
parse_result parse_bool(char const * str) {
	bool * r=NULL;
	if (!str)
		return make_parse_result(NULL, str);
	if (!strcmp(str, "true") ) r=make_bool(true);
	if (!strcmp(str, "false")) r=make_bool(false);

	// ‰ö‚µ‚°‚¾‚¯‚Çˆê‰žc
	if (!strcmp(str, "1")) r=make_bool(true);
	if (!strcmp(str, "0")) r=make_bool(false);

	return make_parse_result(r, last_char(str));
}

static bool in_range(int x, int min_, int max_) {
	return min_<=x && x<max_;
}

parse_result parse_ipv4addr(char const * str) {
	parse_result addr[4];
	addr[0]=parse_int_dec(str);
	addr[1]=parse_int_dec(addr[0].rest+1);
	addr[2]=parse_int_dec(addr[1].rest+1);
	addr[3]=parse_int_dec(addr[2].rest+1);
	if (   addr[0].result && in_range(*(int*)addr[0].result, 0, 256)
		&& addr[1].result && in_range(*(int*)addr[1].result, 0, 256)
		&& addr[2].result && in_range(*(int*)addr[2].result, 0, 256)
		&& addr[3].result && in_range(*(int*)addr[3].result, 0, 256)
		&& addr[0].rest && (*addr[0].rest=='.')
		&& addr[1].rest && (*addr[1].rest=='.')
		&& addr[2].rest && (*addr[2].rest=='.'))
	{
		size_t len=addr[3].rest-str+1;
		return make_parse_result(tostring(take_string(len, fullstr(str))), addr[3].rest);
	} else
		return make_parse_result(NULL, str);
}
