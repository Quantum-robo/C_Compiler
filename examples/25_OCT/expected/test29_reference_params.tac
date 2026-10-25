
inc:
    BeginFunc
    t1 = *x
    t2 = t1 + 1
    *x = t2
    EndFunc

swap:
    BeginFunc
    t3 = *a
    t = t3
    t4 = *b
    *a = t4
    *b = t
    EndFunc

fwd:
    BeginFunc
    PushParam q
    t5 = LCall inc
    PopParams 1
    EndFunc

show:
    BeginFunc
    t6 = *v
    PushParam t6
    PushParam "%d\n"
    t7 = LCall printf
    PopParams 2
    EndFunc

byval:
    BeginFunc
    Return v
    EndFunc

main:
    BeginFunc
    a = 1
    b = 2
    t8 = &a
    r = t8
    t9 = &a
    PushParam t9
    t10 = LCall late
    PopParams 1
    t11 = &a
    PushParam t11
    t12 = LCall inc
    PopParams 1
    PushParam r
    t13 = LCall inc
    PopParams 1
    t14 = &a
    t15 = &b
    PushParam t15
    PushParam t14
    t16 = LCall swap
    PopParams 2
    t17 = &b
    PushParam t17
    t18 = LCall fwd
    PopParams 1
    t19 = &arr[1]
    PushParam t19
    t20 = LCall inc
    PopParams 1
    t21 = 5
    t22 = &t21
    PushParam t22
    t23 = LCall show
    PopParams 1
    t24 = *r
    PushParam t24
    t25 = LCall byval
    PopParams 1
    n = t25
    PushParam r
    PushParam "%d"
    t26 = LCall scanf
    PopParams 2
    t27 = a + b
    t28 = t27 + n
    Return t28
    EndFunc

late:
    BeginFunc
    t29 = *x
    t30 = t29 + 1
    *x = t30
    EndFunc
