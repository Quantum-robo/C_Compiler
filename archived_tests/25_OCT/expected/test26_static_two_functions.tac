
_global_init:
    g = 10
    table[0] = 1
    table[1] = 2
    table[2] = 3
    f.n = 100
    h.n = 200

f:
    BeginFunc
    t1 = f.n + 1
    f.n = t1
    Return f.n
    EndFunc

h:
    BeginFunc
    t2 = h.n + 2
    h.n = t2
    Return h.n
    EndFunc

main:
    BeginFunc
    t3 = LCall f
    a = t3
    t4 = LCall h
    b = t4
    t5 = a + b
    t6 = t5 + g
    t7 = table[2]
    t8 = t6 + t7
    Return t8
    EndFunc
