
main:
    BeginFunc
    Zero nums :20
    nums[0] = 10
    nums[1] = 20
    nums[2] = 30
    nums[3] = 40
    nums[4] = 50
    Zero word :6
    t1 = &word
    PushParam "hello"
    PushParam t1
    LCall strcpy
    PopParams 2
    sum = 0
    i = 0
L1:
    t2 = i < 5
    IfFalse t2 Goto L4
    Goto L2
L3:
    t3 = i
    t4 = i + 1
    i = t4
    Goto L1
L2:
    t5 = i * i
    squares[i] = t5
    t6 = nums[i]
    t7 = sum + t6
    sum = t7
    Goto L3
L4:
    letters[0] = 'a' :1
    letters[1] = 'b' :1
    letters[2] = '\0' :1
    t8 = letters[1] :1
    word[0] = t8 :1
    t9 = nums[1]
    t10 = nums[3]
    t11 = t9 + t10
    nums[2] = t11
    t12 = sum % 5
    t13 = nums[t12]
    t14 = t13 + 1
    nums[t12] = t14
    Return sum
    EndFunc
