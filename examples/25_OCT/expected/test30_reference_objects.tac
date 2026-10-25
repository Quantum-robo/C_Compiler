
Acc::bump:
    BeginFunc
    t1 = *k
    t2 = t1 + this->n
    *k = t2
    EndFunc

Acc::get:
    BeginFunc
    Return this->n
    EndFunc

setx:
    BeginFunc
    p->x = 3
    EndFunc

sum:
    BeginFunc
    t3 = a[0]
    t4 = a[2]
    t5 = t3 + t4
    Return t5
    EndFunc

main:
    BeginFunc
    t6 = &q
    rq = t6
    rq->x = 1
    rq->y = 2
    t7 = &q
    PushParam t7
    t8 = LCall setx
    PopParams 1
    t9 = &acc
    ra = t9
    v = 4
    acc.n = 2
    t10 = &v
    t11 = &acc
    PushParam t10
    PushParam t11
    t12 = LCall Acc::bump
    PopParams 2
    t13 = &v
    PushParam t13
    PushParam ra
    t14 = LCall Acc::bump
    PopParams 2
    PushParam ra
    t15 = LCall Acc::get
    PopParams 1
    t = t15
    t16 = &v
    p = t16
    t17 = &p
    rp = t17
    t18 = *rp
    *t18 = 8
    t19 = &nums
    rn = t19
    rn[1] = 5
    t20 = &nums
    PushParam t20
    t21 = LCall sum
    PopParams 1
    s = t21
    t22 = q.x + v
    t23 = t22 + t
    t24 = t23 + s
    Return t24
    EndFunc
