
main:
    BeginFunc
    a = 1
    b = 2
    r = 0
    Goto L1
L3:
    Goto L4
L6:
    r = 11
    Goto L5
L7:
    r = 12
    Goto L5
    Goto L5
L4:
    t1 = b == 1
    If t1 Goto L6
    t2 = b == 2
    If t2 Goto L7
    Goto L5
L5:
    t3 = r + 100
    r = t3
    Goto L2
L8:
    r = 2
    Goto L2
    Goto L2
L1:
    t4 = a == 1
    If t4 Goto L3
    t5 = a == 2
    If t5 Goto L8
    Goto L2
L2:
    Return r
    EndFunc
