
    .text
    .global SelectionSort
SelectionSort:
    push %rbp
    mov %rsp, %rbp
    sub $48, %rsp
    mov %rdi, -8(%rbp)
    lea -16(%rbp), %rax
    push %rax
    mov $0, %rax
    pop %rdi
    mov %rax, (%rdi)
.L.condition.1:
    mov $1, %rax
    push %rax
    lea -8(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    sub %rdi, %rax
    push %rax
    lea -16(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    cmp %rdi, %rax
    setl %al
    movzb %al, %rax
    je  .L.end.1
    lea -32(%rbp), %rax
    push %rax
    lea -16(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    mov %rax, (%rdi)
    lea -24(%rbp), %rax
    push %rax
    mov $1, %rax
    push %rax
    lea -16(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    add %rdi, %rax
    pop %rdi
    mov %rax, (%rdi)
.L.condition.2:
    lea -8(%rbp), %rax
    mov (%rax), %rax
    push %rax
    lea -24(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    cmp %rdi, %rax
    setl %al
    movzb %al, %rax
    je  .L.end.2
    lea -32(%rbp), %rax
    mov (%rax), %rax
    mov %rax, %rcx
    lea arr(%rip), %rax
    mov (%rax,%rcx,8), %rax
    push %rax
    lea -24(%rbp), %rax
    mov (%rax), %rax
    mov %rax, %rcx
    lea arr(%rip), %rax
    mov (%rax,%rcx,8), %rax
    pop %rdi
    cmp %rdi, %rax
    setl %al
    movzb %al, %rax
    cmp $0, %rax
    je  .L.else.3
    lea -32(%rbp), %rax
    push %rax
    lea -24(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    mov %rax, (%rdi)
    jmp .L.end.3
.L.else.3:
.L.end.3:
    lea -24(%rbp), %rax
    push %rax
    mov $1, %rax
    push %rax
    lea -24(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    add %rdi, %rax
    pop %rdi
    mov %rax, (%rdi)
    jmp  .L.condition.2
.L.end.2:
    lea -40(%rbp), %rax
    push %rax
    lea -32(%rbp), %rax
    mov (%rax), %rax
    mov %rax, %rcx
    lea arr(%rip), %rax
    mov (%rax,%rcx,8), %rax
    pop %rdi
    mov %rax, (%rdi)
    lea -32(%rbp), %rax
    mov (%rax), %rax
    mov %rax, %rcx
    lea arr(%rip), %rax
    push %rax
    push %rcx
    lea -16(%rbp), %rax
    mov (%rax), %rax
    mov %rax, %rcx
    lea arr(%rip), %rax
    mov (%rax,%rcx,8), %rax
    pop %rcx
    pop %rdi
    mov %rax, (%rdi, %rcx, 8)
    lea -16(%rbp), %rax
    mov (%rax), %rax
    mov %rax, %rcx
    lea arr(%rip), %rax
    push %rax
    push %rcx
    lea -40(%rbp), %rax
    mov (%rax), %rax
    pop %rcx
    pop %rdi
    mov %rax, (%rdi, %rcx, 8)
    lea -16(%rbp), %rax
    push %rax
    mov $1, %rax
    push %rax
    lea -16(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    add %rdi, %rax
    pop %rdi
    mov %rax, (%rdi)
    jmp  .L.condition.1
.L.end.1:
L.SelectionSort.return:
    mov %rbp, %rsp
    pop %rbp
    ret

    .text
    .global main
main:
    push %rbp
    mov %rsp, %rbp
    sub $16, %rsp
    lea -16(%rbp), %rax
    push %rax
    mov $10, %rax
    pop %rdi
    mov %rax, (%rdi)
    mov $10, %rax
    push %rax
    pop %rdi
    mov $0, %rax
    call SelectionSort
    lea -8(%rbp), %rax
    push %rax
    mov $0, %rax
    pop %rdi
    mov %rax, (%rdi)
.L.condition.4:
    lea -16(%rbp), %rax
    mov (%rax), %rax
    push %rax
    lea -8(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    cmp %rdi, %rax
    setl %al
    movzb %al, %rax
    je  .L.end.4
    lea -8(%rbp), %rax
    mov (%rax), %rax
    mov %rax, %rcx
    lea arr(%rip), %rax
    mov (%rax,%rcx,8), %rax
    lea printf_format_int(%rip), %rdi
    mov %rax, %rsi
    call printf
    lea -8(%rbp), %rax
    push %rax
    mov $1, %rax
    push %rax
    lea -8(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    add %rdi, %rax
    pop %rdi
    mov %rax, (%rdi)
    jmp  .L.condition.4
.L.end.4:
    mov $0, %rax
    jmp L.main.return
L.main.return:
    mov %rbp, %rsp
    pop %rbp
    ret

    .data
arr:
    .quad 18
    .quad 15
    .quad 43
    .quad 32
    .quad 21
    .quad 92
    .quad 35
    .quad 23
    .quad 12
    .quad 22
printf_format_int:
    .string   "%d\n"

