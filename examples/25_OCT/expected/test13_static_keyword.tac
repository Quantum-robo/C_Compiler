
_global_init:
    calls = 0
    counter.count = 0

helper:
    BeginFunc
    t1 = v * 2
    Return t1
    EndFunc

counter:
    BeginFunc
    t2 = counter.count + 1
    counter.count = t2
    t3 = calls
    t4 = calls + 1
    calls = t4
    Return counter.count
    EndFunc

main:
    BeginFunc
    t5 = LCall counter
    t6 = LCall counter
    t7 = LCall counter
    c = t7
    PushParam c
    t8 = LCall helper
    PopParams 1
    h = t8
    t9 = h + calls
    Return t9
    EndFunc
