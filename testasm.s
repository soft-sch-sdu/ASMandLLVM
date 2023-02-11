    .text
    .global value
value:
    push %rbp
    mov %rsp, %rbp
    sub $32, %rsp
    movq %xmm0, -8(%rbp)
    movq %xmm1, -16(%rbp)
    mov %rdi, -24(%rbp)
    lea -24(%rbp), %rax
    mov (%rax), %rax
    cvtsi2sd %rax, %xmm0
    sub $8, %rsp
    movsd %xmm0, (%rsp)
    lea -16(%rbp), %rax
    movsd (%rax), %xmm0
    sub $8, %rsp
    movsd %xmm0, (%rsp)
    lea -8(%rbp), %rax
    movsd (%rax), %xmm0
    movsd (%rsp), %xmm1
    add $8, %rsp
    addsd %xmm1, %xmm0
    movsd (%rsp), %xmm1
    add $8, %rsp
    addsd %xmm1, %xmm0
    jmp L.value.return
L.value.return:
    mov %rbp, %rsp
    pop %rbp
    ret

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
    mov $4622776132509787750, %rax   # float 11.7
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
    mov $4622832427505129882, %rax   # float 11.8
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
    mov $4623789442425946112, %rax   # float 13.5
    movq %rax, %xmm0
    pop %rdi
    movsd %xmm0, (%rdi)
    mov $0, %rax
    imul $8, %rax
    push %rax
    lea -24(%rbp), %rax
    pop %rdi
    add %rdi, %rax
    movsd (%rax), %xmm0
    sub $8, %rsp
    movsd %xmm0, (%rsp)
    mov $1, %rax
    imul $8, %rax
    push %rax
    lea -24(%rbp), %rax
    pop %rdi
    add %rdi, %rax
    movsd (%rax), %xmm0
    sub $8, %rsp
    movsd %xmm0, (%rsp)
    mov $2, %rax
    push %rax
    pop %rdi
    movsd (%rsp), %xmm1
    add $8, %rsp
    movsd (%rsp), %xmm0
    add $8, %rsp
    mov $0, %rax
    call value
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