#pragma once

#include "of_validation.hh"
#include "size_validation.hh"

namespace Json {

class ArrayValidation : public OfValidationCoroutine<ArrayValidation>, public SizeValidationCoroutine<ArrayIndex> {
public:
    ArrayValidation(const ValueRef & schema, bool allow_mulit);
    virtual ~ArrayValidation();
private:
    bool test_is_array(const ValueRef & json) const;
    bool test_unique_items(const ValueRef & json) const;
    bool test_contains(const ValueRef & json) const;
    bool test_items_object(const ValueRef & json) const;
    bool test_items_array(const ValueRef & json) const;
    // size_t m_size;

    bool m_allow_additional_items;
    std::vector<Schema *> m_items;
};

} // namespace Json