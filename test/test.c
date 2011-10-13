#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gopts.h>
#include <parser.h>

#define PP_TOSTRING(stmt) PP_TOSTRING_I(stmt)
#define PP_TOSTRING_I(stmt) #stmt

static int test_num=0;
static int test_pass=0;
#define TEST(expr) TEST_I(expr)
#define TEST_I(expr)                                                                 \
    do {                                                                             \
		bool pp_test_pass__=(expr)!=0;                                               \
	    fprintf(stderr, "TEST(%d) @%d %s <%s>\n"                                     \
			, test_num, __LINE__, (pp_test_pass__?"pass":"fail"), PP_TOSTRING(expr));\
		test_pass += pp_test_pass__;                                                 \
		++test_num;                                                                  \
	} while (0)

// return immediately if it fail
#define TEST_FATAL(expr) TEST_FATAL_I(expr)
#define TEST_FATAL_I(expr)                                 \
	do {                                                   \
		bool pp_test_pass_=(expr)!=0;                      \
		++test_num;                                        \
		if (!pp_test_pass_) {                              \
			fprintf(stderr, "TEST_FATAL(%s) @%d failure\n" \
				, PP_TOSTRING(expr), __LINE__);            \
			return false;                                  \
		}                                                  \
		++test_pass;                                       \
	} while (0)

void test_report(void) {
	printf("test results...\n");
	printf("\tpass/sum %10d/%d\n", test_pass, test_num);
	printf("\tfail     %10d\n", test_num-test_pass);
	if (test_pass==test_num)
		printf("Congratulations! your code has passed all test cases!\n");
}

bool test_read_option(void) {
	char const ** argv = (char const**)make_strs(10, "--a=10"        , "--bc=0"          , "--bcd=20"
		                       , "--in=cde.conf" , "--output=def.out", "--efg=debug.log"
							   , "--fg24hij=true", "--kl_n0qr=false" , "--stuvw=true"
							   , "--xyz");
//	print_strs(10, argv);
	TEST(10==*read_option_int(10, argv, "a"));
	TEST( 0==*read_option_int(10, argv, "bc"));
	TEST(20==*read_option_int(10, argv, "bcd"));
	TEST( *read_option_bool(10, argv, "fg24hij"));
	TEST(!*read_option_bool(10, argv, "kl_n0qr"));
	TEST( *read_option_bool(10, argv, "stuvw"  ));
	TEST(!strcmp("cde.conf" , read_option_string(10, argv, "in"    )));
	TEST(!strcmp("def.out"  , read_option_string(10, argv, "output")));
	TEST(!strcmp("debug.log", read_option_string(10, argv, "efg"   )));

	{
		FILE * rf = read_option_read_file (10, argv, "in"    );
		FILE * wf = read_option_write_file(10, argv, "output");
		char buff[64];
		fprintf(wf, "output");
		fscanf(rf, "%s", buff);
		TEST(!strcmp(buff, "cde.conf"));
		TEST(!read_option_read_file(10, argv, "efg")); // no such file
		fclose(rf);
		fclose(wf);
	}

	// This string is specified, but it is not found, because parsing empty string must fail.
	TEST(has_option(10, argv, "xyz"));
	TEST(!read_option_int   (10, argv, "xyz"));
	TEST(!read_option_bool  (10, argv, "xyz"));
	TEST(!read_option_string(10, argv, "xyz"));

	TEST(!read_option_int(10, argv, "in"     ));
	TEST(!read_option_int(10, argv, "output" ));
	TEST(!read_option_int(10, argv, "efg"    ));
	TEST(!read_option_int(10, argv, "fg24hij"));
	TEST(!read_option_int(10, argv, "kl_n0qr"));
	TEST(!read_option_int(10, argv, "stuvw"  ));
	TEST(!read_option_int(10, argv, "i"));

	TEST(read_option_string(10, argv, "a"  )); // string parser accept(and success) any input
	TEST(read_option_string(10, argv, "bc" ));
	TEST(read_option_string(10, argv, "bcd"));

	free_strs(10, (char**)argv);
	return true;
}

