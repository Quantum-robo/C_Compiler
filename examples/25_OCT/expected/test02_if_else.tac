
classify:
    BeginFunc
    r = 0
    t1 = n > 0
    IfFalse t1 Goto L1
    r = 1
L1:
    t2 = n % 2
    t3 = t2 == 0
    IfFalse t3 Goto L2
    t4 = r + 10
    r = t4
    Goto L3
L2:
    t5 = r + 20
    r = t5
L3:
    t6 = n < 10
    IfFalse t6 Goto L4
    t7 = r + 100
    r = t7
    Goto L5
L4:
    t8 = n < 100
    IfFalse t8 Goto L6
    t9 = r + 200
    r = t9
    Goto L7
L6:
    t10 = r + 300
    r = t10
L7:
L5:
    t11 = n > 0
    IfFalse t11 Goto L8
    t12 = n > 50
    IfFalse t12 Goto L9
    t13 = r * 2
    r = t13
    Goto L10
L9:
    t14 = r * 3
    r = t14
L10:
L8:
    Return r
    EndFunc

main:
    BeginFunc
    PushParam 42
    t15 = LCall classify
    PopParams 1
    x = t15
    Return x
    EndFunc
