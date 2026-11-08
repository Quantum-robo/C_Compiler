
main:
    BeginFunc
    t1 = &b
    p = t1
    Zero pr :8
    t2 = &pr
    *t2 = 7
    *(t2 + 4) = 9
    t3 = &b
    *t3 = 4
    t4 = &b
    t5 = t4 + 4
    t5[0] = 1
    t6 = p + 4
    t6[1] = 2
    t7 = *p
    t8 = t7 - 1
    t9 = p + 4
    t9[t8] = 3
    t10 = p + 4
    t11 = &b
    t12 = t11 + 4
    t13 = t10[1]
    t14 = t12[0]
    t15 = t13 + t14
    v = t15
    t16 = &pr
    t17 = *(t16 + 4)
    t18 = v + t17
    Return t18
    EndFunc
