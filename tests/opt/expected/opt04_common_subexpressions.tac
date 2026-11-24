
area:
    BeginFunc
    t1 = w * h
    t2 = t1 + 1
    t4 = t1 + 2
    t5 = w + h
    t7 = t5 * t5
    t8 = w + 1
    w = t8
    t9 = t8 * h
    t10 = t2 + t4
    t11 = t10 + t7
    t12 = t11 + t9
    Return t12
    EndFunc

main:
    BeginFunc
    PushParam 3
    PushParam 2
    t13 = LCall area
    PopParams 2
    Return t13
    EndFunc
