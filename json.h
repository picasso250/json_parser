#ifndef JSON_H__
#define JSON_H__

#include <map>
#include <vector>
#include <utility>
#include <string>
#include "error.h"

using namespace std;

struct json_type;
typedef map<string, json_type> json_object;
typedef vector<json_type> json_array;
struct json_type
{
	double dval;
	bool bval;
	string sval;
	json_array aval;
	json_object oval;
// public:
	enum{ NULL_, DOUBLE, BOOL, STRING, OBJECT, ARRAY } type;
	json_type();
	json_type(bool);
	json_type(double);
	json_type(string);
	json_type(json_array);
	json_type(json_object);
};

json_type::json_type():
		type(json_type::NULL_)
		{}
json_type::json_type(bool b):
		type(json_type::BOOL),bval(b)
		{}
json_type::json_type(double d):
		type(json_type::DOUBLE),dval(d)
		{}
json_type::json_type(string s):
		type(json_type::STRING),sval(s)
		{}
json_type::json_type(json_array a):
		type(json_type::ARRAY),aval(a)
		{}
json_type::json_type(json_object o):
		type(json_type::OBJECT),oval(o)
		{}

tuple<error, json_type, string> _json_parse_(const string str);
tuple<error, json_array, string> _eat_array(const string str);
tuple<error, json_object, string> _eat_object(const string str);
tuple<error, string, string> _eat_string(const string str);

string _eat_empty(const string str)
{
	string rest;
	int i;
	for (i = 0; i < str.size(); ++i)
	{
		if (str[i] == ' ' || str[i] == '\t' || str[i] == '\n' || str[i] == '\r') {
		} else {
			break;
		}
	}
	return str.substr(i);
}
pair<error, string> _eat(const string str, const string shouldbe)
{
	error err;
	string rest;
	size_t i;
	for (i = 0; i < shouldbe.size(); ++i)
	{
		if (i < str.size() && str[i] == shouldbe[i]) {
		} else {
			return make_pair("value not match", rest);
		}
	}
	rest = str.substr(i);
	return make_pair(OK, rest);
}
tuple<error, json_array, string> _eat_array(const string str)
{
	error err;
	vector<json_type> res;
	string rest = str.substr(1);
	rest = _eat_empty(rest);
	if (rest.empty()) {
		return make_tuple("open '[' not close", res, rest);
	}
	if (rest[0] == ']') {
		rest = rest.substr(1);
		return make_tuple(OK, res, rest);
	}
	while (1) {
		rest = _eat_empty(rest);
		json_type j;
		tie(err, j, rest) = _json_parse_(rest);
		if (err != OK) {
			return make_tuple(err, res, rest);
		}
		res.push_back(j);
		rest = _eat_empty(rest);
		if (rest.empty()) {
			return make_tuple("expect ']'", res, rest);
		}
		if (rest[0] == ']') {
			rest = rest.substr(1);
			return make_tuple(OK, res, rest);
		} else if (rest[0] == ',') {
			rest = rest.substr(1);
			continue;
		} else {
			return make_tuple("expect ',' or ']'", res, rest);
		}
	}
	return make_tuple(err, res, rest);
}
tuple<error, json_object, string> _eat_object(const string str)
{
	error err;
	json_object res;
	string rest = str.substr(1); // eat '{'
	if (rest.empty()) {
		return make_tuple("open '{' not close", res, rest);
	}
	// cout<<"open object"<<endl;
	rest = _eat_empty(rest);
	if (rest[0] == '}') {
		rest = rest.substr(1);
		// cout<<"close object now!"<<endl;
		return make_tuple(OK, res, rest);
	}
	while (1) {
		rest = _eat_empty(rest);
		string key;
		json_type value;
		if (rest.empty()) {
			return make_tuple("expect a string", res, rest);
		}
		if (rest[0] != '"') {
			return make_tuple("expect key string \"", res, rest);
		}
		auto sr = _eat_string(rest);
		err = get<0>(sr);
		if (err != OK) {
			return make_tuple(err, res, rest);
		}
		key = get<1>(sr);
		rest = get<2>(sr);
		rest = _eat_empty(rest);
		if (rest.empty()) {
			return make_tuple("expect ':'", res, rest);
		}
		if (rest[0] != ':') {
			return make_tuple("expect ':'", res, rest);
		}
		rest = rest.substr(1); // eat ':'
		rest = _eat_empty(rest);
		tie(err, value, rest) = _json_parse_(rest);
		if (err != OK) {
			return make_tuple(err, res, rest);
		}
		res[key] = value;
		// cout<<"found key : "<<key<<endl;
		rest = _eat_empty(rest);
		if (rest.empty())
		{
			return make_tuple("expect '}' or ','", res, rest);
		}
		if (rest[0] == ',') {
			rest = rest.substr(1);
			continue;
		} else if (rest[0] == '}') {
			rest = rest.substr(1);
			// cout<<"object close"<<endl;
			return make_tuple(OK, res, rest);
		}
	}
	return make_tuple(err, res, rest);
}
pair<error, unsigned long> _utf8(const string str)
{
	error err;
	unsigned long code_point;
	size_t size;
	code_point = stol(str, &size, 16);
	if (size != str.size())
	{
		return make_pair("utf8 surrogate not vaild", code_point);
	}
	return make_pair(OK, code_point);
}
pair<error, string> _utf8_str(unsigned long c)
{
	error err;
	string str;
	if (0 <= c && c <= 0x7f)
	{
		str += c;
		return make_pair(OK, str);
	} else if (0x80 <= c && c <= 0x07ff) {
		char c1 = 0xC0 | ((c >> 6) & 0x3f);
		char c2 = 0x80 | (c & 0x3f);
		str += c1;
		str += c2;
		return make_pair(OK, str);
	} else if (0x0800 <= c && c <= 0xFFFF) {
		char c1 = 0xE0 | ((c >> 12) & 0x3f);
		char c2 = 0x80 | ((c >> 6) & 0x3f);
		char c3 = 0x80 | (c & 0x3f);
		str += c1;
		str += c2;
		str += c3;
		return make_pair(OK, str);
	} else if (0x10000 <= c && c <= 0x10FFFF) {
		char c1 = 0xF0 | ((c >> 18) & 0x3f);
		char c2 = 0x80 | ((c >> 12) & 0x3f);
		char c3 = 0x80 | ((c >> 6) & 0x3f);
		char c4 = 0x80 | (c & 0x3f);
		str += c1;
		str += c2;
		str += c3;
		str += c4;
		return make_pair(OK, str);
	}
	return make_pair("unvalid code point", str);
}
tuple<error, string, string> _eat_string(const string str)
{
	error err;
	string res;
	string rest = str.substr(1);
	map<char, char> m = {
		{'"', '"'},  //    quotation mark  U+0022
		{'\\', '\\'}, //    reverse solidus U+005C
		{'/', '/'},  //    solidus         U+002F
		{'b', '\b'}, //    backspace       U+0008
		{'f', '\f'}, //    form feed       U+000C
		{'n', '\n'}, //    line feed       U+000A
		{'r', '\r'}, //    carriage return U+000D
		{'t', '\t'}, //    tab             U+0009
	};
	int i = 0;
	while (i < rest.size())
	{
		char c = rest[i];
		if (c == '\\') {
			if (i + 1 < rest.size()) {
				i++;
				char next = rest[i];
				if (next == 'u') {
					i++;
					if (i + 4 < rest.size()) {
						string cs1(rest, i, 4);
						unsigned long c;
						tie(err, c) = _utf8(cs1);
						if (err != OK) return make_tuple(err, res, rest);
						i += 4;
						if (0xD800 <= c && c <= 0xDBFF) {
							if (i+6 < rest.size()) {
								if (rest[i] != '\\') {
									return make_tuple("invalid surrogate", res, rest);
								}
								i++;
								if (rest[i] != 'u') {
									return make_tuple("invalid surrogate", res, rest);
								}
								i++;
								string cs2(rest, i, 4);
								auto cc2 = _utf8(cs1);
								err = cc2.first;
								if (err != OK) return make_tuple(err, res, rest);
								unsigned long c2 = cc2.second;
								c = 0x10000+(c-0xD800)*0x400+(c2-0xdc00);
							}else{
								return make_tuple("invalid surrogate", res, rest);
							}
						}
						auto ur = _utf8_str(c);
						err = ur.first;
						if (err != OK) {
							return make_tuple(err, res, rest);
						}
						res += ur.second;
					} else {
						return make_tuple("expect utf8", res, rest);
					}
				} else {
					auto it = m.find(next);
					if (it != m.end()) {
						res += it->second;
					} else {
						return make_tuple("'\\' followed by unkown escape", res, rest);
					}
				}
			} else {
				return make_tuple("'\\' not followed", res, rest);
			}
		} else if (c == '"') {
			return make_tuple(OK, res, rest.substr(i+1));
		} else {
			res += c;
		}
		++i;
	}
}

