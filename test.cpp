
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
		cout<< "expect "<< expect<<", got "<<actual<<endl;
		main_ret = 1;
	}
	test_count++;
}

static void test_parse_null() {
	error err;
	json_type j;
    tie(err, j) = json_parse("null");
    EXPECT_EQ(OK, err);
    EXPECT_EQ(json_type::NULL_, j.type);
}

static void test_parse_true() {
    error err;
	json_type j;
    tie(err, j) = json_parse("true");
    EXPECT_EQ(OK, err);
    EXPECT_EQ(json_type::BOOL, j.type);
    EXPECT_EQ(true, j.bval);
}

static void test_parse_false() {
    error err;
	json_type j;
    tie(err, j) = json_parse("false");
    EXPECT_EQ(OK, err);
    EXPECT_EQ(json_type::BOOL, j.type);
    EXPECT_EQ(false, j.bval);
}

static void test_parse_expect_value() {
    error err;
    json_type j;
    tie(err, j) = json_parse(" ");
    EXPECT_EQ(OK, err);
    EXPECT_EQ(json_type::BOOL, j.type);
    EXPECT_EQ(false, j.bval);
}
static void test_parse() {
    test_parse_null();
    test_parse_true();
    test_parse_false();
    test_parse_expect_value();
    test_parse_invalid_value();
    test_parse_root_not_singular();
}

int main(int argc, char const *argv[])
{
	test_parse();
    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}