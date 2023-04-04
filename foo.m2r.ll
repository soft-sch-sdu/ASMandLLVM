; ModuleID = 'output.ll'
source_filename = "source.c"

define i32 @foo(i32 %0, i32 %1) {
entry:
  %cmptemp = icmp ugt i32 %1, 0
  %booltmp = uitofp i1 %cmptemp to double
  %ifcondition = fcmp one double %booltmp, 0.000000e+00
  br i1 %ifcondition, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  br label %after-if

if.else:                                          ; preds = %entry
  br label %after-if

after-if:                                         ; preds = %if.else, %if.then
  %.0 = phi i32 [ 1, %if.then ], [ -1, %if.else ]
  ret i32 %.0
}
