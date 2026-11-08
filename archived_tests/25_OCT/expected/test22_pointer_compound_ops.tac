
main:
    BeginFunc
    x = 1
    t1 = &x
    p = t1
    t2 = *p
    t3 = t2 + 5
    *p = t3
    t4 = *p
    t5 = t4 + 1
    *p = t5
    t6 = *p
    t7 = t6 + 1
    *p = t7
    t8 = *p
    t9 = t8 - 1
    *p = t9
    arr[1] = 4
    t10 = arr[1]
    t11 = t10 * 2
    arr[1] = t11
    t12 = arr[2]
    t13 = t12 + 1
    arr[2] = t13
    t14 = arr[0]
    t15 = t14 - 1
    arr[0] = t15
    t16 = p
    t17 = p + 1
    p = t17
    t18 = *t16
    y = t18
    t19 = x + y
    Return t19
    EndFunc
