#ifndef __CPP_DEFAULT_DELETE_HPP__ 
#define __CPP_DEFAULT_DELETE_HPP__ 

namespace Cpp
{
    template<class T>
    struct DefaultDelete
    {
        void operator()(T* Block) { delete Block; }
    };
}

#endif //__CPP_DEFAULT_DELETE_HPP__