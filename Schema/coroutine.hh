#pragma once

#include <vector>
#include <sstream>
#include <utility>

#include "delegate.hpp"
#include "json_reference.hh"

namespace Json {

class Schema;

class ValidationCoroutine {
public:
    bool do_test(const ValueRef &) const;
    std::string get_report() const;
    virtual ~ValidationCoroutine() {}

protected:
    ValidationCoroutine() = default;

    void m_report(const ValueRef & json, const std::string & str) const;
    std::stringstream & m_report(const ValueRef & json) const;

    template<typename Type>
    typename std::enable_if_t<std::is_base_of<ValidationCoroutine, Type>::value, void>
        push_back_function(Type & t, bool(Type::* const method_ptr)(const ValueRef &) const);

    std::vector<delegate<bool(const ValueRef &)>> m_functs;

    mutable std::stringstream m_report_stream;
private:
    ValidationCoroutine(const ValidationCoroutine &) = delete;
    ValidationCoroutine & operator=(const ValidationCoroutine &) = delete;
};

} // namespace Json