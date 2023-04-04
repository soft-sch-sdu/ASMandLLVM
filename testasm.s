    .text
    .global main
main:
    push %rbp
    mov %rsp, %rbp
    sub $48, %rsp
    mov $0, %rax
    imul $8, %rax
    push %rax
    lea -40(%rbp), %rax
    pop %rdi
    add %rdi, %rax
    push %rax
    mov $11, %rax
    pop %rdi
    mov %rax, (%rdi)
    mov $1, %rax
    imul $8, %rax
    push %rax
    lea -40(%rbp), %rax
    pop %rdi
    add %rdi, %rax
    push %rax
    mov $12, %rax
    pop %rdi
    mov %rax, (%rdi)
    mov $2, %rax
    imul $8, %rax
    push %rax
    lea -40(%rbp), %rax
    pop %rdi
    add %rdi, %rax
    push %rax
    mov $13, %rax
    pop %rdi
    mov %rax, (%rdi)
    mov $3, %rax
    imul $8, %rax
    push %rax
    lea -40(%rbp), %rax
    pop %rdi
    add %rdi, %rax
    push %rax
    mov $14, %rax
    pop %rdi
    mov %rax, (%rdi)
    mov $4, %rax
    imul $8, %rax
    push %rax
    lea -40(%rbp), %rax
    pop %rdi
    add %rdi, %rax
    push %rax
    mov $15, %rax
    pop %rdi
    mov %rax, (%rdi)
    mov $0, %rax
    imul $8, %rax
    push %rax
    lea -40(%rbp), %rax
    pop %rdi
    add %rdi, %rax
    mov (%rax), %rax
    jmp L.main.return
L.main.return:
    mov %rbp, %rsp
    pop %rbp
    ret

