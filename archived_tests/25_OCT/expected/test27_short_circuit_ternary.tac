
side:
    BeginFunc
    Return v
    EndFunc

main:
    BeginFunc
    a = 0
    b = 1
    IfFalse a Goto L1
    PushParam 1
    t1 = LCall side
    PopParams 1
    IfFalse t1 Goto L1
    t2 = 1
    Goto L2
L1:
    t2 = 0
L2:
    r1 = t2
    If b Goto L3
    PushParam 2
    t3 = LCall side
    PopParams 1
    If t3 Goto L3
    t4 = 0
    Goto L4
L3:
    t4 = 1
L4:
    r2 = t4
    If a Goto L5
    IfFalse b Goto L6
    PushParam 3
    t5 = LCall side
    PopParams 1
    IfFalse t5 Goto L6
    t6 = 1
    Goto L7
L6:
    t6 = 0
L7:
    If t6 Goto L5
    t7 = 0
    Goto L8
L5:
    t7 = 1
L8:
    r3 = t7
    t8 = a < b
    IfFalse t8 Goto L9
    PushParam 10
    t10 = LCall side
    PopParams 1
    t9 = t10
    Goto L10
L9:
    PushParam 20
    t11 = LCall side
    PopParams 1
    t9 = t11
L10:
    r4 = t9
    IfFalse a Goto L11
    t12 = 1
    Goto L12
L11:
    IfFalse b Goto L13
    t13 = 2
    Goto L14
L13:
    t13 = 3
L14:
    t12 = t13
L12:
    r5 = t12
    t14 = r1 + r2
    t15 = t14 + r3
    t16 = t15 + r4
    t17 = t16 + r5
    Return t17
    EndFunc
