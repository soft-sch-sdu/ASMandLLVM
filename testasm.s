  .text
  .global fun
fun:
  push %rbp
  mov %rsp, %rbp
  sub $32, %rsp
  movq %xmm0, -8(%rbp)
  movq %xmm1, -16(%rbp)
  mov %rdi, -24(%rbp)
  lea -24(%rbp), %rax
  mov (%rax), %rax
  cvtsi2sd %rax, %xmm0
  sub $8, %rsp
  movsd %xmm0, (%rsp)
  lea -16(%rbp), %rax
  movsd (%rax), %xmm0
  sub $8, %rsp
  movsd %xmm0, (%rsp)
  lea -8(%rbp), %rax
  movsd (%rax), %xmm0
  movsd (%rsp), %xmm1
  add $8, %rsp
  addsd %xmm1, %xmm0
  movsd (%rsp), %xmm1
  add $8, %rsp
  mulsd %xmm1, %xmm0
  jmp L.fun.return
L.fun.return:
  mov %rbp, %rsp
  pop %rbp
  ret

  .text
  .global main
main:
  push %rbp
  mov %rsp, %rbp
  mov $4617428107952285286, %rax   # float 5.1
  movq %rax, %xmm0
  sub $8, %rsp
  movsd %xmm0, (%rsp)
  mov $4610334938539176755, %rax   # float 1.7
  movq %rax, %xmm0
  sub $8, %rsp
  movsd %xmm0, (%rsp)
  mov $1, %rax
  push %rax
  pop %rdi
  movsd (%rsp), %xmm1
  add $8, %rsp
  movsd (%rsp), %xmm0
  add $8, %rsp
  mov $0, %rax
  call fun
  jmp L.main.return
L.main.return:
  lea format(%rip), %rdi
 # mov %rax, %rsi
  call printf
  mov %rbp, %rsp
  pop %rbp
  ret

  .data
 format:
  .string "%f\n"
