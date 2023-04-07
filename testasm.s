

    .text
    .global main
main:
    push %rbp
    mov %rsp, %rbp
    sub $80, %rsp
    mov $0, %rax
    imul $8, %rax
    push %rax
    lea -40(%rbp), %rax
    pop %rdi
    add %rdi, %rax
    push %rax
    mov $2, %rax
    pop %rdi
    mov %rax, (%rdi)
    mov $1, %rax
    imul $8, %rax
    push %rax
    lea -40(%rbp), %rax
    pop %rdi
    add %rdi, %rax
    push %rax
    mov $3, %rax
    pop %rdi
    mov %rax, (%rdi)
    mov $2, %rax
    imul $8, %rax
    push %rax
    lea -40(%rbp), %rax
    pop %rdi
    add %rdi, %rax
    push %rax
    mov $58, %rax
    pop %rdi
    mov %rax, (%rdi)
    mov $3, %rax
    imul $8, %rax
    push %rax
    lea -40(%rbp), %rax
    pop %rdi
    add %rdi, %rax
    push %rax
    mov $34, %rax
    pop %rdi
    mov %rax, (%rdi)
    mov $4, %rax
    imul $8, %rax
    push %rax
    lea -40(%rbp), %rax
    pop %rdi
    add %rdi, %rax
    push %rax
    mov $76, %rax
    pop %rdi
    mov %rax, (%rdi)
    lea -48(%rbp), %rax
    push %rax
    mov $5, %rax
    pop %rdi
    mov %rax, (%rdi)
    lea -56(%rbp), %rax
    push %rax
    mov $0, %rax
    pop %rdi
    mov %rax, (%rdi)
.L.condition.1:
    mov $1, %rax
    push %rax
    lea -48(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    sub %rdi, %rax
    push %rax
    lea -56(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    cmp %rdi, %rax
    setl %al
    movzb %al, %rax
    je  .L.end.1
    lea -64(%rbp), %rax
    push %rax
    mov $0, %rax
    pop %rdi
    mov %rax, (%rdi)
.L.condition.2:
    lea -56(%rbp), %rax
    mov (%rax), %rax
    push %rax
    mov $1, %rax
    push %rax
    lea -48(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    sub %rdi, %rax
    pop %rdi
    sub %rdi, %rax
    push %rax
    lea -64(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    cmp %rdi, %rax
    setl %al
    movzb %al, %rax
    je  .L.end.2
    mov $1, %rax
    push %rax
    lea -64(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    add %rdi, %rax
    imul $8, %rax
    push %rax
    lea -40(%rbp), %rax
    pop %rdi
    add %rdi, %rax
    mov (%rax), %rax
    push %rax
    lea -64(%rbp), %rax
    mov (%rax), %rax
    imul $8, %rax
    push %rax
    lea -40(%rbp), %rax
    pop %rdi
    add %rdi, %rax
    mov (%rax), %rax
    pop %rdi
    cmp %rdi, %rax
    setl %al
    movzb %al, %rax
    cmp $0, %rax
    je  .L.else.3
    lea -72(%rbp), %rax
    push %rax
    lea -64(%rbp), %rax
    mov (%rax), %rax
    imul $8, %rax
    push %rax
    lea -40(%rbp), %rax
    pop %rdi
    add %rdi, %rax
    mov (%rax), %rax
    pop %rdi
    mov %rax, (%rdi)
    lea -64(%rbp), %rax
    mov (%rax), %rax
    imul $8, %rax
    push %rax
    lea -40(%rbp), %rax
    pop %rdi
    add %rdi, %rax
    push %rax
    mov $1, %rax
    push %rax
    lea -64(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    add %rdi, %rax
    imul $8, %rax
    push %rax
    lea -40(%rbp), %rax
    pop %rdi
    add %rdi, %rax
    mov (%rax), %rax
    pop %rdi
    mov %rax, (%rdi)
    mov $1, %rax
    push %rax
    lea -64(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    add %rdi, %rax
    imul $8, %rax
    push %rax
    lea -40(%rbp), %rax
    pop %rdi
    add %rdi, %rax
    push %rax
    lea -72(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    mov %rax, (%rdi)
    jmp .L.end.3
.L.else.3:
.L.end.3:
    lea -64(%rbp), %rax
    push %rax
    mov $1, %rax
    push %rax
    lea -64(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    add %rdi, %rax
    pop %rdi
    mov %rax, (%rdi)
    jmp  .L.condition.2
.L.end.2:
    lea -56(%rbp), %rax
    push %rax
    mov $1, %rax
    push %rax
    lea -56(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    add %rdi, %rax
    pop %rdi
    mov %rax, (%rdi)
    jmp  .L.condition.1
.L.end.1:
    lea -56(%rbp), %rax
    push %rax
    mov $0, %rax
    pop %rdi
    mov %rax, (%rdi)
.L.condition.4:
    lea -48(%rbp), %rax
    mov (%rax), %rax
    push %rax
    lea -56(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    cmp %rdi, %rax
    setl %al
    movzb %al, %rax
    je  .L.end.4
    lea -56(%rbp), %rax
    mov (%rax), %rax
    imul $8, %rax
    push %rax
    lea -40(%rbp), %rax
    pop %rdi
    add %rdi, %rax
    mov (%rax), %rax
    lea printf_format_int(%rip), %rdi
    mov %rax, %rsi
    call printf
    lea -56(%rbp), %rax
    push %rax
    mov $1, %rax
    push %rax
    lea -56(%rbp), %rax
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
printf_format_int:
    .string   "%d\n"




