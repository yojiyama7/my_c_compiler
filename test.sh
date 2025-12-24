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

assert 0 "0"
assert 42 "42"
assert 21 "5+20-4"
assert 32 "40 - 13 + 5"
echo OK
