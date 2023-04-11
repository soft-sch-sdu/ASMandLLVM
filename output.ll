; ModuleID = 'source.c'
source_filename = "source.c"

@b = global [4 x i32] [i32 28, i32 25, i32 22, i32 17]

define i32 @main() {
entry:
  %buf = alloca i32, align 4
  %k = alloca i32, align 4
  %j = alloca i32, align 4
  %i = alloca i32, align 4
  %n = alloca i32, align 4
  %0 = alloca [4 x i32], align 4
  %1 = getelementptr i32, ptr %0, i32 0
  store i32 18, ptr %1, align 4
  %2 = getelementptr i32, ptr %0, i32 1
  store i32 15, ptr %2, align 4
  %3 = getelementptr i32, ptr %0, i32 2
  store i32 2, ptr %3, align 4
  %4 = getelementptr i32, ptr %0, i32 3
  store i32 21, ptr %4, align 4
  store i32 4, ptr %n, align 4
  store i32 0, ptr %i, align 4
  br label %while.condition

while.condition:                                  ; preds = %after-while5, %entry
  %i1 = load i32, ptr %i, align 4
  %n2 = load i32, ptr %n, align 4
  %subtmp = sub i32 %n2, 1
  %cmptemp = icmp ult i32 %i1, %subtmp
  %booltmp = uitofp i1 %cmptemp to double
  %whilecondition = fcmp one double %booltmp, 0.000000e+00
  br i1 %whilecondition, label %while.body, label %after-while

while.body:                                       ; preds = %while.condition
  store i32 0, ptr %j, align 4
  br label %while.condition3

after-while:                                      ; preds = %while.condition
  %5 = getelementptr i32, ptr %0, i32 0
  %6 = load i32, ptr %5, align 4
  %7 = getelementptr i32, ptr %0, i32 1
  %8 = load i32, ptr %7, align 4
  %cmptemp29 = icmp ult i32 %6, %8
  %booltmp30 = uitofp i1 %cmptemp29 to double
  %ifcondition31 = fcmp one double %booltmp30, 0.000000e+00
  br i1 %ifcondition31, label %if.then, label %if.else

while.condition3:                                 ; preds = %after-if, %while.body
  %j6 = load i32, ptr %j, align 4
  %n7 = load i32, ptr %n, align 4
  %subtmp8 = sub i32 %n7, 1
  %i9 = load i32, ptr %i, align 4
  %subtmp10 = sub i32 %subtmp8, %i9
  %cmptemp11 = icmp ult i32 %j6, %subtmp10
  %booltmp12 = uitofp i1 %cmptemp11 to double
  %whilecondition13 = fcmp one double %booltmp12, 0.000000e+00
  br i1 %whilecondition13, label %while.body4, label %after-while5

while.body4:                                      ; preds = %while.condition3
  %j14 = load i32, ptr %j, align 4
  %9 = getelementptr i32, ptr %0, i32 %j14
  %10 = load i32, ptr %9, align 4
  %j15 = load i32, ptr %j, align 4
  %addtmp = add i32 %j15, 1
  %11 = getelementptr i32, ptr %0, i32 %addtmp
  %12 = load i32, ptr %11, align 4
  %cmptemp16 = icmp ugt i32 %10, %12
  %booltmp17 = uitofp i1 %cmptemp16 to double
  %ifcondition = fcmp one double %booltmp17, 0.000000e+00
  br i1 %ifcondition, label %trueBranch, label %after-if

after-while5:                                     ; preds = %while.condition3
  %i27 = load i32, ptr %i, align 4
  %addtmp28 = add i32 %i27, 1
  store i32 %addtmp28, ptr %i, align 4
  br label %while.condition

trueBranch:                                       ; preds = %while.body4
  %j18 = load i32, ptr %j, align 4
  %13 = getelementptr i32, ptr %0, i32 %j18
  %14 = load i32, ptr %13, align 4
  store i32 %14, ptr %buf, align 4
  %j19 = load i32, ptr %j, align 4
  %15 = getelementptr i32, ptr %0, i32 %j19
  %j20 = load i32, ptr %j, align 4
  %addtmp21 = add i32 %j20, 1
  %16 = getelementptr i32, ptr %0, i32 %addtmp21
  %17 = load i32, ptr %16, align 4
  store i32 %17, ptr %15, align 4
  %j22 = load i32, ptr %j, align 4
  %addtmp23 = add i32 %j22, 1
  %18 = getelementptr i32, ptr %0, i32 %addtmp23
  %buf24 = load i32, ptr %buf, align 4
  store i32 %buf24, ptr %18, align 4
  br label %after-if

after-if:                                         ; preds = %trueBranch, %while.body4
  %j25 = load i32, ptr %j, align 4
  %addtmp26 = add i32 %j25, 1
  store i32 %addtmp26, ptr %j, align 4
  br label %while.condition3

if.then:                                          ; preds = %after-while
  ret i32 5

if.else:                                          ; preds = %after-while
  ret i32 2
}
