#pragma once

#include "coroutine.hh"

namespace Json {

template<typename Type>
typename std::enable_if_t<std::is_base_of<ValidationCoroutine, Type>::value, void>
ValidationCoroutine::push_back_function(Type & t, bool(Type::* const method_ptr)(const ValueRef &) const) {
    m_functs.push_back(delegate<bool(const ValueRef &)>(t, method_ptr));
}

} // namespace Json
