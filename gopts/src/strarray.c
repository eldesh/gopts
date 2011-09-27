#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <strarray.h>

void for_each_strs_mutable(size_t size, char * argv[], void (*f)(char*)) {
	size_t i;
	for (i=0; i<size; ++i)
		f(argv[i]);
}

void for_each_strs(size_t size, char const * argv[], void (*f)(char const*)) {
	size_t i;
	for (i=0; i<size; ++i)
		f(argv[i]);
}

char const * const * find_strs (size_t size, char const * const * argv, char const * v) {
	size_t i;
	for (i=0; i<size; ++i) {
		if (!strcmp(argv[i], v))
			return &argv[i];
	}
	return NULL;
}
char ** find_strs_mutable(size_t size, char ** argv, char const * v) {
	size_t i;
	for (i=0; i<size; ++i) {
		if (!strcmp(argv[i], v))
			return &argv[i];
	}
	return NULL;
}

void print_str(char const * str) { printf("%s\n", str); }

void print_strs(size_t size, char const * strs[]) {
	printf("{\n");
	for_each_strs(size, strs, print_str);
	printf("}\n");
}

void free_strs(size_t size, char * argv[]) {
	for_each_strs_mutable(size, argv, (void(*)(char*))free);
	free(argv);
}

bool equals_strs(size_t size, char const * const * lhs, char const * const * rhs) {
	size_t i;
	for (i=0; i<size; ++i) {
		if (!lhs[i] || !rhs[i] || strcmp(lhs[i], rhs[i])) {
//			fprintf(stderr, "%s(%s, %s) error\n", __FUNCTION__, lhs[i], rhs[i]);
			return false;
		}
	}
	return true;
}

// deep copy a string
char * string_dup(char const * str) {
	char * p = (char*)malloc(sizeof(char)*(strlen(str)+1));
	return strcpy(p, str);
}

char ** make_strsv(size_t n, va_list ap) { assert(0<n); {
	char ** xxs=(char**)calloc(n, sizeof(char*));
	size_t i;
	for (i=0; i<n; ++i)
		xxs[i] = string_dup(va_arg(ap, char*));
	return xxs;
}}

char ** make_strs(size_t n, ...) { assert(0<n); {
	char ** r=NULL;
	va_list ap;
	va_start(ap, n);
	r=make_strsv(n, ap);
	va_end(ap);
	return r;
}}

// read a line from file(fp)
static char * get_line(FILE * fp) {
	int const LINE_MAX_LENGTH=512; // TODO: extend buffer until get a linebreak
	char * buff=(char*)calloc(LINE_MAX_LENGTH, sizeof(char));
	if (!buff)
		return NULL;

	if (fgets(buff, LINE_MAX_LENGTH, fp))
		return buff;
	else
		return NULL;
}

static char * id_str(char * p) {
	return p;
}

static void remove_linebreak(char * str) {
	char * r = strchr(str, '\n');
	if (r)
		*r = '\0';
	r = strchr(str, '\r');
	if (r)
		*r = '\0';
}

char * add_double_hyphen(char const * str) {
	char * buff=(char*)calloc(strlen(str)+1+2, sizeof(char));
	sprintf(buff, "--%s", str);
	return buff;
}

char ** lines_file(FILE * fp, int * line, char *(*trans)(char const *), bool (*is_invalid)(char const *)) {
	char * linebuff=NULL;
	size_t num=0;
	char ** strs = (char**)calloc(128, sizeof(char**)); // TODO: impl with realloc

	while ((linebuff=get_line(fp))!=NULL) {
		if (!is_invalid || !is_invalid(linebuff)) {
			remove_linebreak(linebuff);
			if (trans) {
				strs[num] = trans(linebuff);
				free(linebuff);
			} else
				strs[num] = linebuff;
			linebuff  = NULL;
			++num;
			//printf("%d th -> [%s] add\n", size, line);
		}
	}
	*line = num;
	return strs;
}
