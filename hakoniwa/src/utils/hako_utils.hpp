#ifndef _HAKO_UTILS_HPP_
#define _HAKO_UTILS_HPP_

#include <iostream>
#include <stdlib.h>

static inline void HAKO_ABORT(const char* errmsg)
{
    std::cerr << "ERROR: " << errmsg << std::endl;
    exit(1);
}


#endif /* _HAKO_UTILS_HPP_*/