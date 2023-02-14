

    .text
    .global main
main:
    push %rbp
    mov %rsp, %rbp
    sub $32, %rsp
    mov $0, %rax
    imul $8, %rax
    push %rax
    lea -24(%rbp), %rax
    pop %rdi
    add %rdi, %rax
    push %rax
    mov $4612361558371493478, %rax   # float 2.3
    movq %rax, %xmm0
    pop %rdi
    movsd %xmm0, (%rdi)
    mov $1, %rax
    imul $8, %rax
    push %rax
    lea -24(%rbp), %rax
    pop %rdi
    add %rdi, %rax
    push %rax
    mov $4612136378390124954, %rax   # float 2.2
    movq %rax, %xmm0
    pop %rdi
    movsd %xmm0, (%rdi)
    mov $2, %rax
    imul $8, %rax
    push %rax
    lea -24(%rbp), %rax
    pop %rdi
    add %rdi, %rax
    push %rax
    mov $4615514078110652826, %rax   # float 3.7
    movq %rax, %xmm0
    pop %rdi
    movsd %xmm0, (%rdi)
    lea -32(%rbp), %rax
    push %rax
    mov $0, %rax
    pop %rdi
    mov %rax, (%rdi)
.L.condition.1:
    mov $3, %rax
    push %rax
    lea -32(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    cmp %rdi, %rax
    setl %al
    movzb %al, %rax
    je  .L.end.1
    lea -32(%rbp), %rax
    mov (%rax), %rax
    imul $8, %rax
    push %rax
    lea -24(%rbp), %rax
    pop %rdi
    add %rdi, %rax
    movsd (%rax), %xmm0
    lea printf_format_float, %rdi
    call printf
    lea -32(%rbp), %rax
    mov (%rax), %rax
    lea printf_format_int, %rdi
    mov %rax, %rsi
    call printf
    lea -32(%rbp), %rax
    push %rax
    mov $1, %rax
    push %rax
    lea -32(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    add %rdi, %rax
    pop %rdi
    mov %rax, (%rdi)
    jmp  .L.condition.1
.L.end.1:
L.main.return:
    mov %rbp, %rsp
    pop %rbp
    ret

    .data
printf_format_float:
    .string   "%f\n"
printf_format_int:
    .string   "%d\n"


