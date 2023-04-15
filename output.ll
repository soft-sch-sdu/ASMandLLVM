; ModuleID = 'source.c'
source_filename = "source.c"

@a = global [6 x float] [float 0x40321999A0000000, float 0x402E9999A0000000, float 0x4002666660000000, float 0x40351999A0000000, float 0x4040B33340000000, float 0x4000CCCCC0000000]
@0 = private unnamed_addr constant [7 x i8] c"%0.2f\0A\00", align 1

define i32 @main() {
entry:
  %buf = alloca float, align 4
  %k = alloca i32, align 4
  %j = alloca i32, align 4
  %i = alloca i32, align 4
  %n = alloca i32, align 4
  store i32 6, ptr %n, align 4
  store i32 0, ptr %i, align 4
  br label %for.condition

for.condition:                                    ; preds = %after.for5, %entry
  %i1 = load i32, ptr %i, align 4
  %n2 = load i32, ptr %n, align 4
  %subtmp = sub i32 %n2, 1
  %cmptemp = icmp ult i32 %i1, %subtmp
  %booltmp = uitofp i1 %cmptemp to double
  %forcondition = fcmp one double %booltmp, 0.000000e+00
  br i1 %forcondition, label %for.body, label %after.for

for.body:                                         ; preds = %for.condition
  store i32 0, ptr %j, align 4
  br label %for.condition3

after.for:                                        ; preds = %for.condition
  store i32 0, ptr %i, align 4
  br label %for.condition29

for.condition3:                                   ; preds = %after-if, %for.body
  %j6 = load i32, ptr %j, align 4
  %n7 = load i32, ptr %n, align 4
  %subtmp8 = sub i32 %n7, 1
  %i9 = load i32, ptr %i, align 4
  %subtmp10 = sub i32 %subtmp8, %i9
  %cmptemp11 = icmp ult i32 %j6, %subtmp10
  %booltmp12 = uitofp i1 %cmptemp11 to double
  %forcondition13 = fcmp one double %booltmp12, 0.000000e+00
  br i1 %forcondition13, label %for.body4, label %after.for5

for.body4:                                        ; preds = %for.condition3
  %j14 = load i32, ptr %j, align 4
  %0 = getelementptr float, ptr @a, i32 %j14
  %1 = load float, ptr %0, align 4
  %j15 = load i32, ptr %j, align 4
  %addtmp = add i32 %j15, 1
  %2 = getelementptr float, ptr @a, i32 %addtmp
  %3 = load float, ptr %2, align 4
  %cmptemp16 = fcmp ult float %1, %3
  %booltmp17 = uitofp i1 %cmptemp16 to double
  %ifcondition = fcmp one double %booltmp17, 0.000000e+00
  br i1 %ifcondition, label %trueBranch, label %after-if

after.for5:                                       ; preds = %for.condition3
  %i27 = load i32, ptr %i, align 4
  %addtmp28 = add i32 %i27, 1
  store i32 %addtmp28, ptr %i, align 4
  br label %for.condition

trueBranch:                                       ; preds = %for.body4
  %j18 = load i32, ptr %j, align 4
  %4 = getelementptr float, ptr @a, i32 %j18
  %5 = load float, ptr %4, align 4
  store float %5, ptr %buf, align 4
  %j19 = load i32, ptr %j, align 4
  %addtmp20 = add i32 %j19, 1
  %6 = getelementptr float, ptr @a, i32 %addtmp20
  %7 = load float, ptr %6, align 4
  %j21 = load i32, ptr %j, align 4
  %8 = getelementptr float, ptr @a, i32 %j21
  store float %7, ptr %8, align 4
  %buf22 = load float, ptr %buf, align 4
  %j23 = load i32, ptr %j, align 4
  %addtmp24 = add i32 %j23, 1
  %9 = getelementptr float, ptr @a, i32 %addtmp24
  store float %buf22, ptr %9, align 4
  br label %after-if

after-if:                                         ; preds = %trueBranch, %for.body4
  %j25 = load i32, ptr %j, align 4
  %addtmp26 = add i32 %j25, 1
  store i32 %addtmp26, ptr %j, align 4
  br label %for.condition3

for.condition29:                                  ; preds = %for.body30, %after.for
  %i32 = load i32, ptr %i, align 4
  %n33 = load i32, ptr %n, align 4
  %cmptemp34 = icmp ult i32 %i32, %n33
  %booltmp35 = uitofp i1 %cmptemp34 to double
  %forcondition36 = fcmp one double %booltmp35, 0.000000e+00
  br i1 %forcondition36, label %for.body30, label %after.for31

for.body30:                                       ; preds = %for.condition29
  %i37 = load i32, ptr %i, align 4
  %10 = getelementptr float, ptr @a, i32 %i37
  %11 = load float, ptr %10, align 4
  %12 = fpext float %11 to double
  %13 = call i32 (ptr, ...) @printf(ptr @0, double %12)
  %i38 = load i32, ptr %i, align 4
  %addtmp39 = add i32 %i38, 1
  store i32 %addtmp39, ptr %i, align 4
  br label %for.condition29

after.for31:                                      ; preds = %for.condition29
  ret i32 0
}

declare i32 @printf(ptr, ...)
