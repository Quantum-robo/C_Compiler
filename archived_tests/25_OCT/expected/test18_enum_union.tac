
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
    v.i = 65
    v.c = 'A'
    v.f = 3.14
    t2 = v.i + 0
    k = t2
    t3 = code + k
    Return t3
    EndFunc