bool test_has_option(void) {
	char const ** argv = (char const**)make_strs(10, "--a=10"        , "--bc=0"          , "--bcd=20"
		                       , "--in=cde.conf" , "--output=def.out", "--efg=debug.log"
							   , "--fg24hij=true", "--kl_n0qr=false" , "--stuvw=true"
							   , "--xyz");
//	print_strs(10, argv);
	TEST(has_option(10, argv, "a"));
	TEST(has_option(10, argv, "bc"));
	TEST(has_option(10, argv, "bcd"));
	TEST(has_option(10, argv, "fg24hij"));
	TEST(has_option(10, argv, "kl_n0qr"));
	TEST(has_option(10, argv, "stuvw"  ));
	TEST(has_option(10, argv, "in"    ));
	TEST(has_option(10, argv, "output"));
	TEST(has_option(10, argv, "efg"   ));
	// because it is specified with "--xyz", it is found without value specified
	TEST(has_option(10, argv, "xyz"));

	TEST(!has_option(10, argv, "aa"));
	TEST(!has_option(10, argv, "bbc "));
	TEST(!has_option(10, argv, "abc"));
	TEST(!has_option(10, argv, "cd"));
	TEST(!has_option(10, argv, "e fg"));
	TEST(!has_option(10, argv, "inin"));
	TEST(!has_option(10, argv, "outpu"));
	TEST(!has_option(10, argv, "fg24h"));
	TEST(!has_option(10, argv, "hgoehoge"));
	TEST(!has_option(10, argv, "foobar"));
	TEST(!has_option(10, argv, "sttuvw"));
	free_strs(10, (char**)argv);
	return true;
}


bool test_load_option_if_exist(void) {
	char const ** argv = (char const**)make_strs(10, "--a=10"        , "--bc=0"          , "--bcd=20"
		                       , "--in=cde.conf" , "--output=def.out", "--efg=debug.log"
							   , "--fg24hij=true", "--kl_n0qr=false" , "--stuvw=true"
							   , "--xyz");
	int n;
	char * str = (char*)malloc(64);
	bool b;
//	print_strs(10, argv);
	TEST(load_option_if_exist(&n, 10, argv, "a"));
	TEST(n==10);
	TEST(load_option_if_exist_int(&n, 10, argv, "bc"));
	TEST(n==0);
	TEST(load_option_if_exist_int(&n, 10, argv, "bcd"));
	TEST(n==20);
	TEST(load_option_if_exist_bool(&b, 10, argv, "fg24hij"));
	TEST(b==true);
	TEST(load_option_if_exist_bool(&b, 10, argv, "kl_n0qr"));
	TEST(b==false);
	TEST(load_option_if_exist_bool(&b, 10, argv, "stuvw"  ));
	TEST(b==true);
	TEST(load_option_if_exist_string(&str, 10, argv, "in"    ));
	TEST(!strcmp(str, "cde.conf"));
	TEST(load_option_if_exist_string(&str, 10, argv, "output"));
	TEST(!strcmp(str, "def.out"));
	TEST(load_option_if_exist_string(&str, 10, argv, "efg"   ));
	TEST(!strcmp(str, "debug.log"));
	/// read file
	{
		FILE * rf = NULL;
		FILE * wf = NULL;
		char buff[64];
		TEST( load_option_if_exist_read_file (&rf, 10, argv, "in"    ));
		TEST(rf);
		TEST( load_option_if_exist_write_file(&wf, 10, argv, "output"));
		TEST(wf);
		TEST(!load_option_if_exist_read_file (NULL, 10, argv, "efg"   ));

		fprintf(wf, "output");
		fscanf(rf, "%s", buff); TEST(!strcmp(buff, "cde.conf"));

		fclose(rf);
		fclose(wf);
	}

	// this can't be used for options which don't take values
	//TEST(load_option_if_exist_???(<???>, 10, argv, "xyz"));
	free_strs(10, (char**)argv);
	free(str);
	return true;
}

bool test_has_novalue_option(void) {
	char const ** argv = (char const**)make_strs(10, "--a=10"        , "--bc=0"          , "--bcd=20"
		                       , "--in=cde.conf" , "--output=def.out", "--efg=debug.log"
							   , "--fg24hij=true", "--kl_n0qr=false" , "--stuvw=true"
							   , "--xyz");
//	print_strs(10, argv);
	TEST(!has_novalue_option(10, argv, "a"));       // if a value specified, it is not found
	TEST(!has_novalue_option(10, argv, "bc"));
	TEST(!has_novalue_option(10, argv, "bcd"));
	TEST(!has_novalue_option(10, argv, "fg24hij"));
	TEST(!has_novalue_option(10, argv, "kl_n0qr"));
	TEST(!has_novalue_option(10, argv, "stuvw"  ));
	TEST(!has_novalue_option(10, argv, "in"    ));
	TEST(!has_novalue_option(10, argv, "output"));
	TEST(!has_novalue_option(10, argv, "efg"   ));

	TEST(has_novalue_option(10, argv, "xyz"));

	TEST(!has_novalue_option(10, argv, "aa"));
	TEST(!has_novalue_option(10, argv, "bbc "));
	TEST(!has_novalue_option(10, argv, "abc"));
	TEST(!has_novalue_option(10, argv, "cd"));
	TEST(!has_novalue_option(10, argv, "e fg"));
	TEST(!has_novalue_option(10, argv, "inin"));
	TEST(!has_novalue_option(10, argv, "outpu"));
	TEST(!has_novalue_option(10, argv, "fg24h"));
	TEST(!has_novalue_option(10, argv, "hgoehoge"));
	TEST(!has_novalue_option(10, argv, "foobar"));
	TEST(!has_novalue_option(10, argv, "sttuvw"));
	free_strs(10, (char**)argv);
	return true;
}

