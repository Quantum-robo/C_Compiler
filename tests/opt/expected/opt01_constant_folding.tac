
fold:
    BeginFunc
    t22 = 10 / z
    t23 = 64 + t22
    t24 = 10 + t23
    Return t24
    EndFunc

main:
    BeginFunc
    PushParam 1
    t25 = LCall fold
    PopParams 1
    Return t25
    EndFunc
