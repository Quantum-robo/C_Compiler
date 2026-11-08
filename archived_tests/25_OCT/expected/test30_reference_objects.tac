
Acc::bump:
    BeginFunc
    t1 = *k
    t2 = *this
    t3 = t1 + t2
    *k = t3
    EndFunc

Acc::get:
    BeginFunc
    t4 = *this
    Return t4
    EndFunc

setx:
    BeginFunc
    *p = 3
    EndFunc

sum:
    BeginFunc
    t5 = a[0]
    t6 = a[2]
    t7 = t5 + t6
    Return t7
    EndFunc

main:
    BeginFunc
    t8 = &q
    rq = t8
    *rq = 1
    *(rq + 4) = 2
    t9 = &q
    PushParam t9
    t10 = LCall setx
    PopParams 1
    t11 = &acc
    ra = t11
    v = 4
    t12 = &acc
    *t12 = 2
    t13 = &v
    t14 = &acc
    PushParam t13
    PushParam t14
    t15 = LCall Acc::bump
    PopParams 2
    t16 = &v
    PushParam t16
    PushParam ra
    t17 = LCall Acc::bump
    PopParams 2
    PushParam ra
    t18 = LCall Acc::get
    PopParams 1
    t = t18
    t19 = &v
    p = t19
    t20 = &p
    rp = t20
    t21 = *rp
    *t21 = 8
    t22 = &nums
    rn = t22
    rn[1] = 5
    t23 = &nums
    PushParam t23
    t24 = LCall sum
    PopParams 1
    s = t24
    t25 = &q
    t26 = *t25
    t27 = t26 + v
    t28 = t27 + t
    t29 = t28 + s
    Return t29
    EndFunc
