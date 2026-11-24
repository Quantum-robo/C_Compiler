
copies:
    BeginFunc
    t1 = a + a
    Return t1
    EndFunc

alias:
    BeginFunc
    y = x
    t2 = &y
    *t2 = 7
    t3 = y + 1
    Return t3
    EndFunc

main:
    BeginFunc
    PushParam 3
    t4 = LCall copies
    PopParams 1
    PushParam 4
    t5 = LCall alias
    PopParams 1
    t6 = t4 + t5
    Return t6
    EndFunc
