
_global_init:
    g = 1

getg:
    BeginFunc
    t1 = &g
    Return t1
    EndFunc

pick:
    BeginFunc
    IfFalse c Goto L1
    Return a
L1:
    Return b
    EndFunc

main:
    BeginFunc
    x = 1
    y = 2
    t2 = LCall getg
    r = t2
    *r = 10
    t3 = LCall getg
    *t3 = 20
    t4 = &x
    t5 = &y
    PushParam 1
    PushParam t5
    PushParam t4
    t6 = LCall pick
    PopParams 3
    *t6 = 5
    t7 = &x
    t8 = &y
    PushParam 0
    PushParam t8
    PushParam t7
    t9 = LCall pick
    PopParams 3
    t10 = *t9
    t11 = t10 + 1
    v = t11
    q = p
    t12 = &x
    rr = t12
    t13 = sizeof *q
    s1 = t13
    t14 = sizeof p[1]
    s2 = t14
    t15 = sizeof *rr
    s3 = t15
    t16 = g + x
    t17 = t16 + y
    t18 = t17 + v
    t19 = t18 + s1
    t20 = t19 + s2
    t21 = t20 + s3
    Return t21
    EndFunc
