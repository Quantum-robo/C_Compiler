
main:
    BeginFunc
    x = 5
    y = 0
    t1 = &x
    p = t1
    t2 = &p
    pp = t2
    t3 = *p
    y = t3
    *p = 10
    t4 = *pp
    *t4 = 20
    q = arr
    t5 = q + 1
    q = t5
    *q = 7
    t6 = *p
    t7 = *q
    t8 = t6 + t7
    z = t8
    t9 = z + y
    Return t9
    EndFunc
