#include <regex>

#include "validation_helper.hh"
#include "coroutine.hpp"
#include "string_validation.hh"

#include "of_validation.hpp"
#include "size_validation.hpp"

using std::string;
using std::regex;
using std::map;
using std::smatch;
using std::regex_search;
using std::any_of;
using std::unique_ptr;

namespace Json {

class ForamtImpl {
    map<string, regex> m_map;
public:
    ForamtImpl() {
        m_map["date-time"] = regex(".*");
        m_map["email"] = regex(".*");
        m_map["hostname"] = regex(".*");
        m_map["ipv4"] = regex("^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$");
        m_map["ipv6"] = regex("^s*((([0-9A-Fa-f]{1,4}:){7}([0-9A-Fa-f]{1,4}|:))|(([0-9A-Fa-f]{1,4}:){6}(:[0-9A-Fa-f]{1,4}|((25[0-5]|2[0-4]d|1dd|[1-9]?d)(.(25[0-5]|2[0-4]d|1dd|[1-9]?d)){3})|:))|(([0-9A-Fa-f]{1,4}:){5}(((:[0-9A-Fa-f]{1,4}){1,2})|:((25[0-5]|2[0-4]d|1dd|[1-9]?d)(.(25[0-5]|2[0-4]d|1dd|[1-9]?d)){3})|:))|(([0-9A-Fa-f]{1,4}:){4}(((:[0-9A-Fa-f]{1,4}){1,3})|((:[0-9A-Fa-f]{1,4})?:((25[0-5]|2[0-4]d|1dd|[1-9]?d)(.(25[0-5]|2[0-4]d|1dd|[1-9]?d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){3}(((:[0-9A-Fa-f]{1,4}){1,4})|((:[0-9A-Fa-f]{1,4}){0,2}:((25[0-5]|2[0-4]d|1dd|[1-9]?d)(.(25[0-5]|2[0-4]d|1dd|[1-9]?d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){2}(((:[0-9A-Fa-f]{1,4}){1,5})|((:[0-9A-Fa-f]{1,4}){0,3}:((25[0-5]|2[0-4]d|1dd|[1-9]?d)(.(25[0-5]|2[0-4]d|1dd|[1-9]?d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){1}(((:[0-9A-Fa-f]{1,4}){1,6})|((:[0-9A-Fa-f]{1,4}){0,4}:((25[0-5]|2[0-4]d|1dd|[1-9]?d)(.(25[0-5]|2[0-4]d|1dd|[1-9]?d)){3}))|:))|(:(((:[0-9A-Fa-f]{1,4}){1,7})|((:[0-9A-Fa-f]{1,4}){0,5}:((25[0-5]|2[0-4]d|1dd|[1-9]?d)(.(25[0-5]|2[0-4]d|1dd|[1-9]?d)){3}))|:)))(%.+)?s*");
        m_map["uri"] = regex(".*");
    }
    bool validate_format(const string & key, const string & str) {
        const auto it = m_map.find(key);
        if (it == m_map.end()) {
            return false;
        }
        smatch sm;
        return regex_search(str, sm, it->second);
    }
    bool validate_regex(const string & regex_str, const string & str) {
        regex re(regex_str);
        smatch sm;
        return regex_search(str, sm, re);
    }
};

unique_ptr<ForamtImpl> StringValidation::m_pformat(new ForamtImpl());

StringValidation::StringValidation(const ValueRef & schema, bool allow_multi) :
    OfValidationCoroutine(schema, allow_multi),
    SizeValidationCoroutine(schema, "Length") {
    push_back_function(*this, &StringValidation::test_is_string);
    push_back_all_size_tests(schema);
    if (schema("pattern").isString()) {
        m_pattern = schema("pattern").asString();
        push_back_function(*this, &StringValidation::test_patten);
    }
    if (schema("format").isString()) {
        m_format.push_back(schema("format").asString());
        push_back_function(*this, &StringValidation::test_format);
    }
    if (schema("format").isArray()) {
        for (const auto & json : schema("format")) {
            m_format.push_back(json.asString());
        }
        push_back_function(*this, &StringValidation::test_format_array);
    }
    if (allow_multi) {
        add_all_of_tests();
    }
}

bool StringValidation::test_is_string(const ValueRef & json) const {
    if (!json->isString()) {
        m_report(json, "is not a string!");
        return false;
    }
    return true;
}

bool StringValidation::test_patten(const ValueRef & json) const {
    if (!m_pformat->validate_regex(m_pattern, json->asString())) {
        m_report(json) << " doas not match patten: \"" << m_pattern << "\"!";
        return false;
    }
    return true;
}

bool StringValidation::test_format(const ValueRef & json) const {
    if (!m_pformat->validate_format(m_format[0], json->asString())) {
        m_report(json) << " doas not match format: \"" << m_format[0] << "\"!";
        return false;
    }
    return true;
}

bool StringValidation::test_format_array(const ValueRef & json) const {
    const auto str = json->asString();
    const auto result = any_of(m_format.begin(), m_format.end(), [&str](const Value & json) {
        return m_pformat->validate_format(json.asString(), str);
    });
    if (!result) {
        m_report(json) << " does not match any format: " << array_as_string(m_format);
        return false;
    }
    return true;
}

} // namespace Json