
/*
 * Copyright (C) 2007-2009, libturglem development team.
 * This file is released under the LGPL.
 *
 */

#ifndef __MAFSA_POOL_HPP__
#define __MAFSA_POOL_HPP__

#include <MAFSA/stack.hpp>

namespace MAFSA {

/* Memory allocator for fixed size structures */

template <typename _T, int objects_per_page = 65536>
class pool
{
protected:
    struct page
    {
        _T *data;
        _T *free_node;
        page *next;

         page();
        ~page();

        bool full() const;
        void attach(page * p);
        _T *allocate();
    };

    stack<_T*> free_nodes;
    page *root;

    uint16_t pages_num;
    uint32_t objects_num;

public:
     pool();
    ~pool();

    uint16_t allocated_pages   () const;
    uint32_t allocated_objects () const;
    uint32_t allocated_bytes   () const;
    size_t   page_size         () const;

    _T *allocate();
    void free(_T *elem);
};

#include "pool.tcc"

} /* namespace MAFSA */

#endif /* __MAFSA_POOL_HPP__ */
