
main:
    BeginFunc
    n = 12345
    digits = 0
L1:
    t1 = n > 0
    IfFalse t1 Goto L2
    t2 = n / 10
    n = t2
    t3 = digits
    t4 = digits + 1
    digits = t4
    Goto L1
L2:
    i = 0
    total = 0
L3:
    t5 = i < 3
    IfFalse t5 Goto L4
    j = 0
L5:
    t6 = j < 3
    IfFalse t6 Goto L6
    t7 = i * j
    t8 = total + t7
    total = t8
    t9 = j
    t10 = j + 1
    j = t10
    Goto L5
L6:
    t11 = i
    t12 = i + 1
    i = t12
    Goto L3
L4:
    a = 0
    b = 10
L7:
    t13 = a < 5
    IfFalse t13 Goto L9
    t14 = b > 5
    IfFalse t14 Goto L9
    t15 = 1
    Goto L10
L9:
    t15 = 0
L10:
    IfFalse t15 Goto L8
    t16 = a
    t17 = a + 1
    a = t17
    t18 = b
    t19 = b - 1
    b = t19
    Goto L7
L8:
    t20 = digits + total
    Return t20
    EndFunc
