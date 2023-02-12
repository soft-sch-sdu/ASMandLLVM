
    .text
    .global main
main:
    push %rbp
    mov %rsp, %rbp
    sub $16, %rsp
    lea -8(%rbp), %rax
    push %rax
    mov $4614162998222441677, %rax   # float 3.1
    movq %rax, %xmm0
    pop %rdi
    movsd %xmm0, (%rdi)
    lea -16(%rbp), %rax
    push %rax
    mov $4616414798036126925, %rax   # float 4.2
    movq %rax, %xmm0
    pop %rdi
    movsd %xmm0, (%rdi)
    mov $4607632778762754458, %rax   # float 1.1
    movq %rax, %xmm0
    sub $8, %rsp
    movsd %xmm0, (%rsp)
    mov $4607632778762754458, %rax   # float 1.1
    movq %rax, %xmm0
    movsd (%rsp), %xmm1
    add $8, %rsp
    subsd %xmm1, %xmm0

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
