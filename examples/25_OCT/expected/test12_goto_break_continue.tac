
main:
    BeginFunc
    sum = 0
    i = 0
L1:
    t1 = i < 10
    IfFalse t1 Goto L4
    Goto L2
L3:
    t2 = i
    t3 = i + 1
    i = t3
    Goto L1
L2:
    t4 = i % 2
    t5 = t4 == 0
    IfFalse t5 Goto L5
    Goto L3
L5:
    t6 = i > 7
    IfFalse t6 Goto L6
    Goto L4
L6:
    t7 = sum + i
    sum = t7
    Goto L3
L4:
    w = 0
L7:
    IfFalse 1 Goto L8
    t8 = w
    t9 = w + 1
    w = t9
    t10 = w == 2
    IfFalse t10 Goto L9
    Goto L7
L9:
    t11 = w >= 5
    IfFalse t11 Goto L10
    Goto L8
L10:
    Goto L7
L8:
    d = 0
L11:
    t12 = d
    t13 = d + 1
    d = t13
    t14 = d == 1
    IfFalse t14 Goto L14
    Goto L12
L14:
L12:
    t15 = d < 3
    If t15 Goto L11
L13:
    a = 0
L15:
    t16 = a < 3
    IfFalse t16 Goto L18
    Goto L16
L17:
    t17 = a
    t18 = a + 1
    a = t18
    Goto L15
L16:
    b = 0
L19:
    t19 = b < 3
    IfFalse t19 Goto L22
    Goto L20
L21:
    t20 = b
    t21 = b + 1
    b = t21
    Goto L19
L20:
    t22 = a * b
    t23 = t22 == 4
    IfFalse t23 Goto L23
    Goto found
L23:
    Goto L21
L22:
    Goto L17
L18:
    sum = -1
found:
    t24 = sum + w
    t25 = t24 + d
    Return t25
    EndFunc
