
area:
    BeginFunc
    t1 = &r
    t2 = &r
    t3 = *(t1 + 8)
    t4 = *t2
    t5 = t3 - t4
    w = t5
    t6 = &r
    t7 = &r
    t8 = *(t6 + 12)
    t9 = *(t7 + 4)
    t10 = t8 - t9
    h = t10
    t11 = w * h
    Return t11
    EndFunc

main:
    BeginFunc
    t12 = &p
    *t12 = 3
    t13 = &p
    *(t13 + 4) = 4
    t14 = &p
    pp = t14
    t15 = *pp
    t16 = t15 + 1
    *pp = t16
    t17 = &r
    *t17 = 0
    t18 = &r
    *(t18 + 4) = 0
    t19 = &r
    t20 = &p
    t21 = *t20
    *(t19 + 8) = t21
    t22 = &r
    t23 = &p
    t24 = *(t23 + 4)
    *(t22 + 12) = t24
    PushParam r
    t25 = LCall area
    PopParams 1
    a = t25
    Return a
    EndFunc
