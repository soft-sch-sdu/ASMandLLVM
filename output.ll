; ModuleID = 'source.c'
source_filename = "source.c"

define i32 @foo(i32 %0) {
entry:
  %1 = alloca i32, align 4
  store i32 %0, i32* %1, align 4
  %j = load i32, i32* %1, align 4
  %cmptemp = icmp ult i32 %j, 5
  %booltmp = uitofp i1 %cmptemp to double
  %ifcondition = fcmp one double %booltmp, 0.000000e+00
  br i1 %ifcondition, label %trueBranch, label %falseBranch

trueBranch:                                       ; preds = %entry
  store i32 3, i32* %1, align 4
  br label %after-if

falseBranch:                                      ; preds = %entry
  %j1 = load i32, i32* %1, align 4
  %addtmp = add i32 %j1, 4
  store i32 %addtmp, i32* %1, align 4
  br label %after-if

after-if:                                         ; preds = %falseBranch, %trueBranch
  %j2 = load i32, i32* %1, align 4
  ret i32 %j2
}

define i32 @main() {
entry:
  %calltmp = call i32 @foo(i32 23)
  ret i32 %calltmp
}
