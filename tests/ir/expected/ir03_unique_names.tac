
_global_init:
    x = 1
    counter.n = 0
    other.n = 10

counter:
    BeginFunc
    t1 = counter.n
    t2 = counter.n + 1
    counter.n = t2
    Return counter.n
    EndFunc

other:
    BeginFunc
    t3 = other.n
    t4 = other.n + 1
    other.n = t4
    Return t3
    EndFunc

main:
    BeginFunc
    total = x
    x.1 = 5
    t5 = total + x.1
    total = t5
    x.2 = 6
    t6 = total + x.2
    total = t6
    x.3 = 7
    t7 = total + x.3
    total = t7
    i = 0
L1:
    t8 = i < 2
    IfFalse t8 Goto L4
    Goto L2
L3:
    t9 = i
    t10 = i + 1
    i = t10
    Goto L1
L2:
    t11 = total + i
    total = t11
    Goto L3
L4:
    i.2 = 0
L5:
    t12 = i.2 < 3
    IfFalse t12 Goto L8
    Goto L6
L7:
    t13 = i.2
    t14 = i.2 + 1
    i.2 = t14
    Goto L5
L6:
    t15 = total + i.2
    total = t15
    Goto L7
L8:
    shadow = 0
    shadow.2 = 2
    t16 = total + shadow.2
    total = t16
    t17 = LCall counter
    t18 = total + t17
    t19 = LCall other
    t20 = t18 + t19
    t21 = t20 + x
    Return t21
    EndFunc

# ---- tables ----
# global   x:int(4)
# global   counter.n:int(4) static
# global   other.n:int(4) static
# function counter -> int
# function other -> int
# function main -> int
#   local  total:int(4)
#   local  x.1:int(4)
#   local  x.2:int(4)
#   local  x.3:int(4)
#   local  i:int(4)
#   local  i.2:int(4)
#   local  shadow:int(4)
#   local  shadow.2:int(4)