bool test_lines_file(void) {
	char const ** ss = (char const**)make_strs(10, "--a=10"        , "--bc=0"          , "--bcd=20"
		                     , "--in=cde.conf" , "--output=def.out", "--efg=debug.log"
							 , "--fg24hij=true", "--kl_n0qr=false" , "--stuvw=true"
							 , "--xyz");
	{
		FILE * fp=fopen("testcase/test0.conf", "r");
		int size;
		char const ** fs=NULL;
		TEST_FATAL(fp);
		fs=(char const**)lines_file(fp, &size, add_double_hyphen, is_cplusplus_style_comment);
//		printf("fs:"); print_strs(10, fs);
//		printf("ss:"); print_strs(10, ss);
		TEST(equals_strs(10, ss, fs));
		free_strs(size, (char**)fs);
		fclose(fp);
	}
	{
		FILE * fp=fopen("testcase/test1.conf", "r");
		int size;
		char const ** fs=NULL;
		TEST_FATAL(fp);
		fs=(char const**)lines_file(fp, &size, add_double_hyphen, is_cplusplus_style_comment);
		TEST(equals_strs(10, ss, fs));
		free_strs(size, (char**)fs);
		fclose(fp);
	}
	free_strs(10, (char**)ss);
	return true;
}

bool test_int_parse(void) {
	int v;
	char const * const argv[10] = {"--a=10"   , "--b=0"   , "--c=20"  , "--d=0x0"
		             , "--e=0x1"  , "--f=0x10", "--g=0xff", "--h=0xffff"
					 , "--i=0x256", "--j"};
	size_t size = sizeof(argv)/sizeof(argv[0]);
	TEST(read_option_int(size, argv, "a") && 10    ==*read_option_int(size, argv, "a"));
	TEST(read_option_int(size, argv, "b") &&  0    ==*read_option_int(size, argv, "b"));
	TEST(read_option_int(size, argv, "c") && 20    ==*read_option_int(size, argv, "c"));
	TEST(read_option_int(size, argv, "d") && 0x0   ==*read_option_int(size, argv, "d"));
	TEST(read_option_int(size, argv, "e") && 0x1   ==*read_option_int(size, argv, "e"));
	TEST(read_option_int(size, argv, "f") && 0x10  ==*read_option_int(size, argv, "f"));
	TEST(read_option_int(size, argv, "g") && 0xff  ==*read_option_int(size, argv, "g"));
	TEST(read_option_int(size, argv, "h") && 0xffff==*read_option_int(size, argv, "h"));
	TEST(read_option_int(size, argv, "i") && 0x256 ==*read_option_int(size, argv, "i"));
	TEST(!read_option_int(size, argv, "j"));

	TEST(load_option_if_exist(&v, size, argv, "a"));
	TEST(v==10);
	TEST(load_option_if_exist(&v, size, argv, "b"));
	TEST(v== 0);
	TEST(load_option_if_exist(&v, size, argv, "c"));
	TEST(v==20);
	TEST(load_option_if_exist(&v, size, argv, "d"));
	TEST(v==0x0);
	TEST(load_option_if_exist(&v, size, argv, "e"));
	TEST(v==0x1);
	TEST(load_option_if_exist(&v, size, argv, "f"));
	TEST(v==0x10);
	TEST(load_option_if_exist(&v, size, argv, "g"));
	TEST(v==0xff);
	TEST(load_option_if_exist(&v, size, argv, "h"));
	TEST(v==0xffff);
	TEST(load_option_if_exist(&v, size, argv, "i"));
	TEST(v==0x256);
	TEST(!load_option_if_exist(&v, size, argv, "j"));
	TEST(v==0x256);
	return true;
}

bool test_parse_options(void) {
	int av, bv, cv, dv, ev, fv, gv, hv, iv;
	bool jv=false, kv=false, bar=false;
	char * str = (char*)malloc(64);
	char const * const argv[  ] = {"--a=10"   , "--b=0"   , "--c=20"  , "--d=0x0"
		             , "--e=0x1"  , "--f=0x10", "--g=0xff", "--h=0xffff"
					 , "--i=0x256", "--j"
					 , "--k=true"
					 , "--str=hoge"
					 , "--bar"};
	size_t const size = sizeof(argv)/sizeof(argv[0]);

	option_spec * ss =
		make_spec_int("a", &av,
		make_spec_int("b", &bv,
		make_spec_int("c", &cv,
		make_spec_int("d", &dv,
		make_spec_int("e", &ev,
		make_spec_int("f", &fv,
		make_spec_int("g", &gv,
		make_spec_int("h", &hv,
		make_spec_int("i", &iv,
		make_spec_novalue("j"  , &jv,
		make_spec_bool   ("k"  , &kv,
		make_spec_string ("str", &str,
		make_spec_novalue("bar", &bar, NULL)))))))))))));

	parse_options(size, argv, ss);

	TEST(av==10    );
	TEST(bv==0     );
	TEST(cv==20    );
	TEST(dv==0x0   );
	TEST(ev==0x1   );
	TEST(fv==0x10  );
	TEST(gv==0xff  );
	TEST(hv==0xffff);
	TEST(iv==0x256 );
	TEST(jv);
	TEST(kv);
	TEST(!strcmp(str, "hoge"));
	TEST(bar);

	free_option_specs(ss);
	free(str);
	return true;
}

