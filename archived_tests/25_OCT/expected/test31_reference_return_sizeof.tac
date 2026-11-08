
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
    s1 = 4
    s2 = 4
    s3 = 4
    t13 = g + x
    t14 = t13 + y
    t15 = t14 + v
    t16 = t15 + s1
    t17 = t16 + s2
    t18 = t17 + s3
    Return t18
    EndFunc
