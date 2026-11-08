
main:
    BeginFunc
    k = 5
    Zero cube :32
    cube[0] = 1
    cube[1] = 2
    cube[2] = 3
    cube[3] = 4
    cube[4] = 5
    cube[5] = 6
    cube[6] = 7
    cube[7] = 8
    Zero part :24
    part[0] = 1
    part[3] = 4
    part[4] = 5
    Zero names :16
    t1 = &names[0] :1
    PushParam "ab"
    PushParam t1
    LCall strcpy
    PopParams 2
    t2 = &names[8] :1
    PushParam "cd"
    PushParam t2
    LCall strcpy
    PopParams 2
    t3 = k + 1
    t4 = k * 2
    Zero dyn :12
    dyn[0] = k
    dyn[1] = t3
    dyn[2] = t4
    t5 = cube[7]
    t6 = part[4]
    t7 = t5 + t6
    t8 = dyn[2]
    t9 = t7 + t8
    Return t9
    EndFunc
