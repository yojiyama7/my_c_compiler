#!/bin/bash

assert() {

  expected="$1"
  input="$2"

  ./mycc "$input" > tmp.s
  cc -z noexecstack -o tmp tmp.s # -z noexecstack で何らかの警告を黙らせている
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

# make && ./mycc "\
# if ( 1 == 1 ) return 4; return 3;" > tmp.s
# cc -z noexecstack -o tmp tmp.s
# ./tmp
# echo $?

# exit

assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
assert 32 "40 - 13 + 5;"
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 6 '-2+(-2)+10;'
assert 1 '1 == 1;'
assert 1 '10 != 42;'
assert 0 "5 < 3;"
assert 1 "5 > 3;"
assert 1 "5 >= 3;"
assert 0 "5 <= 3;"
assert 1 "(0 == (0 + 3)) == 0;"
assert 3 "a = 3;"
assert 30 "a = 3; b = 5; (a + 3) * b;"
assert 42 "\
foo = 1;
z = 1;
z - foo + 42;"
assert 14 "foo = 1; bar = 2; baz = 3; xxx = 4; baz * xxx + foo * bar;"
assert 8 "return 8;"
assert 8 "return 8;\
return 4;"
assert 4 "\
if ( 1 == 1 )
  return 4;
return 3;"
assert 3 "\
if ( 1 == 0 )
  return 4;
return 3;"
echo OK
