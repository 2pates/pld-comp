.globl main
main: 
pushq %rbp
movq %rsp, %rbp
Visit return
Visit atomic
#tmp1
    movl $42, -4(%rbp)
    movl -4(%rbp), %eax
    leave
    ret
