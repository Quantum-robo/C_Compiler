
Counter::increment:
    BeginFunc
    t1 = this->value + 1
    this->value = t1
    EndFunc

Counter::get:
    BeginFunc
    Return this->value
    EndFunc

main:
    BeginFunc
    c.value = 0
    t2 = &c
    PushParam t2
    t3 = LCall Counter::increment
    PopParams 1
    t4 = &c
    PushParam t4
    t5 = LCall Counter::increment
    PopParams 1
    t6 = &c
    PushParam t6
    t7 = LCall Counter::get
    PopParams 1
    v = t7
    t8 = &c
    pc = t8
    t9 = v * 10
    pc->value = t9
    Return pc->value
    EndFunc
