#pragma once

#include "coroutine.hh"

namespace Json {

class NullValidation : public ValidationCoroutine {
public:
    NullValidation(const ValueRef & schema, bool is_null);
    virtual ~NullValidation() {}
private:
    bool test_is_null(const ValueRef & json) const;
    bool m_is_null;
};

} // namespace Json