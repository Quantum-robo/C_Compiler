
main:
    BeginFunc
    PushParam 4
    t1 = LCall malloc
    PopParams 1
    single = t1
    *single = 42
    n = 5
    t2 = n * 4
    PushParam t2
    t3 = LCall malloc
    PopParams 1
    arr = t3
    i = 0
L1:
    t4 = i < n
    IfFalse t4 Goto L4
    Goto L2
L3:
    t5 = i
    t6 = i + 1
    i = t6
    Goto L1
L2:
    t7 = i * 2
    arr[i] = t7
    Goto L3
L4:
    t8 = n * 4
    PushParam t8
    t9 = LCall malloc
    PopParams 1
    buf = t9
    t10 = arr[4]
    buf[0] = t10
    PushParam buf
    t11 = LCall free
    PopParams 1
    PushParam single
    LCall free
    PopParams 1
    PushParam arr
    LCall free
    PopParams 1
    Return 0
    EndFunc
