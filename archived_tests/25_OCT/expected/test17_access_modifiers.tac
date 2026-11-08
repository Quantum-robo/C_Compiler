
Account::setPin:
    BeginFunc
    *this = p
    EndFunc

Account::deposit:
    BeginFunc
    t1 = amt > 0
    IfFalse t1 Goto L1
    t2 = *(this + 4)
    t3 = t2 + amt
    *(this + 4) = t3
L1:
    EndFunc

Account::getBalance:
    BeginFunc
    t4 = *(this + 4)
    Return t4
    EndFunc

main:
    BeginFunc
    t5 = &acc
    *(t5 + 12) = 7
    t6 = &acc
    PushParam 1234
    PushParam t6
    t7 = LCall Account::setPin
    PopParams 2
    t8 = &acc
    PushParam 500.5
    PushParam t8
    t9 = LCall Account::deposit
    PopParams 2
    t10 = &acc
    PushParam t10
    t11 = LCall Account::getBalance
    PopParams 1
    b = t11
    t12 = &acc
    t13 = *(t12 + 12)
    Return t13
    EndFunc