bool test_parse_separate_options(void) {
	int av, bv, cv, dv, ev, fv, gv, hv, iv;
	bool jv=false, kv=false, bar=false;
	char * str = (char*)malloc(64);
	char const * const argv[  ] = {"--a=10"   , "--b=0"   , "--c=20"  , "--d=0x0"
		             , "--e=0x1"  , "--f=0x10", "--g=0xff", "--h=0xffff"
					 , "--i=0x256", "--j"
					 , "--k=true"
					 , "--str=hoge"
					 , "--bar"};
	size_t const size = sizeof(argv)/sizeof(argv[0]);

	option_spec * ss_ae =
		make_spec_int("a", &av,
		make_spec_int("b", &bv,
		make_spec_int("c", &cv,
		make_spec_int("d", &dv,
		make_spec_int("e", &ev, NULL)))));
	option_spec * aa_fi =
		make_spec_int("f", &fv,
		make_spec_int("g", &gv,
		make_spec_int("h", &hv,
		make_spec_int("i", &iv, NULL))));
	option_spec * aa_je =
		make_spec_novalue("j"  , &jv,
		make_spec_bool   ("k"  , &kv,
		make_spec_string ("str", &str,
		make_spec_novalue("bar", &bar, NULL))));

	option_spec * ss_all = append_option_spec(ss_ae,
		                       append_option_spec(aa_fi, aa_je));

	parse_options(size, argv, ss_all);

	TEST(av==10    );
	TEST(bv==0     );
	TEST(cv==20    );
	TEST(dv==0x0   );
	TEST(ev==0x1   );
	TEST(fv==0x10  );
	TEST(gv==0xff  );
	TEST(hv==0xffff);
	TEST(iv==0x256 );
	TEST(jv);
	TEST(kv);
	TEST(!strcmp(str, "hoge"));
	TEST(bar);

	free_option_specs(ss_ae);
	free(str);
	return true;
}

bool test_parser(void) {
	{
		char const * str="0.0.0.0";
		parse_result r=parse_ipv4addr(str);
		TEST(r.result && !strcmp(r.result, str));
		free_parse_result(r);
	}
	{
		char const * str="100.100.100.100";
		parse_result r=parse_ipv4addr(str);
		TEST(r.result && !strcmp(r.result, str));
		free_parse_result(r);
	}
	{
		char const * str="255.255.255.255";
		parse_result r=parse_ipv4addr(str);
		TEST(r.result && !strcmp(r.result, str));
		free_parse_result(r);
	}
	{
		char const * str="0.1.2.3";
		parse_result r=parse_ipv4addr(str);
		TEST(r.result && !strcmp(r.result, str));
		free_parse_result(r);
	}
	{
		char const * str="0.0.0";
		parse_result r=parse_ipv4addr(str);
		TEST(!r.result && r.rest==str);
		free_parse_result(r);
	}
	{
		char const * str="0";
		parse_result r=parse_ipv4addr(str);
		TEST(!r.result && r.rest==str);
		free_parse_result(r);
	}
	{
		char const * str="0.0.0.256";
		parse_result r=parse_ipv4addr(str);
		TEST(!r.result && r.rest==str);
		free_parse_result(r);
	}
	{
		char const * str="1.0.0.";
		parse_result r=parse_ipv4addr(str);
		TEST(!r.result && r.rest==str);
		free_parse_result(r);
	}
	{
		char const * str="";
		parse_result r=parse_ipv4addr(str);
		TEST(!r.result && r.rest==str);
		free_parse_result(r);
	}
	{
		char const * str="abce";
		parse_result r=parse_ipv4addr(str);
		TEST(!r.result && r.rest==str);
		free_parse_result(r);
	}
	return true;
}

int main () {
	test_read_option();
	test_has_option();
	test_load_option_if_exist();
	test_has_novalue_option();
	test_lines_file();
	test_int_parse();
	test_parse_options();
	test_parse_separate_options();
	test_parser();
	test_report();
	return 0;
}

