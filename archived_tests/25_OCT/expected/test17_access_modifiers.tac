
Account::setPin:
    BeginFunc
    this->pin = p
    EndFunc

Account::deposit:
    BeginFunc
    t1 = amt > 0
    IfFalse t1 Goto L1
    t2 = this->balance + amt
    this->balance = t2
L1:
    EndFunc

Account::getBalance:
    BeginFunc
    Return this->balance
    EndFunc

main:
    BeginFunc
    acc.id = 7
    t3 = &acc
    PushParam 1234
    PushParam t3
    t4 = LCall Account::setPin
    PopParams 2
    t5 = &acc
    PushParam 500.5
    PushParam t5
    t6 = LCall Account::deposit
    PopParams 2
    t7 = &acc
    PushParam t7
    t8 = LCall Account::getBalance
    PopParams 1
    b = t8
    Return acc.id
    EndFunc
