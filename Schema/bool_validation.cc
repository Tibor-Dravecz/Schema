#include "bool_validation.hh"
#include "coroutine.hpp"

namespace Json {

BoolValidation::BoolValidation(const ValueRef &)
    : ValidationCoroutine() {
    push_back_function(*this, &BoolValidation::test_is_bool);
}

bool BoolValidation::test_is_bool(const ValueRef & json) const {
    if (!json->isBool()) {
        m_report(json, "is not a bool!");
        return false;
    }
    return true;
}

} // namespace Json