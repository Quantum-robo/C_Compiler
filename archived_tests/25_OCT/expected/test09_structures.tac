
area:
    BeginFunc
    t1 = r.bottomRight.x - r.topLeft.x
    w = t1
    t2 = r.bottomRight.y - r.topLeft.y
    h = t2
    t3 = w * h
    Return t3
    EndFunc

main:
    BeginFunc
    p.x = 3
    p.y = 4
    t4 = &p
    pp = t4
    t5 = pp->x + 1
    pp->x = t5
    r.topLeft.x = 0
    r.topLeft.y = 0
    r.bottomRight.x = p.x
    r.bottomRight.y = p.y
    PushParam r
    t6 = LCall area
    PopParams 1
    a = t6
    Return a
    EndFunc
