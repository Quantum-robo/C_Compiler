
main:
    BeginFunc
    sum = 0
    k = 0
L1:
    t1 = k < 10
    IfFalse t1 Goto L4
    Goto L2
L3:
    t2 = k
    t3 = k + 1
    k = t3
    Goto L1
L2:
    t4 = sum + k
    sum = t4
    Goto L3
L4:
    i = 10
L5:
    t5 = i > 0
    IfFalse t5 Goto L8
    Goto L6
L7:
    t6 = i
    t7 = i - 1
    i = t7
    Goto L5
L6:
    t8 = sum - 1
    sum = t8
    Goto L7
L8:
    r = 0
L9:
    t9 = r < 3
    IfFalse t9 Goto L12
    Goto L10
L11:
    t10 = r
    t11 = r + 1
    r = t11
    Goto L9
L10:
    c = 0
L13:
    t12 = c < 3
    IfFalse t12 Goto L16
    Goto L14
L15:
    t13 = c
    t14 = c + 1
    c = t14
    Goto L13
L14:
    t15 = r * c
    t16 = sum + t15
    sum = t16
    Goto L15
L16:
    Goto L11
L12:
    j = 0
L17:
    t17 = j < 5
    IfFalse t17 Goto L20
    Goto L18
L19:
    Goto L17
L18:
    t18 = j + 2
    j = t18
    Goto L19
L20:
    Return sum
    EndFunc
