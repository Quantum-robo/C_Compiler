
A::get:
    BeginFunc
    t1 = *this
    Return t1
    EndFunc

A::twice:
    BeginFunc
    PushParam this
    t2 = LCall A::get
    PopParams 1
    PushParam this
    t3 = LCall A::get
    PopParams 1
    t4 = t2 + t3
    Return t4
    EndFunc

B::get:
    BeginFunc
    t5 = *this
    t6 = t5 * 10
    Return t6
    EndFunc

main:
    BeginFunc
    t7 = &a
    pa = t7
    t8 = &a
    *t8 = 3
    t9 = &b
    *t9 = 4
    t10 = &a
    PushParam t10
    t11 = LCall A::get
    PopParams 1
    x = t11
    t12 = &b
    PushParam t12
    t13 = LCall B::get
    PopParams 1
    y = t13
    PushParam pa
    t14 = LCall A::twice
    PopParams 1
    z = t14
    t15 = x + y
    t16 = t15 + z
    Return t16
    EndFunc