tuple<error, json_type, string> _json_parse_(const string str)
{
	error err;
	json_type j;
	string rest;
	rest = _eat_empty(str);
	if (rest.empty()) {
		return make_tuple("expect value", j, rest);
	}
	int pos;
	if (rest[0] == 'n') {
		auto r = _eat(rest, "null");
		if (r.first != OK) {
			return make_tuple("expect null", j, rest);
		} else {
			return make_tuple(OK, j, r.second);
		}
	} else if (rest[0] == 'f') {
		auto r = _eat(rest, "false");
		if (r.first != OK) {
			return make_tuple("expect false", j, rest);
		} else {
			j = json_type(false);
			return make_tuple(OK, j, r.second);
		}
	} else if (rest[0] == 't') {
		auto r = _eat(rest, "true");
		if (r.first != OK) {
			return make_tuple("expect true", j, rest);
		} else {
			j = json_type(true);
			return make_tuple(OK, j, r.second);
		}
	} else if (rest[0] == '-' || isdigit(rest[0])) {
		size_t endpos = 0;
		double d = stod(rest, &endpos);
		// if (endpos != rest.size())
		// {
		// 	return make_tuple("expect digit", j, rest.substr(endpos));
		// }
		j = json_type(d);
		rest = rest.substr(endpos);
		return make_tuple(OK, j, rest);
	} else if (rest[0] == '"') {
		auto rrr = _eat_string(rest);
		err = get<0>(rrr);
		if (err == OK)
		{
			j = json_type(get<1>(rrr));
		}
		return make_tuple(OK, j, get<2>(rrr));
	} else if (rest[0] == '[') {
		json_array a;
		tie(err, a, rest) = _eat_array(rest);
		j = json_type(a);
		return make_tuple(err, j, rest);
	} else if (rest[0] == '{') {
		json_object o;
		tie(err, o, rest) = _eat_object(rest);
		j = json_type(o);
		return make_tuple(err, j, rest);
	} else {
		return make_tuple("invalid value", j, rest);
	}
}

pair<error, json_type> json_parse(const string str)
{
	error err;
	json_type j;
	string rest;
	tie(err, j, rest) = _json_parse_(str);
	if (err != OK) {
		return make_pair(err, j);
	}
	if (j.type != json_type::ARRAY && j.type != json_type::OBJECT)
	{
		return make_pair("json not array or object", j);
	}
	rest = _eat_empty(rest);
	if (rest.empty())
	{
		return make_pair(OK, j);
	}
	return make_pair("json complete but have rest string", j);
}

#endif
