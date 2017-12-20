#include "validation_helper.hh"
#include "coroutine.hpp"
#include "number_validation.hh"

#include "of_validation.hpp"
#include "size_validation.hpp"

using std::to_string;

namespace Json {

NumberValidation::NumberValidation(const ValueRef & schema, bool allow_multi) :
    OfValidationCoroutine(schema, allow_multi),
    SizeValidationCoroutine(schema, "imum") {
    push_back_function(*this, &NumberValidation::test_is_integer);
    push_back_all_size_tests(schema);

    if (allow_multi) {
        add_all_of_tests();
    }
}

bool NumberValidation::test_is_integer(const ValueRef & json) const {
    if (!json->isInt64() && !json->isDouble()) {
        m_report(json, "is not an integer!");
        return false;
    }
    return true;
}

} // namespace Json