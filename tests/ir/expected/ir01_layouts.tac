
main:
    BeginFunc
    t1 = &m
    *t1 = 'x' :1
    t2 = &m
    *(t2 + 4) = 100
    t3 = &m
    *(t3 + 8) = 'y' :1
    t4 = &m
    *(t4 + 10) = 7 :2
    t5 = &o
    *t5 = 1 :1
    t6 = &o
    *(t6 + 4) = 2 :1
    t7 = &o
    t8 = t7 + 8
    t8[1] = 3
    t9 = &o
    t10 = t9 + 16
    t11 = &t10[1] :12
    t12 = t11 + 4
    t12[0] = 4
    t13 = &u
    *t13 = 5
    total = 64
    t14 = &m
    t15 = &o
    t16 = t15 + 8
    t17 = *(t14 + 4)
    t18 = t16[1]
    t19 = t17 + t18
    t20 = &o
    t21 = t20 + 16
    t22 = &t21[1] :12
    t23 = t22 + 4
    t24 = t23[0]
    t25 = t19 + t24
    t26 = &u
    t27 = *t26
    t28 = t25 + t27
    t29 = t28 + total
    Return t29
    EndFunc

# ---- tables ----
# struct Mixed size=12 align=4: c@0:1 i@4:4 d@8:1 s@10:2
# struct Inner size=12 align=4: tag@0:1 vals@4:8
# struct Outer size=40 align=4: flag@0:1 in@4:12 list@16:24
# union U size=8 align=4: c@0:1 i@0:4 s@0:6
# function main -> int
#   local  m:Mixed(12)
#   local  o:Outer(40)
#   local  u:U(8)
#   local  total:int(4)
