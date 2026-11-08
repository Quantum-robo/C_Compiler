
main:
    BeginFunc
    val = 25
    t1 = &val
    ref = t1
    p = ref
    t2 = *ref
    copy = t2
    *ref = 50
    t3 = *ref
    t4 = t3 + 2
    *ref = t4
    t5 = *ref
    t6 = t5 + 1
    *ref = t6
    t7 = *ref
    t8 = t7 - 1
    *ref = t8
    t9 = &arr[1]
    e = t9
    *e = 7
    t10 = 5
    t11 = &t10
    c = t11
    *p = 9
    t12 = *ref
    t13 = t12 + copy
    t14 = *e
    t15 = t13 + t14
    t16 = *c
    t17 = t15 + t16
    Return t17
    EndFunc
