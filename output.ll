; ModuleID = 'source.c'
source_filename = "source.c"

@arr = global [10 x float] [float 0x404A0CCCC0000000, float 0x4014666660000000, float 0x40410CCCC0000000, float 0x4018666660000000, float 0x40371999A0000000, float 0x401C666660000000, float 0x4000CCCCC0000000, float 0x40430CCCC0000000, float 0x4010666660000000, float 0x40301999A0000000]
@0 = private unnamed_addr constant [8 x i8] c"%0.2f  \00", align 1

define void @SelectionSort(i32 %0) {
entry:
  %tmp = alloca float, align 4
  %k = alloca i32, align 4
  %j = alloca i32, align 4
  %i = alloca i32, align 4
  %1 = alloca i32, align 4
  store i32 %0, ptr %1, align 4
  store i32 0, ptr %i, align 4
  br label %for.condition

for.condition:                                    ; preds = %after.for5, %entry
  %i1 = load i32, ptr %i, align 4
  %size = load i32, ptr %1, align 4
  %subtmp = sub i32 %size, 1
  %booltmp = icmp ult i32 %i1, %subtmp
  %forcondition = icmp eq i1 %booltmp, true
  br i1 %forcondition, label %for.body, label %after.for

for.body:                                         ; preds = %for.condition
  %i2 = load i32, ptr %i, align 4
  store i32 %i2, ptr %k, align 4
  %i6 = load i32, ptr %i, align 4
  %addtmp = add i32 %i6, 1
  store i32 %addtmp, ptr %j, align 4
  br label %for.condition3

after.for:                                        ; preds = %for.condition
  ret void

for.condition3:                                   ; preds = %after-if, %for.body
  %j7 = load i32, ptr %j, align 4
  %size8 = load i32, ptr %1, align 4
  %booltmp9 = icmp ult i32 %j7, %size8
  %forcondition10 = icmp eq i1 %booltmp9, true
  br i1 %forcondition10, label %for.body4, label %after.for5

for.body4:                                        ; preds = %for.condition3
  %j11 = load i32, ptr %j, align 4
  %2 = getelementptr float, ptr @arr, i32 %j11
  %3 = load float, ptr %2, align 4
  %k12 = load i32, ptr %k, align 4
  %4 = getelementptr float, ptr @arr, i32 %k12
  %5 = load float, ptr %4, align 4
  %booltmp13 = fcmp ult float %3, %5
  %ifcondition = icmp eq i1 %booltmp13, true
  br i1 %ifcondition, label %then-block, label %after-if

after.for5:                                       ; preds = %for.condition3
  %k17 = load i32, ptr %k, align 4
  %6 = getelementptr float, ptr @arr, i32 %k17
  %7 = load float, ptr %6, align 4
  store float %7, ptr %tmp, align 4
  %i18 = load i32, ptr %i, align 4
  %8 = getelementptr float, ptr @arr, i32 %i18
  %9 = load float, ptr %8, align 4
  %k19 = load i32, ptr %k, align 4
  %10 = getelementptr float, ptr @arr, i32 %k19
  store float %9, ptr %10, align 4
  %tmp20 = load float, ptr %tmp, align 4
  %i21 = load i32, ptr %i, align 4
  %11 = getelementptr float, ptr @arr, i32 %i21
  store float %tmp20, ptr %11, align 4
  %i22 = load i32, ptr %i, align 4
  %addtmp23 = add i32 %i22, 1
  store i32 %addtmp23, ptr %i, align 4
  br label %for.condition

after-if:                                         ; preds = %for.body4, %then-block
  %j15 = load i32, ptr %j, align 4
  %addtmp16 = add i32 %j15, 1
  store i32 %addtmp16, ptr %j, align 4
  br label %for.condition3

then-block:                                       ; preds = %for.body4
  %j14 = load i32, ptr %j, align 4
  store i32 %j14, ptr %k, align 4
  br label %after-if
}

