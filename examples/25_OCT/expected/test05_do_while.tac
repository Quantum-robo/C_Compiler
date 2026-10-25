
main:
    BeginFunc
    count = 0
L1:
    t1 = count
    t2 = count + 1
    count = t2
L2:
    t3 = count < 5
    If t3 Goto L1
L3:
    x = 100
L4:
    t4 = x - 30
    x = t4
L5:
    t5 = x > 0
    If t5 Goto L4
L6:
    outer = 0
    acc = 0
L7:
    inner = 0
L10:
    t6 = outer + inner
    t7 = acc + t6
    acc = t7
    t8 = inner
    t9 = inner + 1
    inner = t9
L11:
    t10 = inner < 2
    If t10 Goto L10
L12:
    t11 = outer
    t12 = outer + 1
    outer = t12
L8:
    t13 = outer < 2
    If t13 Goto L7
L9:
    t14 = count + acc
    Return t14
    EndFunc
