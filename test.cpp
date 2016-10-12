
#include <iostream>
#include "json.h"
#include "error.h"

int main_ret = 0;
int test_count = 0;
int test_pass = 0;

template <typename T>
void EXPECT_EQ(T expect, T actual)
{
	if (expect == actual) {
		test_pass++;
	} else {
		cout<< "expect '"<< expect<<"', got '"<<actual<<"'"<< endl;
		main_ret = 1;
	}
	test_count++;
}

static void test_parse_null() {
    cout<<__FUNCTION__<<endl;
	error err;
	json_type j;
    tie(err, j) = json_parse("null");
    EXPECT_EQ(OK, err);
    EXPECT_EQ(json_type::NULL_, j.type);
}

static void test_parse_true() {
    cout<<__FUNCTION__<<endl;
    error err;
	json_type j;
    tie(err, j) = json_parse("true");
    EXPECT_EQ(OK, err);
    EXPECT_EQ(json_type::BOOL, j.type);
    EXPECT_EQ(true, j.bval);
}

static void test_parse_false() {
    cout<<__FUNCTION__<<endl;
    error err;
	json_type j;
    tie(err, j) = json_parse("false");
    EXPECT_EQ(OK, err);
    EXPECT_EQ(json_type::BOOL, j.type);
    EXPECT_EQ(false, j.bval);
}

static void test_parse_expect_value() {
    cout<<__FUNCTION__<<endl;
    error err;
    json_type j;
    tie(err, j) = json_parse(" ");
    EXPECT_EQ(error("expect value"), err);
    tie(err, j) = json_parse("");
    EXPECT_EQ(error("expect value"), err);
}
static void TEST_ERROR(error err, string s) {
    error e;
    json_type j;
    tie(e, j) = json_parse(s);
    cout << "\ttesting '"<<s<<"'"<<endl;
    EXPECT_EQ(err, e);
}
static void test_parse_invalid_value() {
    cout<<__FUNCTION__<<endl;

    TEST_ERROR(error("invalid value"), "value");

    /* invalid number */
    TEST_ERROR(error("invalid value"), "+0");
    TEST_ERROR(error("invalid value"), "+1");
    TEST_ERROR(error("invalid value"), ".123"); /* at least one digit before '.' */
    TEST_ERROR(error("invalid value"), "INF");
    TEST_ERROR(error("invalid value"), "inf");
    TEST_ERROR(error("invalid value"), "NAN");
    TEST_ERROR(error("expect null"), "nan");

}
static void test_parse_root_not_singular() {
    cout<<__FUNCTION__<<endl;
    error err;
    json_type j;
    tie(err, j) = json_parse("{} {}");
    EXPECT_EQ(error("json complete but have rest string"), err);
}

static void TEST_NUMBER(double d, string s) {
    cout << "\ttesting '"<<s<<"'"<<endl;
    error err;
    json_type j;
    tie(err, j) = json_parse(s);
    EXPECT_EQ(OK, err);
    EXPECT_EQ(json_type::DOUBLE, j.type);
    EXPECT_EQ(d, j.dval);
}
static void test_parse_number() {
    cout<<__FUNCTION__<<endl;
    TEST_NUMBER(0.0, "0");
    TEST_NUMBER(0.0, "-0");
    TEST_NUMBER(0.0, "-0.0");
    TEST_NUMBER(1.0, "1");
    TEST_NUMBER(-1.0, "-1");
    TEST_NUMBER(1.5, "1.5");
    TEST_NUMBER(-1.5, "-1.5");
    TEST_NUMBER(3.1416, "3.1416");
    TEST_NUMBER(1E10, "1E10");
    TEST_NUMBER(1e10, "1e10");
    TEST_NUMBER(1E+10, "1E+10");
    TEST_NUMBER(1E-10, "1E-10");
    TEST_NUMBER(-1E10, "-1E10");
    TEST_NUMBER(-1e10, "-1e10");
    TEST_NUMBER(-1E+10, "-1E+10");
    TEST_NUMBER(-1E-10, "-1E-10");
    TEST_NUMBER(1.234E+10, "1.234E+10");
    TEST_NUMBER(1.234E-10, "1.234E-10");
    // TEST_NUMBER(0.0, "1e-10000"); /* must underflow */

    /* the smallest number > 1 */
    TEST_NUMBER(1.0000000000000002, "1.0000000000000002");
    /* minimum denormal */
    TEST_NUMBER( 4.9406564584124654e-324, "4.9406564584124654e-324");
    TEST_NUMBER(-4.9406564584124654e-324, "-4.9406564584124654e-324");
    /* Max subnormal double */
    TEST_NUMBER( 2.2250738585072009e-308, "2.2250738585072009e-308");
    TEST_NUMBER(-2.2250738585072009e-308, "-2.2250738585072009e-308");
    /* Min normal positive double */
    TEST_NUMBER( 2.2250738585072014e-308, "2.2250738585072014e-308");
    TEST_NUMBER(-2.2250738585072014e-308, "-2.2250738585072014e-308");
    /* Max double */
    TEST_NUMBER( 1.7976931348623157e+308, "1.7976931348623157e+308");
    TEST_NUMBER(-1.7976931348623157e+308, "-1.7976931348623157e+308");
}
void TEST_STRING(string str, string json)
{
    cout << "\ttesting '"<<json<<"'"<<endl;
    error err;
    json_type j;
    tie(err, j) = json_parse(json);
    EXPECT_EQ(OK, err);
    EXPECT_EQ(json_type::STRING, j.type);
    EXPECT_EQ(str, j.sval);
}
void test_parse_string()
{
    cout<<__FUNCTION__<<endl;
    TEST_STRING("hello world", "\"hello world\"");
    TEST_STRING("hello\tworld\n", "\"hello\\tworld\\n\"");
}
json_array TEST_ARRAY(string json)
{
    cout << "\ttesting '"<<json<<"'"<<endl;
    error err;
    json_type j;
    tie(err, j) = json_parse(json);
    EXPECT_EQ(OK, err);
    EXPECT_EQ(json_type::ARRAY, j.type);
    return j.aval;
}
void test_parse_array()
{
    cout<<__FUNCTION__<<endl;
    json_array a;
    a = TEST_ARRAY("[]");
    EXPECT_EQ((size_t)0, a.size());
    a = TEST_ARRAY("[1]");
    EXPECT_EQ((size_t)1, a.size());
    EXPECT_EQ(json_type::DOUBLE, a[0].type);
    EXPECT_EQ(1.0, a[0].dval);
}
json_object TEST_OBJECT(string json)
{
    cout << "\ttesting '"<<json<<"'"<<endl;
    error err;
    json_type j;
    tie(err, j) = json_parse(json);
    EXPECT_EQ(OK, err);
    EXPECT_EQ(json_type::OBJECT, j.type);
    return j.oval;
}
void test_parse_object()
{
    cout<<__FUNCTION__<<endl;
    json_object a;
    a = TEST_OBJECT("{}");
    EXPECT_EQ((size_t)0, a.size());
    a = TEST_OBJECT("{\"foo\": \"bar\"}");
    EXPECT_EQ((size_t)1, a.size());
    EXPECT_EQ(json_type::STRING, a["foo"].type);
    EXPECT_EQ(string("bar"), a["foo"].sval);
}
static void test_parse() {
    test_parse_null();
    test_parse_true();
    test_parse_false();
    test_parse_expect_value();
    test_parse_invalid_value();
    test_parse_root_not_singular();
    test_parse_number();
    test_parse_string();
    test_parse_array();
    test_parse_object();
}

int main(int argc, char const *argv[])
{
	test_parse();
    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}