; ModuleID = 'source.c'
source_filename = "source.c"

define i32 @foo(i32 %0) {
entry:
  %1 = alloca i32, align 4
  store i32 %0, i32* %1, align 4
  %j = load i32, i32* %1, align 4
  %ifcondition = icmp ne i32 %j, 0
  br i1 %ifcondition, label %trueBranch, label %falseBranch

trueBranch:                                       ; preds = %entry
  store i32 3, i32* %1, align 4
  br label %after-if

falseBranch:                                      ; preds = %entry
  store i32 4, i32* %1, align 4
  br label %after-if

after-if:                                         ; preds = %falseBranch, %trueBranch
  %j1 = load i32, i32* %1, align 4
  ret i32 %j1
}

define i32 @main() {
entry:
  %calltmp = call i32 @foo(i32 23)
  ret i32 %calltmp
}
