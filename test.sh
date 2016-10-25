set -x
g++ -std=c++11 -o test_pass test_pass.cpp
g++ -std=c++11 -o test_fail test_fail.cpp

./test_pass test/pass*.json
./test_fail test/fail*.json