define void @InsertionSort(i32 %0) {
entry:
  %tmp = alloca float, align 4
  %j = alloca i32, align 4
  %i = alloca i32, align 4
  %1 = alloca i32, align 4
  store i32 %0, ptr %1, align 4
  store i32 1, ptr %i, align 4
  br label %for.condition

for.condition:                                    ; preds = %after-if, %entry
  %i1 = load i32, ptr %i, align 4
  %size = load i32, ptr %1, align 4
  %booltmp = icmp ult i32 %i1, %size
  %forcondition = icmp eq i1 %booltmp, true
  br i1 %forcondition, label %for.body, label %after.for

for.body:                                         ; preds = %for.condition
  %i2 = load i32, ptr %i, align 4
  %2 = getelementptr float, ptr @arr, i32 %i2
  %3 = load float, ptr %2, align 4
  %i3 = load i32, ptr %i, align 4
  %subtmp = sub i32 %i3, 1
  %4 = getelementptr float, ptr @arr, i32 %subtmp
  %5 = load float, ptr %4, align 4
  %booltmp4 = fcmp ult float %3, %5
  %ifcondition = icmp eq i1 %booltmp4, true
  br i1 %ifcondition, label %then-block, label %after-if

after.for:                                        ; preds = %for.condition
  ret void

after-if:                                         ; preds = %for.body, %after.for8
  %i25 = load i32, ptr %i, align 4
  %addtmp26 = add i32 %i25, 1
  store i32 %addtmp26, ptr %i, align 4
  br label %for.condition

then-block:                                       ; preds = %for.body
  %i5 = load i32, ptr %i, align 4
  %6 = getelementptr float, ptr @arr, i32 %i5
  %7 = load float, ptr %6, align 4
  store float %7, ptr %tmp, align 4
  %i9 = load i32, ptr %i, align 4
  %subtmp10 = sub i32 %i9, 1
  store i32 %subtmp10, ptr %j, align 4
  br label %for.condition6

for.condition6:                                   ; preds = %for.body7, %then-block
  %j11 = load i32, ptr %j, align 4
  %booltmp12 = icmp uge i32 %j11, 0
  %j13 = load i32, ptr %j, align 4
  %8 = getelementptr float, ptr @arr, i32 %j13
  %9 = load float, ptr %8, align 4
  %tmp14 = load float, ptr %tmp, align 4
  %booltmp15 = fcmp ugt float %9, %tmp14
  %booltmp16 = and i1 %booltmp12, %booltmp15
  %forcondition17 = icmp eq i1 %booltmp16, true
  br i1 %forcondition17, label %for.body7, label %after.for8

for.body7:                                        ; preds = %for.condition6
  %j18 = load i32, ptr %j, align 4
  %10 = getelementptr float, ptr @arr, i32 %j18
  %11 = load float, ptr %10, align 4
  %j19 = load i32, ptr %j, align 4
  %addtmp = add i32 %j19, 1
  %12 = getelementptr float, ptr @arr, i32 %addtmp
  store float %11, ptr %12, align 4
  %j20 = load i32, ptr %j, align 4
  %subtmp21 = sub i32 %j20, 1
  store i32 %subtmp21, ptr %j, align 4
  br label %for.condition6

after.for8:                                       ; preds = %for.condition6
  %tmp22 = load float, ptr %tmp, align 4
  %j23 = load i32, ptr %j, align 4
  %addtmp24 = add i32 %j23, 1
  %13 = getelementptr float, ptr @arr, i32 %addtmp24
  store float %tmp22, ptr %13, align 4
  br label %after-if
}

