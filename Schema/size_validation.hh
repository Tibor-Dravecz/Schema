#pragma once

#include <string>

#include "coroutine.hh"

namespace Json {

template<typename T>
class SizeValidationCoroutine : virtual public ValidationCoroutine {
protected:
    SizeValidationCoroutine(const ValueRef & schema, const std::string & name);
    void push_back_all_size_tests(const ValueRef & schema);
    virtual ~SizeValidationCoroutine() {}
private:
    bool test_size_min(const ValueRef & json) const;
    bool test_size_max(const ValueRef & json) const;

    T m_min = 0;
    bool m_exclusive_min = false;

    T m_max = 0;
    bool m_exclusive_max = false;

    std::string  m_name;
};

} // namespace Json