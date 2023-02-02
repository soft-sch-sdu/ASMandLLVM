

    .text
    .global fun
fun:
    push %rbp
    mov %rsp, %rbp
    sub $32, %rsp
    mov %rdi, -8(%rbp)
    mov %rsi, -16(%rbp)
    mov %rdx, -24(%rbp)
    lea -24(%rbp), %rax
    mov (%rax), %rax
    push %rax
    lea -16(%rbp), %rax
    mov (%rax), %rax
    push %rax
    lea -8(%rbp), %rax
    mov (%rax), %rax
    pop %rdi
    add %rdi, %rax
    pop %rdi
    imul %rdi, %rax
    push %rax
    lea h1(%rip), %rax
    mov (%rax), %rax
    pop %rdi
    add %rdi, %rax
    jmp L.fun.return
L.fun.return:
    mov %rbp, %rsp
    pop %rbp
    ret

    .text
    .global fun1
fun1:
    push %rbp
    mov %rsp, %rbp
    sub $16, %rsp
    mov %rdi, -8(%rbp)
    mov $6, %rax
    jmp L.fun1.return
L.fun1.return:
    mov %rbp, %rsp
    pop %rbp
    ret

    .text
    .global main
main:
    push %rbp
    mov %rsp, %rbp
    mov $5, %rax
    push %rax
    mov $1, %rax
    push %rax
    mov $1, %rax
    push %rax
    pop %rdx
    pop %rsi
    pop %rdi
    mov $0, %rax
    call fun
    jmp L.main.return
L.main.return:
    lea printf_format, %rdi
    mov %rax, %rsi
    call printf
    mov %rbp, %rsp
    pop %rbp
    ret

    .data
g:
    .long 7
h1:
    .long 4
h2:
    .double 0
printf_format:
  .string   "%d\n"

