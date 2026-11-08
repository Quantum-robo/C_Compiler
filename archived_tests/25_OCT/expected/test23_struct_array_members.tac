
main:
    BeginFunc
    t1 = &b
    p = t1
    pr.first = 7
    pr.second = 9
    b.len = 4
    b.data[0] = 1
    p->data[1] = 2
    t2 = p->len - 1
    p->data[t2] = 3
    t3 = p->data[1]
    t4 = b.data[0]
    t5 = t3 + t4
    v = t5
    t6 = v + pr.second
    Return t6
    EndFunc
