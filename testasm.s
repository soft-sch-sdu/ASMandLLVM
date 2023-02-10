    .text
    .global main
main:
    push %rbp
    mov %rsp, %rbp
    sub $16, %rsp
    lea -8(%rbp), %rax
    push %rax
    mov $4623789442425946112, %rax   # float 13.5
    movq %rax, %xmm0
    pop %rdi
    movsd %xmm0, (%rdi)
    lea -8(%rbp), %rax
    movsd (%rax), %xmm0
    jmp L.main.return
L.main.return:
    lea printf_format, %rdi
    call printf
    mov %rbp, %rsp
    pop %rbp
    ret

    .data
printf_format:
  .string   "%f\n"