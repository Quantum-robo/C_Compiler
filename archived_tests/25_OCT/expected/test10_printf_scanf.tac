
main:
    BeginFunc
    PushParam "Enter two numbers: "
    t1 = LCall printf
    PopParams 1
    t2 = &a
    t3 = &b
    PushParam t3
    PushParam t2
    PushParam "%d %d"
    t4 = LCall scanf
    PopParams 3
    t5 = &f
    PushParam t5
    PushParam "%f"
    t6 = LCall scanf
    PopParams 2
    PushParam name
    PushParam "%s"
    t7 = LCall scanf
    PopParams 2
    t8 = a + b
    t9 = a * b
    PushParam t9
    PushParam t8
    PushParam "Sum = %d, Product = %d\n"
    t10 = LCall printf
    PopParams 3
    PushParam name
    PushParam f
    PushParam "Float: %f Name: %s\n"
    t11 = LCall printf
    PopParams 3
    PushParam "Done\n"
    t12 = LCall printf
    PopParams 1
    Return 0
    EndFunc
