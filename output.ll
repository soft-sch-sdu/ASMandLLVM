; ModuleID = 'source.c'
source_filename = "source.c"

@0 = global i32 1

define void @quicksort(i32 %0, i32 %1) {
entry:
  %x = alloca i32, align 4
  %v = alloca i32, align 4
  %j = alloca i32, align 4
  %i = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  store i32 %1, ptr %3, align 4
  %n = load i32, ptr %3, align 4
  %m = load i32, ptr %2, align 4
  %cmptmp = icmp ule i32 %n, %m
  %booltmp = uitofp i1 %cmptmp to double
  %ifcondition = fcmp one double %booltmp, 0.000000e+00
  br i1 %ifcondition, label %trueBranch, label %after-if

trueBranch:                                       ; preds = %entry
  ret void

after-if:                                         ; preds = %entry
  %m1 = load i32, ptr %2, align 4
  %subtmp = sub i32 %m1, 1
  store i32 %subtmp, ptr %i, align 4
  %n2 = load i32, ptr %3, align 4
  store i32 %n2, ptr %j, align 4
  %a = load i32, ptr @0, align 4
  store i32 %a, ptr %v, align 4
  br label %while.condition

while.condition:                                  ; preds = %after-if9, %after-if
  br i1 true, label %while.body, label %after-while

while.body:                                       ; preds = %while.condition
  %i3 = load i32, ptr %i, align 4
  %j4 = load i32, ptr %j, align 4
  %cmptmp5 = icmp uge i32 %i3, %j4
  %booltmp6 = uitofp i1 %cmptmp5 to double
  %ifcondition7 = fcmp one double %booltmp6, 0.000000e+00
  br i1 %ifcondition7, label %trueBranch8, label %after-if9

after-while:                                      ; preds = %while.condition
  %a13 = load i32, ptr @0, align 4
  store i32 %a13, ptr %x, align 4
  %a14 = load i32, ptr @0, align 4
  store i32 %a14, ptr @0, align 4
  %x15 = load i32, ptr %x, align 4
  store i32 %x15, ptr @0, align 4
  %m16 = load i32, ptr %2, align 4
  %j17 = load i32, ptr %j, align 4
  %calltmp = call void @quicksort(i32 %m16, i32 %j17)
  %i18 = load i32, ptr %i, align 4
  %addtmp = add i32 %i18, 1
  %n19 = load i32, ptr %3, align 4
  %calltmp20 = call void @quicksort(i32 %addtmp, i32 %n19)

trueBranch8:                                      ; preds = %while.body
  br label %after-if9

after-if9:                                        ; preds = %trueBranch8, %while.body
  %a10 = load i32, ptr @0, align 4
  store i32 %a10, ptr %x, align 4
  %a11 = load i32, ptr @0, align 4
  store i32 %a11, ptr @0, align 4
  %x12 = load i32, ptr %x, align 4
  store i32 %x12, ptr @0, align 4
  br label %while.condition
}
