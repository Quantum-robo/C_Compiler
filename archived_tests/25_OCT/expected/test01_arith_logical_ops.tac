
main:
    BeginFunc
    a = 20
    b = 6
    t1 = a + b
    add = t1
    t2 = a - b
    sub = t2
    t3 = a * b
    mul = t3
    t4 = a / b
    div = t4
    t5 = a % b
    mod = t5
    t6 = -a
    neg = t6
    t7 = b * 2
    t8 = a + t7
    t9 = a - b
    t10 = t9 / 2
    t11 = t8 - t10
    prec = t11
    t12 = a < b
    lt = t12
    t13 = a > b
    gt = t13
    t14 = a <= b
    le = t14
    t15 = a >= b
    ge = t15
    t16 = a == b
    eq = t16
    t17 = a != b
    ne = t17
    t18 = a > 0
    IfFalse t18 Goto L1
    t19 = b > 0
    IfFalse t19 Goto L1
    t20 = 1
    Goto L2
L1:
    t20 = 0
L2:
    land = t20
    t21 = a < 0
    If t21 Goto L3
    t22 = b > 0
    If t22 Goto L3
    t23 = 0
    Goto L4
L3:
    t23 = 1
L4:
    lor = t23
    t24 = !land
    lnot = t24
    t25 = a & b
    band = t25
    t26 = a | b
    bor = t26
    t27 = a ^ b
    bxor = t27
    t28 = ~a
    bnot = t28
    t29 = a << 2
    shl = t29
    t30 = a >> 1
    shr = t30
    t31 = a + 1
    a = t31
    t32 = a - 2
    a = t32
    t33 = a * 3
    a = t33
    t34 = a / 4
    a = t34
    t35 = a % 5
    a = t35
    t36 = a
    t37 = a + 1
    a = t37
    t38 = b
    t39 = b - 1
    b = t39
    t40 = a + 1
    a = t40
    t41 = b - 1
    b = t41
    t42 = add + sub
    t43 = t42 + mul
    t44 = t43 + div
    t45 = t44 + mod
    Return t45
    EndFunc
