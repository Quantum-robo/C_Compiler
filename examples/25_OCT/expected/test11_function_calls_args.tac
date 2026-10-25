
add:
    BeginFunc
    t1 = a + b
    Return t1
    EndFunc

mul3:
    BeginFunc
    t2 = a * b
    t3 = t2 * c
    Return t3
    EndFunc

show:
    BeginFunc
    PushParam v
    PushParam "%d\n"
    t4 = LCall printf
    PopParams 2
    Return
    EndFunc

main:
    BeginFunc
    PushParam 3
    PushParam 2
    t5 = LCall add
    PopParams 2
    x = t5
    t6 = x + 1
    PushParam 2
    PushParam t6
    PushParam x
    t7 = LCall mul3
    PopParams 3
    y = t7
    PushParam 2
    PushParam 1
    t8 = LCall add
    PopParams 2
    PushParam 3
    PushParam 2
    PushParam 1
    t9 = LCall mul3
    PopParams 3
    PushParam t9
    PushParam t8
    t10 = LCall add
    PopParams 2
    z = t10
    PushParam z
    t11 = LCall show
    PopParams 1
    PushParam y
    PushParam x
    t12 = LCall add
    PopParams 2
    PushParam t12
    t13 = LCall show
    PopParams 1
    Return 0
    EndFunc
