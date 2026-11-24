
work:
    BeginFunc
    t3 = n + 1
    PushParam t3
    PushParam "%d\n"
    LCall printf
    PopParams 2
    Return t3
    EndFunc

main:
    BeginFunc
    PushParam 4
    LCall work
    PopParams 1
    Return 0
    EndFunc
