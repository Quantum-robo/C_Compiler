
_global_init:
    g = 3
    table[0] = 1
    table[1] = 2
    table[2] = 3
    table[3] = 4
    t1 = &greeting
    PushParam "hi"
    PushParam t1
    LCall strcpy
    PopParams 2
    t2 = &origin
    *t2 = 5
    *(t2 + 4) = 6
    N = 3
    main.calls = 0

main:
    BeginFunc
    t3 = main.calls
    t4 = main.calls + 1
    main.calls = t4
    t5 = table[2]
    t6 = g + t5
    t7 = &origin
    t8 = *(t7 + 4)
    t9 = t6 + t8
    t10 = greeting[1] :1
    t11 = t9 + t10
    sized[1] = t11
    t12 = sized[1]
    buf[0] = t12
    t13 = buf[0]
    t14 = t13 + main.calls
    Return t14
    EndFunc

# ---- tables ----
# struct Pt size=8 align=4: x@0:4 y@4:4
# global   g:int(4)
# global   table:int[4](16)
# global   greeting:char[8](8)
# global   origin:Pt(8)
# global   N:const int(4)
# global   sized:int[3](12)
# global   main.calls:int(4) static
# function main -> int
#   local  buf:int[3](12)
