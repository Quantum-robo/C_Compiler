
main:
    BeginFunc
    day = 3
    kind = 0
    Goto L1
L3:
    kind = 10
    Goto L2
L4:
L5:
    kind = 20
    Goto L2
L6:
    kind = 40
L7:
    t1 = kind + 1
    kind = t1
    Goto L2
    Goto L2
L1:
    t2 = day == 1
    If t2 Goto L3
    t3 = day == 2
    If t3 Goto L4
    t4 = day == 3
    If t4 Goto L5
    t5 = day == 4
    If t5 Goto L6
    Goto L7
L2:
    score = 0
    i = 0
L8:
    t6 = i < 3
    IfFalse t6 Goto L11
    Goto L9
L10:
    t7 = i
    t8 = i + 1
    i = t8
    Goto L8
L9:
    Goto L12
L14:
    t9 = score + 5
    score = t9
    Goto L13
L15:
    t10 = score + 1
    score = t10
    Goto L13
L12:
    t11 = i == 0
    If t11 Goto L14
    Goto L15
L13:
    Goto L10
L11:
    t12 = kind + score
    Return t12
    EndFunc
