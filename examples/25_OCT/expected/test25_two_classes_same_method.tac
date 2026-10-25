
A::get:
    BeginFunc
    Return this->v
    EndFunc

A::twice:
    BeginFunc
    PushParam this
    t1 = LCall A::get
    PopParams 1
    PushParam this
    t2 = LCall A::get
    PopParams 1
    t3 = t1 + t2
    Return t3
    EndFunc

B::get:
    BeginFunc
    t4 = this->w * 10
    Return t4
    EndFunc

main:
    BeginFunc
    t5 = &a
    pa = t5
    a.v = 3
    b.w = 4
    t6 = &a
    PushParam t6
    t7 = LCall A::get
    PopParams 1
    x = t7
    t8 = &b
    PushParam t8
    t9 = LCall B::get
    PopParams 1
    y = t9
    PushParam pa
    t10 = LCall A::twice
    PopParams 1
    z = t10
    t11 = x + y
    t12 = t11 + z
    Return t12
    EndFunc
