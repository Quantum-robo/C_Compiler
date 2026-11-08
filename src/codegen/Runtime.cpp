#include "MipsGen.hpp"

// Runtime library appended to every generated program. Written in SPIM assembly.
//
// Calling convention (the same as for compiled functions): the caller pushes the arguments right to
// left and calls with jal, so on entry argument 0 is at 0($sp), argument 1 at 4($sp), ... The result is
// returned in $v0. A runtime function must preserve $s0-$s7, $fp and $ra; $t0-$t9 may be changed.
const char* const kRuntimeAsm = R"ASM(
# ======================= runtime library =======================

rt_exit:                        # void exit(int status)
    lw   $a0, 0($sp)
    li   $v0, 17
    syscall

rt_malloc:                      # void *malloc(int bytes): word-aligned block from the heap
    lw   $a0, 0($sp)
    addiu $a0, $a0, 3
    srl  $a0, $a0, 2
    sll  $a0, $a0, 2
    bgtz $a0, rt_malloc_go
    li   $a0, 4
rt_malloc_go:
    li   $v0, 9
    syscall
    jr   $ra

rt_free:                        # void free(void *p)
    jr   $ra

rt_strlen:                      # int strlen(const char *s)
    lw   $t0, 0($sp)
    move $v0, $zero
rt_strlen_loop:
    lb   $t1, 0($t0)
    beqz $t1, rt_strlen_done
    addiu $t0, $t0, 1
    addiu $v0, $v0, 1
    j    rt_strlen_loop
rt_strlen_done:
    jr   $ra

rt_strcpy:                      # char *strcpy(char *dst, const char *src)
    lw   $t0, 0($sp)
    lw   $t1, 4($sp)
    move $v0, $t0
rt_strcpy_loop:
    lb   $t2, 0($t1)
    sb   $t2, 0($t0)
    addiu $t0, $t0, 1
    addiu $t1, $t1, 1
    bnez $t2, rt_strcpy_loop
    jr   $ra

rt_strcmp:                      # int strcmp(const char *a, const char *b)
    lw   $t0, 0($sp)
    lw   $t1, 4($sp)
rt_strcmp_loop:
    lbu  $t2, 0($t0)
    lbu  $t3, 0($t1)
    bne  $t2, $t3, rt_strcmp_done
    beqz $t2, rt_strcmp_done
    addiu $t0, $t0, 1
    addiu $t1, $t1, 1
    j    rt_strcmp_loop
rt_strcmp_done:
    subu $v0, $t2, $t3
    jr   $ra

rt_printf:                      # int printf(const char *fmt, ...): supports %d %c %s %%
    addiu $sp, $sp, -16
    sw   $ra, 12($sp)
    sw   $s0, 8($sp)
    sw   $s1, 4($sp)
    sw   $s2, 0($sp)
    lw   $s0, 16($sp)           # format string
    addiu $s1, $sp, 20          # address of the next argument
rt_printf_next:
    lb   $t0, 0($s0)
    beqz $t0, rt_printf_done
    addiu $s0, $s0, 1
    li   $t1, 37                # '%'
    beq  $t0, $t1, rt_printf_spec
rt_printf_putc:
    move $a0, $t0
    li   $v0, 11
    syscall
    j    rt_printf_next
rt_printf_spec:
    lb   $t0, 0($s0)
    addiu $s0, $s0, 1
    li   $t1, 100               # 'd'
    beq  $t0, $t1, rt_printf_d
    li   $t1, 99                # 'c'
    beq  $t0, $t1, rt_printf_c
    li   $t1, 115               # 's'
    beq  $t0, $t1, rt_printf_s
    j    rt_printf_putc         # "%%" and unknown specifiers print the character itself
rt_printf_d:
    lw   $a0, 0($s1)
    addiu $s1, $s1, 4
    li   $v0, 1
    syscall
    j    rt_printf_next
rt_printf_c:
    lw   $a0, 0($s1)
    addiu $s1, $s1, 4
    li   $v0, 11
    syscall
    j    rt_printf_next
rt_printf_s:
    lw   $a0, 0($s1)
    addiu $s1, $s1, 4
    li   $v0, 4
    syscall
    j    rt_printf_next
rt_printf_done:
    move $v0, $zero
    lw   $ra, 12($sp)
    lw   $s0, 8($sp)
    lw   $s1, 4($sp)
    lw   $s2, 0($sp)
    addiu $sp, $sp, 16
    jr   $ra
)ASM";
