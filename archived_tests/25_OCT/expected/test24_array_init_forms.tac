
main:
    BeginFunc
    k = 5
    cube[0] = 1
    cube[1] = 2
    cube[2] = 3
    cube[3] = 4
    cube[4] = 5
    cube[5] = 6
    cube[6] = 7
    cube[7] = 8
    part[0] = 1
    part[3] = 4
    part[4] = 5
    names[0] = "ab"
    names[8] = "cd"
    t1 = k + 1
    t2 = k * 2
    dyn[0] = k
    dyn[1] = t1
    dyn[2] = t2
    t3 = cube[7]
    t4 = part[4]
    t5 = t3 + t4
    t6 = dyn[2]
    t7 = t5 + t6
    Return t7
    EndFunc
