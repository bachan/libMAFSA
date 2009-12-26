
/*
 * Copyright (C) 2007-2009, libturglem development team.
 * This file is released under the LGPL.
 *
 */

#ifndef __MAFSA_STACK_HPP__
#define __MAFSA_STACK_HPP__

namespace MAFSA {

template <typename _DATA>
class stack
{
    _DATA *buffer;
    size_t reserved, current;

public:
     stack(size_t reserve = 1024) : buffer(new _DATA[reserve]), reserved(reserve), current(-1) {}
    ~stack() { delete [] buffer; }

    void push(_DATA l)
    {
        if (current + 1 >= reserved)
        {
            _DATA *_t = new _DATA [2 * reserved];
            memcpy(_t, buffer, reserved * sizeof(_DATA));

            reserved *= 2;
            delete [] buffer;

            buffer = _t;
        }

        buffer[++current] = l;
    }

    _DATA   pop() { return current != (size_t) -1 ? buffer[current--] : buffer[0]; }
    _DATA *data() { return buffer; }

    size_t size()     const { return current + 1; }
    size_t capacity() const { return reserved;    }

    bool empty() const { return size() == 0; }
    void erase()       {        current = 0; }
};

} /* namespace MAFSA */

#endif /* __MAFSA_STACK_HPP__ */
