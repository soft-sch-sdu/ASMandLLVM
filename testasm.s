

    .text
    .global quicksort
quicksort:
    push %rbp
    mov %rsp, %rbp
    sub $48, %rsp
    mov %rdi, -8(%rbp)
    mov %rsi, -16(%rbp)
    lea -8(%rbp), %rax
    mov (%rax), %rax
    push %rax
    lea -16(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    cmp %rdi, %rax
    setle %al
    movzb %al, %rax
    cmp $0, %rax
    je  .L.else.1
    jmp L.quicksort.return
    jmp .L.end.1
.L.else.1:
.L.end.1:
    lea -24(%rbp), %rax
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
    lea -16(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    mov %rax, (%rdi)
    lea -40(%rbp), %rax
    push %rax
    lea -16(%rbp), %rax
    mov (%rax), %rax
    mov %rax, %rcx
    lea a(%rip), %rax
    movsd (%rax,%rcx,8), %xmm0
    pop %rdi
    movsd %xmm0, (%rdi)
.L.condition.2:
    mov $1, %rax
    cmp $0, %rax
    je  .L.end.2
.L.do.3:
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
    lea -40(%rbp), %rax
    movsd (%rax), %xmm0
    sub $8, %rsp
    movsd %xmm0, (%rsp)
    lea -24(%rbp), %rax
    mov (%rax), %rax
    mov %rax, %rcx
    lea a(%rip), %rax
    movsd (%rax,%rcx,8), %xmm0
    movsd (%rsp), %xmm1
    add $8, %rsp
    mov $0, %rax
    ucomisd %xmm0, %xmm1
    movq $1, %rcx
    cmovb %rcx, %rax
    cmp $0, %rax
    je  .L.end.3
    jmp  .L.do.3
.L.end.3:
.L.do.4:
    lea -32(%rbp), %rax
    push %rax
    mov $1, %rax
    push %rax
    lea -32(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    sub %rdi, %rax
    pop %rdi
    mov %rax, (%rdi)
    lea -40(%rbp), %rax
    movsd (%rax), %xmm0
    sub $8, %rsp
    movsd %xmm0, (%rsp)
    lea -32(%rbp), %rax
    mov (%rax), %rax
    mov %rax, %rcx
    lea a(%rip), %rax
    movsd (%rax,%rcx,8), %xmm0
    movsd (%rsp), %xmm1
    add $8, %rsp
    mov $0, %rax
    ucomisd %xmm0, %xmm1
    movq $1, %rcx
    cmova %rcx, %rax
    cmp $0, %rax
    je  .L.end.4
    jmp  .L.do.4
.L.end.4:
    lea -32(%rbp), %rax
    mov (%rax), %rax
    push %rax
    lea -24(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    cmp %rdi, %rax
    setge %al
    movzb %al, %rax
    cmp $0, %rax
    je  .L.else.5
    jmp .L.end.2
    jmp .L.end.5
.L.else.5:
.L.end.5:
    lea -48(%rbp), %rax
    push %rax
    lea -24(%rbp), %rax
    mov (%rax), %rax
    mov %rax, %rcx
    lea a(%rip), %rax
    movsd (%rax,%rcx,8), %xmm0
    pop %rdi
    movsd %xmm0, (%rdi)
    lea -24(%rbp), %rax
    mov (%rax), %rax
    mov %rax, %rcx
    lea a(%rip), %rax
    push %rax
    push %rcx
    lea -32(%rbp), %rax
    mov (%rax), %rax
    mov %rax, %rcx
    lea a(%rip), %rax
    movsd (%rax,%rcx,8), %xmm0
    pop %rcx
    pop %rdi
    movsd %xmm0, (%rdi, %rcx, 8)
    lea -32(%rbp), %rax
    mov (%rax), %rax
    mov %rax, %rcx
    lea a(%rip), %rax
    push %rax
    push %rcx
    lea -48(%rbp), %rax
    movsd (%rax), %xmm0
    pop %rcx
    pop %rdi
    movsd %xmm0, (%rdi, %rcx, 8)
    jmp  .L.condition.2
.L.end.2:
    lea -48(%rbp), %rax
    push %rax
    lea -24(%rbp), %rax
    mov (%rax), %rax
    mov %rax, %rcx
    lea a(%rip), %rax
    movsd (%rax,%rcx,8), %xmm0
    pop %rdi
    movsd %xmm0, (%rdi)
    lea -24(%rbp), %rax
    mov (%rax), %rax
    mov %rax, %rcx
    lea a(%rip), %rax
    push %rax
    push %rcx
    lea -16(%rbp), %rax
    mov (%rax), %rax
    mov %rax, %rcx
    lea a(%rip), %rax
    movsd (%rax,%rcx,8), %xmm0
    pop %rcx
    pop %rdi
    movsd %xmm0, (%rdi, %rcx, 8)
    lea -16(%rbp), %rax
    mov (%rax), %rax
    mov %rax, %rcx
    lea a(%rip), %rax
    push %rax
    push %rcx
    lea -48(%rbp), %rax
    movsd (%rax), %xmm0
    pop %rcx
    pop %rdi
    movsd %xmm0, (%rdi, %rcx, 8)
    lea -8(%rbp), %rax
    mov (%rax), %rax
    push %rax
    lea -32(%rbp), %rax
    mov (%rax), %rax
    push %rax
    pop %rsi
    pop %rdi
    mov $0, %rax
    call quicksort
    mov $1, %rax
    push %rax
    lea -24(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    add %rdi, %rax
    push %rax
    lea -16(%rbp), %rax
    mov (%rax), %rax
    push %rax
    pop %rsi
    pop %rdi
    mov $0, %rax
    call quicksort
L.quicksort.return:
    mov %rbp, %rsp
    pop %rbp
    ret

    .text
    .global main
main:
    push %rbp
    mov %rsp, %rbp
    sub $16, %rsp
    lea -8(%rbp), %rax
    push %rax
    mov $10, %rax
    pop %rdi
    mov %rax, (%rdi)
    mov $0, %rax
    push %rax
    mov $9, %rax
    push %rax
    pop %rsi
    pop %rdi
    mov $0, %rax
    call quicksort
    lea -16(%rbp), %rax
    push %rax
    mov $0, %rax
    pop %rdi
    mov %rax, (%rdi)
.L.condition.6:
    lea -8(%rbp), %rax
    mov (%rax), %rax
    push %rax
    lea -16(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    cmp %rdi, %rax
    setl %al
    movzb %al, %rax
    je  .L.end.6
    lea -16(%rbp), %rax
    mov (%rax), %rax
    mov %rax, %rcx
    lea a(%rip), %rax
    movsd (%rax,%rcx,8), %xmm0
    lea printf_format_float(%rip), %rdi
    call printf
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
    jmp  .L.condition.6
.L.end.6:
    mov $0, %rax
    jmp L.main.return
L.main.return:
    mov %rbp, %rsp
    pop %rbp
    ret

    .data
a:
    .double 52.1
    .double 5.1
    .double 34.1
    .double 6.1
    .double 23.1
    .double 7.1
    .double 2.1
    .double 38.1
    .double 4.1
    .double 16.1
printf_format_float:
    .string   "%f\n"




