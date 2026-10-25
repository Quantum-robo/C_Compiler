
f:
    BeginFunc
    Goto L1
L3:
    Return 1
L4:
    Return 2
L5:
    Return 3
    Goto L2
L1:
    t1 = x == 0
    If t1 Goto L3
    t2 = x == -3
    If t2 Goto L4
    t3 = x == 8
    If t3 Goto L5
    Goto L2
L2:
    ZERO = 9
    t4 = ZERO + -2
    t5 = t4 + 16
    t6 = t5 + 65
    t7 = t6 + 66
    Return t7
    EndFunc

main:
    BeginFunc
    o = 66
    PushParam o
    t8 = LCall f
    PopParams 1
    Return t8
    EndFunc
