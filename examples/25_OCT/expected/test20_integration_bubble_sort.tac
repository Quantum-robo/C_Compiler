
swap:
    BeginFunc
    t1 = *a
    tmp = t1
    t2 = *b
    *a = t2
    *b = tmp
    EndFunc

sort:
    BeginFunc
    i = 0
L1:
    t3 = n - 1
    t4 = i < t3
    IfFalse t4 Goto L4
    Goto L2
L3:
    t5 = i
    t6 = i + 1
    i = t6
    Goto L1
L2:
    j = 0
L5:
    t7 = n - i
    t8 = t7 - 1
    t9 = j < t8
    IfFalse t9 Goto L8
    Goto L6
L7:
    t10 = j
    t11 = j + 1
    j = t11
    Goto L5
L6:
    t12 = j + 1
    t13 = arr[j]
    t14 = arr[t12]
    t15 = t13 > t14
    IfFalse t15 Goto L9
    t16 = &arr[j]
    t17 = j + 1
    t18 = &arr[t17]
    PushParam t18
    PushParam t16
    t19 = LCall swap
    PopParams 2
L9:
    Goto L7
L8:
    Goto L3
L4:
    EndFunc

main:
    BeginFunc
    data[0] = 5
    data[1] = 1
    data[2] = 4
    data[3] = 2
    data[4] = 3
    PushParam 5
    PushParam data
    t20 = LCall sort
    PopParams 2
    t21 = data[0]
    s.min = t21
    t22 = data[4]
    s.max = t22
    i = 0
L10:
    t23 = i < 5
    IfFalse t23 Goto L13
    Goto L11
L12:
    t24 = i
    t25 = i + 1
    i = t25
    Goto L10
L11:
    t26 = data[i]
    PushParam t26
    PushParam "%d "
    t27 = LCall printf
    PopParams 2
    Goto L12
L13:
    PushParam s.max
    PushParam s.min
    PushParam "\nmin=%d max=%d\n"
    t28 = LCall printf
    PopParams 3
    Return 0
    EndFunc
