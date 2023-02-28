; ModuleID = 'source.c'
source_filename = "source.c"

define i32 @foo(i32 %0) {
entry:
  %j = alloca i32, align 4
  %kkkk = alloca float, align 4
  %g = alloca i32, align 4
  %1 = alloca i32, align 4
  store i32 %0, i32* %1, align 4
  store i32 8, i32* %g, align 4
  store float 0x4021666660000000, float* %kkkk, align 4
  store i32 9, i32* %j, align 4
  %j1 = load i32, i32* %j, align 4
  ret i32 %j1
}

define i32 @main() {
entry:
  %calltmp = call i32 @foo(i32 23)
  ret i32 %calltmp
}
