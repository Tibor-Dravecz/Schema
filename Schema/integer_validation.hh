#pragma once

#include "of_validation.hh"
#include "size_validation.hh"

namespace Json {

class IntegerValidation : public OfValidationCoroutine<IntegerValidation>, public SizeValidationCoroutine<Int64> {
public:
    IntegerValidation(const ValueRef & schema, bool allow_mulit);
    virtual ~IntegerValidation() {}
private:
    bool test_is_integer(const ValueRef & json) const;
    bool test_multiple_of(const ValueRef & json) const;

    Int64 m_muliple_of;
};

} // namespace Json