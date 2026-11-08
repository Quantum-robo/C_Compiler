
Node::get:
    BeginFunc
    t1 = *this
    Return t1
    EndFunc

Node::nextVal:
    BeginFunc
    t2 = *(this + 4)
    t3 = *t2
    Return t3
    EndFunc

Node::posb:
    BeginFunc
    t4 = *(this + 12)
    Return t4
    EndFunc

Node::spawn:
    BeginFunc
    PushParam 16
    t5 = LCall malloc
    PopParams 1
    n = t5
    *n = 16
    Return n
    EndFunc

Node::link:
    BeginFunc
    *(this + 4) = other
    EndFunc

main:
    BeginFunc
    t6 = &a
    PushParam t6
    t7 = LCall Node::spawn
    PopParams 1
    b = t7
    t8 = &a
    PushParam b
    PushParam t8
    t9 = LCall Node::link
    PopParams 2
    t10 = &a
    PushParam t10
    t11 = LCall Node::get
    PopParams 1
    t12 = &a
    PushParam t12
    t13 = LCall Node::nextVal
    PopParams 1
    t14 = t11 + t13
    t15 = &a
    PushParam t15
    t16 = LCall Node::posb
    PopParams 1
    t17 = t14 + t16
    Return t17
    EndFunc

# ---- tables ----
# struct P2 size=8 align=4: a@0:4 b@4:4
# class Node size=16 align=4: val@0:4 next@4:4 pos@8:8
# function Node::get -> int
#   param  this:Node*(4)
# function Node::nextVal -> int
#   param  this:Node*(4)
# function Node::posb -> int
#   param  this:Node*(4)
# function Node::spawn -> Node*
#   param  this:Node*(4)
#   local  n:Node*(4)
# function Node::link -> void
#   param  this:Node*(4)
#   param  other:Node*(4)
# function main -> int
#   local  a:Node(16)
#   local  b:Node*(4)
