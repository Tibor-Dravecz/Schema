#include "coroutine.hpp"
#include "null_validation.hh"

namespace Json {

NullValidation::NullValidation(const ValueRef &, bool is_null)
    : ValidationCoroutine(), m_is_null(is_null) {
    push_back_function(*this, &NullValidation::test_is_null);
}

bool NullValidation::test_is_null(const ValueRef & json) const {
    if (json->isNull()) {
        if (!m_is_null) {
            m_report(json) << "is null!";
        }
        return m_is_null;
    }
    if (m_is_null) {
        m_report(json) << "is not null!";
    }
    return !m_is_null;
}

} // namespace Json