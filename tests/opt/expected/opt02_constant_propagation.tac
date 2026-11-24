
main:
    BeginFunc
    PushParam 30
    PushParam "big %d\n"
    LCall printf
    PopParams 2
    Return 0
    EndFunc
