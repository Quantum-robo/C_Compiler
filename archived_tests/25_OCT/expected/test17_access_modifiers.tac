
Account::setPin:
    BeginFunc
    *this = p
    EndFunc

Account::deposit:
    BeginFunc
    t1 = amt > 0
    IfFalse t1 Goto L1
    t2 = *(this + 4) :f4
    t3 = t2 + amt
    *(this + 4) = t3 :f4
L1:
    EndFunc

Account::getBalance:
    BeginFunc
    t4 = *(this + 4) :f4
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
    t8 = 500.5
    t9 = &acc
    PushParam t8
    PushParam t9
    t10 = LCall Account::deposit
    PopParams 2
    t11 = &acc
    PushParam t11
    t12 = LCall Account::getBalance
    PopParams 1
    b = t12
    t13 = &acc
    t14 = *(t13 + 12)
    Return t14
    EndFunc
