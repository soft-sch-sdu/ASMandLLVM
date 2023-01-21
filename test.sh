#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  echo "$input" | ./2023spring - > tmp.s || exit
  clang++ -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

# test cases with int only (without floats)
assert 10 "int fun(){10;}"
assert 47 "int fun(){42 + 10 -5;}"
assert 57 "int fun(){42 + 10 --5;}"
assert 15 "int fun(){42 + 10 -5; 8+7;}"
assert 9 "int fun(){int temp = 7; int p = 9;}"
assert 20 "int fun(){int temp = 7; int a = 19+1;}"
assert 7 "int fun(){int temp=9, b = 7; }"
assert 7 "int fun(){int b = 7; b; }"
assert 16 "int fun(){int temp, b = 7; 9+b;}"
assert 20 "int fun(){int temp, b = 11;;; int c = --9+b;}"

assert 1 "int fun(){true;}"
assert 0 "int fun(){bool i=true;;bool k,s, j=false;}"
assert 21 "int fun(){bool i=true;;bool k,s, j=false;} float another() {21;}"

echo OK
