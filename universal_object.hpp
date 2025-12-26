#ifndef UNIVERSAL_OBJECT_HPP
#define UNIVERSAL_OBJECT_HPP

#include "declorations.hpp"

template <typename ENUM_STATE>
class Universal_object
{
private://protected:
    ENUM_STATE status;

public:
    virtual ENUM_STATE getStatus() const { return status; }
    virtual void setStatus(ENUM_STATE status_) { status = status_; }
    //ф-иф для вызова и обновления внутренних данных
    virtual void update();
};

template <typename ENUM_STATE>
inline void Universal_object<ENUM_STATE>::update()
{

}

#endif // UNIVERSAL_OBJECT_HPP
