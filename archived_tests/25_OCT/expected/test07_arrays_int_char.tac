
main:
    BeginFunc
    nums[0] = 10
    nums[1] = 20
    nums[2] = 30
    nums[3] = 40
    nums[4] = 50
    word = "hello"
    sum = 0
    i = 0
L1:
    t1 = i < 5
    IfFalse t1 Goto L4
    Goto L2
L3:
    t2 = i
    t3 = i + 1
    i = t3
    Goto L1
L2:
    t4 = i * i
    squares[i] = t4
    t5 = nums[i]
    t6 = sum + t5
    sum = t6
    Goto L3
L4:
    letters[0] = 'a'
    letters[1] = 'b'
    letters[2] = '\0'
    t7 = letters[1]
    word[0] = t7
    t8 = nums[1]
    t9 = nums[3]
    t10 = t8 + t9
    nums[2] = t10
    t11 = sum % 5
    t12 = nums[t11]
    t13 = t12 + 1
    nums[t11] = t13
    Return sum
    EndFunc
