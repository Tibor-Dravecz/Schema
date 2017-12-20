#include <algorithm>

#include "validation_helper.hh"
#include "coroutine.hpp"
#include "object_validation.hh"
#include "validator.hh"

#include "of_validation.hpp"
#include "size_validation.hpp"

using std::string;
using std::vector;

using std::find;
using std::find_if;
using std::transform;
using std::make_pair;

namespace Json {

ObjectValidation::ObjectValidation(const ValueRef & schema, bool allow_multi) :
    OfValidationCoroutine(schema, allow_multi),
    SizeValidationCoroutine(schema, "Properties") {
    push_back_function(*this, &ObjectValidation::test_is_object);
    push_back_all_size_tests(schema);
    if (schema("propertyNames").isArray()) {
        m_property_names;
        for (const auto member : schema("propertyNames")) {
            m_property_names.push_back(member.asString());
        }
        push_back_function(*this, &ObjectValidation::test_property_names);
    }
    if (schema("dependencies").isObject()) {
        const auto deps = schema("dependencies").getMemberNames();
        for (const auto & member : deps) {
            vector<string> vec;
            const auto & list = schema("dependencies")[member];
            transform(list.begin(), list.end(), vec.begin(), [&vec](const Value & json) {
                return json.asString();
            });
            m_dependencies.emplace(make_pair(member, vec));
        }
        push_back_function(*this, &ObjectValidation::test_dependencies);
    }
    if (schema("required").isArray()) {
        for (const auto & member : schema("required")) {
            m_required_names.push_back(member.asString());
        }
        push_back_function(*this, &ObjectValidation::test_required_names);
    }
    if (schema("additionalItems").isBool() && !schema("additionalItems").asBool()) {
        push_back_function(*this, &ObjectValidation::test_additional_items);
    } else if (schema("additionalItems").isArray()) {
        if (schema("additionalItems").isArray()) {
            for (const auto & member : schema("additionalItems")) {
                m_additional_names.push_back(member.asString());
            }
        }
        push_back_function(*this, &ObjectValidation::test_additional_items);
    }
    if (schema("properties").isObject()) {
        const auto properties = schema["properties"];
        m_schema_names = properties->getMemberNames();
        for (const auto & name : m_schema_names) {
            m_properties.emplace(make_pair(name, new Schema(properties[name], true)));
        }
        push_back_function(*this, &ObjectValidation::test_properties);
    }
    if (schema("patternProperties").isObject()) {
        push_back_function(*this, &ObjectValidation::test_patten_properties);
    }
    if (allow_multi) {
        add_all_of_tests();
    }
}

ObjectValidation::~ObjectValidation() {
    for (const auto & pair : m_properties) {
        delete pair.second;
    }
}

bool ObjectValidation::test_is_object(const ValueRef & json) const {
    if (!json->isObject()) {
        m_report(json, "is not an object!");
        return false;
    }
    m_json_names = json->getMemberNames();
    m_size = m_json_names.size();
    return true;
}

bool ObjectValidation::test_property_names(const ValueRef & json) const {
    const auto it = find_if(m_json_names.begin(), m_json_names.end(), [this](const string & name) {
        return find(m_property_names.begin(), m_property_names.end(), name) == m_property_names.end();
    });
    if (it != m_json_names.end()) {
        m_report(json) << "/" << *it << " is not element of the \"propertyNames\"\n";
        return false;
    }
    return true;
}

bool ObjectValidation::test_dependencies(const ValueRef & json) const {
    for (const auto & member : m_json_names) {
        const auto it = m_dependencies.find(member);
        if (it != m_dependencies.end()) {
            for (const auto & dep : it->second) {
                if (json(dep).isNull()) {
                    m_report(json) << "/" << member << " failed on dependencie: " << array_as_string(it->second) << "\n";
                    return false;
                }
            }
        }
    }
    return true;
}

bool ObjectValidation::test_required_names(const ValueRef & json) const {
    if (m_required_names.size() > m_size) {
        m_report(json) << "not enough element, expected:" << array_as_string(m_required_names);
        return false;
    } else {
        const auto & names = m_json_names;
        const auto it = find_if(m_required_names.begin(), m_required_names.end(), [&names](const string & name) {
            return find(names.begin(), names.end(), name) == names.end();
        });
        if (it != m_required_names.end()) {
            m_report(json) << "some requied element(s) is/are missing, expected:" << array_as_string(m_required_names);
            return false;
        }
    }
    return true;
}

bool ObjectValidation::test_additional_items(const ValueRef & json) const {
    const auto result = any_of(m_json_names.begin(), m_json_names.end(), [this](const string & name) {
        return false
            || find(m_schema_names.begin(), m_schema_names.end(), name) != m_schema_names.end()
            || find(m_required_names.begin(), m_required_names.end(), name) != m_required_names.end()
            || find(m_additional_names.begin(), m_additional_names.end(), name) != m_additional_names.end()
            // ... || find_if(patten_names.begin(), patten_names.end(), [](){}) != patten_names.end()
            ;
    });
    if (!result) {
        m_report(json, "fail on additionalItems");
    }
    return result;
}

bool ObjectValidation::test_properties(const ValueRef & json) const {
    for (const auto & name : m_json_names) {
        if (m_properties.find(name) != m_properties.end()) {
            const auto * schema = m_properties.at(name);
            if (!schema->do_test(json[name])) {
                m_report_stream << schema->get_report();
                return false;
            }
        }
    }
    return true;
}

bool ObjectValidation::test_patten_properties(const ValueRef &) const {
    // ...
    return true;
}

} // namespace Json