#pragma once

#include <algorithm>

#include "of_validation.hh"

namespace Json {

template<typename T>
inline OfValidationCoroutine<T>::OfValidationCoroutine(const ValueRef & schema, bool allow_multi)
    : ValidationCoroutine() {
    reset(schema, allow_multi);
}

template<typename T>
inline void OfValidationCoroutine<T>::reset(const ValueRef & schema, bool allow_multi) {
    m_all_of.clear();
    m_any_of.clear();
    m_one_of.clear();
    m_not.clear();
    if (allow_multi) {
        if (schema("allOf").isArray()) {
            const auto all_of = schema["allOf"];
            for (ArrayIndex i = 0; i < all_of->size(); i++) {
                m_all_of.push_back(new T(all_of[i], true));
            }
        }
        if (schema("anyOf").isArray()) {
            const auto any_of = schema["anyOf"];
            for (ArrayIndex i = 0; i < any_of->size(); i++) {
                m_any_of.push_back(new T(any_of[i], true));
            }
        }
        if (schema("oneOf").isArray()) {
            const auto one_of = schema["oneOf"];
            for (ArrayIndex i = 0; i < one_of->size(); i++) {
                m_one_of.push_back(new T(one_of[i], true));
            }
        }
        if (schema("not").isObject()) {
            m_not.push_back(new T(schema["not"], true));
        }
    }
}

template<typename T>
void delete_vector(std::vector<T *> & vec) {
    for (auto * ptr : vec) {
        if (ptr) {
            delete ptr;
        }
    }
}

template<typename T>
inline OfValidationCoroutine<T>::~OfValidationCoroutine() {
    delete_vector(m_all_of);
    delete_vector(m_any_of);
    delete_vector(m_one_of);
    delete_vector(m_not);
}

template<typename T>
inline void OfValidationCoroutine<T>::add_all_of_tests() {
    if (m_all_of.size() > 0) {
        push_back_function(*this, &OfValidationCoroutine<T>::test_all_of);
    }
    if (m_any_of.size() > 0) {
        push_back_function(*this, &OfValidationCoroutine<T>::test_any_of);
    }
    if (m_one_of.size() > 0) {
        push_back_function(*this, &OfValidationCoroutine<T>::test_one_of);
    }
    if (m_not.size() > 0) {
        push_back_function(*this, &OfValidationCoroutine<T>::test_not);
    }
}

template<typename T>
inline bool OfValidationCoroutine<T>::test_all_of(const ValueRef & json) const {
    // const T * failed;
    const auto result = std::all_of(m_all_of.begin(), m_all_of.end(), [&json](const T * coroutine) {
        // failed = &coroutine;
        return coroutine->do_test(json);
    });
    if (result == false) {
        m_report(json, "allOf validation failed!");
    }
    return result;
}

template<typename T>
inline bool OfValidationCoroutine<T>::test_any_of(const ValueRef & json) const {
    const auto result = std::any_of(m_any_of.begin(), m_any_of.end(), [&json](const T * coroutine) {
        return coroutine->do_test(json);
    });
    if (result == false) {
        m_report(json, "anyOf validation failed!");
    }
    return result;
}

template<typename T>
inline bool OfValidationCoroutine<T>::test_one_of(const ValueRef & json) const {
    const auto result = std::count_if(m_one_of.begin(), m_one_of.end(), [&json](const T * coroutine) {
        return coroutine->do_test(json);
    });
    if (result != 1) {
        m_report(json, "oneOf validation failed!");
    }
    return result == 1;
}

template<typename T>
inline bool OfValidationCoroutine<T>::test_not(const ValueRef & json) const {
    const auto result = m_not[0]->do_test(json);
    if (result == true) {
        m_report(json, "not validation failed!");
    }
    return !result;
}

} // namespace Json