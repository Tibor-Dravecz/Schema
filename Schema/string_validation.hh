#pragma once

#include <memory>

#include "of_validation.hh"
#include "size_validation.hh"

namespace Json {

class ForamtImpl;

class StringValidation : public OfValidationCoroutine<StringValidation>, public SizeValidationCoroutine<size_t> {
public:
    StringValidation(const ValueRef & schema, bool allow_mulit);
    virtual ~StringValidation() {}
private:
    bool test_is_string(const ValueRef & json) const;
    bool test_patten(const ValueRef & json) const;
    bool test_format(const ValueRef & json) const;
    bool test_format_array(const ValueRef & json) const;

    std::string m_pattern;
    std::vector<std::string> m_format;

    static std::unique_ptr<ForamtImpl> m_pformat;
};

} // namespace Json