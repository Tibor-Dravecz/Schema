#include "validation_helper.hh"
#include "coroutine.hpp"
#include "integer_validation.hh"

#include "of_validation.hpp"
#include "size_validation.hpp"

using std::to_string;

namespace Json {

IntegerValidation::IntegerValidation(const ValueRef & schema, bool allow_multi) :
    OfValidationCoroutine(schema, allow_multi),
    SizeValidationCoroutine(schema, "imum") {
    push_back_function(*this, &IntegerValidation::test_is_integer);
    push_back_all_size_tests(schema);
    if (schema("multipleOf").isInt64()) {
        m_muliple_of = schema("multipleOf").asInt64();
        push_back_function(*this, &IntegerValidation::test_multiple_of);
    }
    if (allow_multi) {
        add_all_of_tests();
    }
}

bool IntegerValidation::test_is_integer(const ValueRef & json) const {
    if (!json->isInt64()) {
        m_report(json, "is not an integer!");
        return false;
    }
    return true;
}

bool IntegerValidation::test_multiple_of(const ValueRef & json) const {
    if (json->asInt64() % m_muliple_of != 0) {
        m_report(json) << " is not multiple of " << to_string(m_muliple_of) << "!";
        return false;
    }
    return true;
}

} // namespace Json