
main:
    BeginFunc
    m[0] = 1
    m[1] = 2
    m[2] = 3
    m[3] = 4
    m[4] = 5
    m[5] = 6
    sum = 0
    i = 0
L1:
    t1 = i < 2
    IfFalse t1 Goto L4
    Goto L2
L3:
    t2 = i
    t3 = i + 1
    i = t3
    Goto L1
L2:
    j = 0
L5:
    t4 = j < 3
    IfFalse t4 Goto L8
    Goto L6
L7:
    t5 = j
    t6 = j + 1
    j = t6
    Goto L5
L6:
    t7 = i * 3
    t8 = t7 + j
    t9 = m[t8]
    t10 = sum + t9
    sum = t10
    Goto L7
L8:
    Goto L3
L4:
    t[3] = 9
    t11 = t[3]
    t12 = m[5]
    t13 = t11 + t12
    t[4] = t13
    t14 = m[0]
    t15 = t14 * 10
    m[4] = t15
    Return sum
    EndFunc
