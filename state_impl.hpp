#include <boost/type_index.hpp>
#include "state.hpp"
#include "event.hpp"

namespace Nikos {

using boost::typeindex::type_id_with_cvr;

template <typename T>
State * ConcState<T>::processEvent(Event *e)
{
    return e->triggerStateChange(static_cast<T*>(this));
}

template <typename T>
State * ConcState<T>::getInst()
{
    static State *inst = nullptr;
    
    if (!inst)
        inst = new T();
        
    return inst;
}

template <typename T>
const std::string ConcState<T>::toStr()
{
    return type_id_with_cvr<T>().pretty_name();
}

}