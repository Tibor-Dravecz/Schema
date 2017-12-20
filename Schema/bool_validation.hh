#pragma once

#include "coroutine.hh"

namespace Json {

class BoolValidation : public ValidationCoroutine {
public:
    BoolValidation(const ValueRef & schema);
    virtual ~BoolValidation() {}
private:
    bool test_is_bool(const ValueRef & json) const;
};

} // namespace Json