
main:
    BeginFunc
    t1 = new int
    single = t1
    *single = 42
    n = 5
    t2 = new int[n]
    arr = t2
    i = 0
L1:
    t3 = i < n
    IfFalse t3 Goto L4
    Goto L2
L3:
    t4 = i
    t5 = i + 1
    i = t5
    Goto L1
L2:
    t6 = i * 2
    arr[i] = t6
    Goto L3
L4:
    t7 = sizeof n
    t8 = n * t7
    PushParam t8
    t9 = LCall malloc
    PopParams 1
    buf = t9
    t10 = arr[4]
    buf[0] = t10
    PushParam buf
    t11 = LCall free
    PopParams 1
    delete single
    delete arr
    Return 0
    EndFunc
