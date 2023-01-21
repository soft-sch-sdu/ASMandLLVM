  .globl main
main:
  push %rbp
  mov %rsp, %rbp
  sub $208, %rsp
  lea -8(%rbp), %rax
  push %rax
  mov $81, %rax
  pop %rdi
  mov %rax, (%rdi)
  mov $4626632339690723738, %rax   # float 21.1
  movq %rax, %xmm0
  lea format(%rip), %rdi
  # mov %rax, %rsi
  call printf
  mov %rbp, %rsp
  pop %rbp
  ret
  
  .data
 format: 
  .string "%f\n"
