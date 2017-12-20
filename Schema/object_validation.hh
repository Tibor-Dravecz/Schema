#pragma once

#include "of_validation.hh"
#include "size_validation.hh"

namespace Json {

class ObjectValidation : public OfValidationCoroutine<ObjectValidation>, public SizeValidationCoroutine<size_t> {
public:
    ObjectValidation(const ValueRef & schema, bool allow_mulit);
    virtual ~ObjectValidation();
private:
    bool test_is_object(const ValueRef &) const;
    bool test_property_names(const ValueRef &) const;
    bool test_dependencies(const ValueRef &) const;
    bool test_required_names(const ValueRef &) const;
    bool test_additional_items(const ValueRef &) const;
    bool test_properties(const ValueRef &) const;
    bool test_patten_properties(const ValueRef &) const;

    std::vector<std::string> m_schema_names;
    std::vector<std::string> m_property_names;
    std::map<std::string, std::vector<std::string>> m_dependencies;
    std::vector<std::string> m_required_names;
    std::vector<std::string> m_additional_names;

    std::map<std::string, Schema *> m_properties;
    std::map<std::string, Schema *> m_patten_properties;


    mutable std::vector<std::string> m_json_names;
    mutable size_t m_size;
};

} // namespace Json