
    .text
    .global foo
foo:
    push %rbp
    mov %rsp, %rbp
    sub $32, %rsp
    mov %rdi, -8(%rbp)
    lea -16(%rbp), %rax
    push %rax
    mov $8, %rax
    pop %rdi
    mov %rax, (%rdi)
    lea -24(%rbp), %rax
    push %rax
    mov $4621087282649523814, %rax   # float 8.7
    movq %rax, %xmm0
    pop %rdi
    movsd %xmm0, (%rdi)
    lea -8(%rbp), %rax
    mov (%rax), %rax
    cmp $0, %rax
    je  .L.else.1
    lea -8(%rbp), %rax
    mov (%rax), %rax
    jmp L.foo.return
    jmp .L.end.1
.L.else.1:
.L.end.1:
L.foo.return:
    mov %rbp, %rsp
    pop %rbp
    ret

    .text
    .global main
main:
    push %rbp
    mov %rsp, %rbp
    mov $23, %rax
    push %rax
    pop %rdi
    mov $0, %rax
    call foo
    jmp L.main.return
L.main.return:
    mov %rbp, %rsp
    pop %rbp
    ret

    .data