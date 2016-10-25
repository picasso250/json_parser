
#include <iostream>
#include <fstream>
#include "json.h"
#include "error.h"

static bool test_fail(string s) {
    error err;
    json_type j;
    tie(err, j) = json_parse(s);
    // cout<<err<<endl;
    return err != OK;
}
static bool test_pass(string s) {
    error err;
    json_type j;
    tie(err, j) = json_parse(s);
    return err == OK;
}

int main(int argc, char const *argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        std::ifstream ifs(argv[i]);
        std::string content( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );
        // cout<<content<<endl;
    	if (!test_fail(content)) {
            cout << argv[i] << " not fail\n";
            return 1;
        }
    }
    return 0;
}