define void @BubbleSort(i32 %0) {
entry:
  %tmp = alloca float, align 4
  %j = alloca i32, align 4
  %i = alloca i32, align 4
  %1 = alloca i32, align 4
  store i32 %0, ptr %1, align 4
  store i32 0, ptr %i, align 4
  br label %for.condition

for.condition:                                    ; preds = %after.for4, %entry
  %i1 = load i32, ptr %i, align 4
  %size = load i32, ptr %1, align 4
  %subtmp = sub i32 %size, 1
  %booltmp = icmp ult i32 %i1, %subtmp
  %forcondition = icmp eq i1 %booltmp, true
  br i1 %forcondition, label %for.body, label %after.for

for.body:                                         ; preds = %for.condition
  store i32 0, ptr %j, align 4
  br label %for.condition2

after.for:                                        ; preds = %for.condition
  ret void

for.condition2:                                   ; preds = %after-if, %for.body
  %j5 = load i32, ptr %j, align 4
  %size6 = load i32, ptr %1, align 4
  %i7 = load i32, ptr %i, align 4
  %subtmp8 = sub i32 %size6, %i7
  %subtmp9 = sub i32 %subtmp8, 1
  %booltmp10 = icmp ult i32 %j5, %subtmp9
  %forcondition11 = icmp eq i1 %booltmp10, true
  br i1 %forcondition11, label %for.body3, label %after.for4

for.body3:                                        ; preds = %for.condition2
  %j12 = load i32, ptr %j, align 4
  %2 = getelementptr float, ptr @arr, i32 %j12
  %3 = load float, ptr %2, align 4
  %j13 = load i32, ptr %j, align 4
  %addtmp = add i32 %j13, 1
  %4 = getelementptr float, ptr @arr, i32 %addtmp
  %5 = load float, ptr %4, align 4
  %booltmp14 = fcmp ugt float %3, %5
  %ifcondition = icmp eq i1 %booltmp14, true
  br i1 %ifcondition, label %then-block, label %after-if

after.for4:                                       ; preds = %for.condition2
  %i24 = load i32, ptr %i, align 4
  %addtmp25 = add i32 %i24, 1
  store i32 %addtmp25, ptr %i, align 4
  br label %for.condition

after-if:                                         ; preds = %for.body3, %then-block
  %j22 = load i32, ptr %j, align 4
  %addtmp23 = add i32 %j22, 1
  store i32 %addtmp23, ptr %j, align 4
  br label %for.condition2

then-block:                                       ; preds = %for.body3
  %j15 = load i32, ptr %j, align 4
  %6 = getelementptr float, ptr @arr, i32 %j15
  %7 = load float, ptr %6, align 4
  store float %7, ptr %tmp, align 4
  %j16 = load i32, ptr %j, align 4
  %addtmp17 = add i32 %j16, 1
  %8 = getelementptr float, ptr @arr, i32 %addtmp17
  %9 = load float, ptr %8, align 4
  %j18 = load i32, ptr %j, align 4
  %10 = getelementptr float, ptr @arr, i32 %j18
  store float %9, ptr %10, align 4
  %tmp19 = load float, ptr %tmp, align 4
  %j20 = load i32, ptr %j, align 4
  %addtmp21 = add i32 %j20, 1
  %11 = getelementptr float, ptr @arr, i32 %addtmp21
  store float %tmp19, ptr %11, align 4
  br label %after-if
}

define i32 @main() {
entry:
  %count = alloca i32, align 4
  %i = alloca i32, align 4
  store i32 10, ptr %count, align 4
  %count1 = load i32, ptr %count, align 4
  call void @BubbleSort(i32 %count1)
  store i32 0, ptr %i, align 4
  br label %for.condition

for.condition:                                    ; preds = %for.body, %entry
  %i2 = load i32, ptr %i, align 4
  %count3 = load i32, ptr %count, align 4
  %booltmp = icmp ult i32 %i2, %count3
  %forcondition = icmp eq i1 %booltmp, true
  br i1 %forcondition, label %for.body, label %after.for

for.body:                                         ; preds = %for.condition
  %i4 = load i32, ptr %i, align 4
  %0 = getelementptr float, ptr @arr, i32 %i4
  %1 = load float, ptr %0, align 4
  %2 = fpext float %1 to double
  %3 = call i32 (ptr, ...) @printf(ptr @0, double %2)
  %i5 = load i32, ptr %i, align 4
  %addtmp = add i32 %i5, 1
  store i32 %addtmp, ptr %i, align 4
  br label %for.condition

after.for:                                        ; preds = %for.condition
  ret i32 0
}

declare i32 @printf(ptr, ...)
