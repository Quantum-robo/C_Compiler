
main:
    BeginFunc
    Zero nums :16
    nums[0] = 1
    nums[1] = 2
    nums[2] = 3
    nums[3] = 4
    Zero text :6
    t1 = &text
    PushParam "hello"
    PushParam t1
    LCall strcpy
    PopParams 2
    Zero grid :24
    grid[0] = 1
    grid[1] = 2
    grid[2] = 3
    grid[3] = 4
    grid[4] = 5
    grid[5] = 6
    ip = nums
    cp = text
    rp = recs
    t2 = ip + 8
    ip = t2
    t3 = ip
    t4 = ip - 4
    ip = t4
    t5 = ip + 4
    ip = t5
    t6 = cp + 3
    cp = t6
    t7 = cp
    t8 = cp - 1
    cp = t8
    t9 = rp + 16
    rp = t9
    t10 = rp
    t11 = rp + 8
    rp = t11
    t12 = ip - nums
    t13 = t12 / 4
    d1 = t13
    t14 = rp - recs
    t15 = t14 / 8
    d2 = t15
    t16 = &grid[3]
    row = t16
    t17 = ip + 4
    t18 = *t17
    t19 = cp[1] :1
    t20 = t18 + t19
    t21 = &rp[1] :8
    t22 = *t21
    t23 = t20 + t22
    t24 = grid[5]
    t25 = t23 + t24
    v = t25
    t26 = d1 + d2
    t27 = t26 + v
    t28 = row + 4
    t29 = *t28
    t30 = t27 + t29
    Return t30
    EndFunc

# ---- tables ----
# struct Rec size=8 align=4: id@0:4 tag@4:1
# function main -> int
#   local  nums:int[4](16)
#   local  text:char[6](6)
#   local  recs:Rec[4](32)
#   local  grid:int[2][3](24)
#   local  ip:int*(4)
#   local  cp:char*(4)
#   local  rp:Rec*(4)
#   local  d1:long(4)
#   local  d2:long(4)
#   local  row:int*(4)
#   local  v:int(4)
