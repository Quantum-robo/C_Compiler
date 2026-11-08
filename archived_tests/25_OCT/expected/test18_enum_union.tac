
main:
    BeginFunc
    col = 6
    code = 0
    t1 = col == 5
    IfFalse t1 Goto L1
    code = 1
    Goto L2
L1:
    code = 2
L2:
    t2 = &v
    *t2 = 65
    t3 = &v
    *t3 = 'A' :1
    t4 = &v
    *t4 = 3.14 :f4
    t5 = &v
    t6 = *t5
    t7 = t6 + 0
    k = t7
    t8 = code + k
    Return t8
    EndFunc
