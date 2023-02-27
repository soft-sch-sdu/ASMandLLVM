; ModuleID = 'source.c'
source_filename = "source.c"

@0 = global i32 177
@1 = global float 0x4002666660000000
@2 = global i1 false

define i32 @foo(i32 %0) {
entry:
  %1 = alloca i32, align 4
  store i32 %0, i32* %1, align 4
  %j = load i32, i32* %1, align 4
  ret i32 %j
}

define i32 @main() {
entry:
  %calltmp = call i32 @foo(i32 23)
  ret i32 %calltmp
}
