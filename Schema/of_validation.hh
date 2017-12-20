#pragma once

#include "coroutine.hh"

namespace Json {

template<typename T>
class OfValidationCoroutine : virtual public ValidationCoroutine {
protected:
    OfValidationCoroutine(const ValueRef & schema, bool allow_multi);
    void reset(const ValueRef & schema, bool allow_multi);
    void add_all_of_tests();
    virtual ~OfValidationCoroutine();
private:
    bool test_all_of(const ValueRef & json) const;
    bool test_any_of(const ValueRef & json) const;
    bool test_one_of(const ValueRef & json) const;
    bool test_not(const ValueRef & json) const;

    std::vector<T *> m_all_of;
    std::vector<T *> m_any_of;
    std::vector<T *> m_one_of;
    std::vector<T *> m_not;
};

} // namespace Json