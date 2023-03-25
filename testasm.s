    .text
    .global foo
foo:
    push %rbp
    mov %rsp, %rbp
    sub $16, %rsp
    mov %rdi, -8(%rbp)
    mov $5, %rax
    push %rax
    lea -8(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    cmp %rdi, %rax
    setl %al
    movzb %al, %rax
    cmp $0, %rax
    je  .L.else.1
    lea -8(%rbp), %rax
    push %rax
    mov $3, %rax
    pop %rdi
    mov %rax, (%rdi)
    jmp .L.end.1
.L.else.1:
    lea -8(%rbp), %rax
    push %rax
    mov $4, %rax
    push %rax
    lea -8(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    add %rdi, %rax
    pop %rdi
    mov %rax, (%rdi)
.L.end.1:
    mov $3, %rax
    push %rax
    mov $7, %rax
    pop %rdi
    div %rdi, %rax
    lea -8(%rbp), %rax
    mov (%rax), %rax
    jmp L.foo.return
L.foo.return:
    mov %rbp, %rsp
    pop %rbp
    ret

    .text
    .global goo
goo:
    push %rbp
    mov %rsp, %rbp
    sub $32, %rsp
    mov %rdi, -8(%rbp)
    lea -16(%rbp), %rax
    push %rax
    mov $2, %rax
    pop %rdi
    mov %rax, (%rdi)
.L.condition.2:
    mov $0, %rax
    push %rax
    lea -8(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    cmp %rdi, %rax
    setg %al
    movzb %al, %rax
    cmp $0, %rax
    je  .L.end.2
    lea -8(%rbp), %rax
    push %rax
    mov $1, %rax
    push %rax
    lea -8(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    sub %rdi, %rax
    pop %rdi
    mov %rax, (%rdi)
    lea -32(%rbp), %rax
    push %rax
    mov $17, %rax
    pop %rdi
    mov %rax, (%rdi)
    lea -16(%rbp), %rax
    push %rax
    lea -32(%rbp), %rax
    mov (%rax), %rax
    push %rax
    lea -16(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    add %rdi, %rax
    pop %rdi
    mov %rax, (%rdi)
    jmp  .L.condition.2
.L.end.2:
    lea -16(%rbp), %rax
    mov (%rax), %rax
    jmp L.goo.return
L.goo.return:
    mov %rbp, %rsp
    pop %rbp
    ret

    .text
    .global main
main:
    push %rbp
    mov %rsp, %rbp
    mov $34, %rax
    push %rax
    pop %rdi
    mov $0, %rax
    call foo
    mov $3, %rax
    push %rax
    pop %rdi
    mov $0, %rax
    call goo
    jmp L.main.return
L.main.return:
    mov %rbp, %rsp
    pop %rbp
    ret

    .data
