#pragma once

#include "of_validation.hh"
#include "size_validation.hh"

namespace Json {

class NumberValidation : public OfValidationCoroutine<NumberValidation>, public SizeValidationCoroutine<double> {
public:
    NumberValidation(const ValueRef & schema, bool allow_mulit);
    virtual ~NumberValidation() {}
private:
    bool test_is_integer(const ValueRef & json) const;
};

} // namespace Json