#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  echo "$input" | ./2023spring - > tmp.s || exit
  clang tmp.s -o tmp
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
assert 10 "int main(){return 10;}"
assert 7 "int main() {return 7;}"
assert 7 "int main() {{} { {7;} }}"
assert 47 "int main(){42 + 10 -5;}"
assert 57 "int main(){42 + 10 --5;}"
assert 15 "int main(){42 + 10 -5; 8+7;}"
assert 9 "int main(){int temp = 7; int p = 9;}"
assert 20 "int main(){int temp = 7; int a = 19+1;}"
assert 7 "int main(){int temp=9, b = 7; }"
assert 7 "int main(){int b = 7; b; }"
assert 16 "int main(){int temp, b = 7; 9+b;}"
assert 6 'int main(){int c; c =9; return c-3;}'
assert 3 'int main(){int c; -9; return 3;}'
#assert 20 "int main(int j, int k, int h){int temp, b = 11;;; int c = --9+b; return c;}"
assert 20 "int main(int j, int k, int h){int temp, b = 11;;;; return --b+9;}"
assert 20 "int main(int j, int k, int h){int temp, b = 11; int c; c= 6; return --b+9;}"
assert 1 "int main(){true;}"
assert 0 "int main(){bool i=true;;bool k,s, j=false;}"
assert 2 "int main(int kk, int h){h =2; return h;}"
assert 81 "int main(int kk, int h){int i=81;}  int another() {21;}"
assert 2 "int main(){int h =2; return h;} "
assert 11 "int main(){return 5+6;}"
assert 5 "int fun(int i, int j) {return i;} int main(){return fun(5, 1);} "
assert 7 "int fun(int i, int j) {return j;} int main(){return fun(5, 7);} "
assert 33 "int foo(int a, int b){
                           return a+b;
           }
           int main() {
                return foo(11,22);
           }"

assert 21  "int foo(int a, int b, int c, int d, int e, int f){
                  return a+b+c+d+e+f;
             }

             int main() {
                  return foo(1,2,3,4,5,6);
             }"

assert 26  " int g = 17;
             int main(){ return 9 + g; }"

assert 9  " int main(){ int i = 9; return *&i; }"
#assert 11 " int main(){ int a[5] = { 11, 12, 13, 14,15}; return a[0]; }"
#assert 29 " int main(){ int a[5] = { 11, 12, 13, 14,15};
#                        int temp = 3; a[4] = a[4] + temp;return a[0]+a[4]; }"
assert 2 " int main(){ int i = 3, j = 4; if (i>j) return 1; else return 2; }"
assert 5 " int main(){ if (3 <4) return 5; else return 2; }"
assert 5 " int main(){ int i = 3, j = 4; if (i <j) return 5; else return 2; }"
assert 2 " int main(){ int i = 3, j = 4; if (0) return 5; else return 2; }"
assert 2 " int main(){ int i = 3, j = 4; if (0.0) return 5; else return 2; }"
assert 2 " int main(){ int i = 3, j = 4; if (i - 3) return 5; else return 2; }"
assert 12 " int main(){ int i =1, sum = 0; while (i<=4) {i =i+1; if (i==2)
                                                         continue; sum =i+sum;
} return sum; }"
assert 6 " int main(){ int i =1, sum = 0; do { sum =i+sum; i = i +1; } while(i<=3); return sum; }"
assert 6 "int i =1; int main(){ i=i+5; return i; }"
assert 12 "int i =1; int main(){ int sum = 0;  for (i=0;i<4; i=i+1) sum = sum +2*i; return sum; }"
echo OK
