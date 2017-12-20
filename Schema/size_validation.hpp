#pragma once

#include "size_validation.hh"
#include "validation_helper.hh"

namespace Json {

template<typename T>
inline SizeValidationCoroutine<T>::SizeValidationCoroutine(const ValueRef & schema, const std::string & name) {
    m_name = name;
    if (get::is<T>(schema("min" + m_name))) {
        m_min = get::as<T>(schema("min" + m_name));
        m_exclusive_min = get::is_as_bool(schema("exclusiveMinimum"));
    }
    if (get::is<T>(schema("max" + m_name))) {
        m_max = get::as<T>(schema("max" + m_name));
        m_exclusive_max = get::is_as_bool(schema("exclusiveMaximum"));
    }
}

template<typename T>
inline void SizeValidationCoroutine<T>::push_back_all_size_tests(const ValueRef & schema) {
    if (get::is<T>(schema("min" + m_name))) {
        push_back_function(*this, &SizeValidationCoroutine<T>::test_size_min);
    }
    if (get::is<T>(schema("max" + m_name))) {
        push_back_function(*this, &SizeValidationCoroutine<T>::test_size_max);
    }
}

template<typename T>
bool SizeValidationCoroutine<T>::test_size_min(const ValueRef & json) const {
    if (m_exclusive_min) {
        if (get::as<T>(json("min" + m_name)) <= m_min) {
            return false;
        }
    } else  if (get::as<T>(json("min" + m_name)) < m_min) {
        return false;
    }
    return true;
}

template<typename T>
bool SizeValidationCoroutine<T>::test_size_max(const ValueRef & json) const {
    if (m_exclusive_max) {
        if (get::as<T>(json("max" + m_name)) >= m_max) {
            return false;
        }
    } else  if (get::as<T>(json("max" + m_name)) > m_max) {
        return false;
    }
    return true;
}

} // namespace Json