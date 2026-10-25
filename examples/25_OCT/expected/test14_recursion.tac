
factorial:
    BeginFunc
    t1 = n <= 1
    IfFalse t1 Goto L1
    Return 1
L1:
    t2 = n - 1
    PushParam t2
    t3 = LCall factorial
    PopParams 1
    t4 = n * t3
    Return t4
    EndFunc

fib:
    BeginFunc
    t5 = n < 2
    IfFalse t5 Goto L2
    Return n
L2:
    t6 = n - 1
    PushParam t6
    t7 = LCall fib
    PopParams 1
    t8 = n - 2
    PushParam t8
    t9 = LCall fib
    PopParams 1
    t10 = t7 + t9
    Return t10
    EndFunc

gcd:
    BeginFunc
    t11 = b == 0
    IfFalse t11 Goto L3
    Return a
L3:
    t12 = a % b
    PushParam t12
    PushParam b
    t13 = LCall gcd
    PopParams 2
    Return t13
    EndFunc

power:
    BeginFunc
    t14 = exp == 0
    IfFalse t14 Goto L4
    Return 1
L4:
    t15 = exp - 1
    PushParam t15
    PushParam base
    t16 = LCall power
    PopParams 2
    t17 = base * t16
    Return t17
    EndFunc

main:
    BeginFunc
    PushParam 5
    t18 = LCall factorial
    PopParams 1
    f = t18
    PushParam 7
    t19 = LCall fib
    PopParams 1
    fb = t19
    PushParam 18
    PushParam 48
    t20 = LCall gcd
    PopParams 2
    g = t20
    PushParam 10
    PushParam 2
    t21 = LCall power
    PopParams 2
    p = t21
    t22 = f + fb
    t23 = t22 + g
    t24 = t23 + p
    Return t24
    EndFunc
