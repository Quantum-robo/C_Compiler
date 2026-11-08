
Counter::increment:
    BeginFunc
    t1 = *this
    t2 = t1 + 1
    *this = t2
    EndFunc

Counter::get:
    BeginFunc
    t3 = *this
    Return t3
    EndFunc

main:
    BeginFunc
    t4 = &c
    *t4 = 0
    t5 = &c
    PushParam t5
    t6 = LCall Counter::increment
    PopParams 1
    t7 = &c
    PushParam t7
    t8 = LCall Counter::increment
    PopParams 1
    t9 = &c
    PushParam t9
    t10 = LCall Counter::get
    PopParams 1
    v = t10
    t11 = &c
    pc = t11
    t12 = v * 10
    *pc = t12
    t13 = *pc
    Return t13
    EndFunc
