    .text
    .global main
main:
    push %rbp
    mov %rsp, %rbp
    sub $16, %rsp
    lea -1(%rbp), %rax
    push %rax
    mov $97, %rax   # char
    pop %rdi
    mov %rax, (%rdi)
    lea -1(%rbp), %rax
    mov (%rax), %rax
    jmp L.main.return
L.main.return:
    lea printf_format, %rdi
    mov %rax, %rsi
    call printf
    mov %rbp, %rsp
    pop %rbp
    ret

    .data
printf_format:
  .string   "%c\n"