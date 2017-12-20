#include <algorithm>

#include "validation_helper.hh"
#include "coroutine.hpp"
#include "array_validation.hh"
#include "validator.hh"

#include "of_validation.hpp"
#include "size_validation.hpp"

using std::move;
using std::string;
using std::vector;
using std::find;
using std::find_if;
using std::adjacent_find;
using std::sort;

namespace Json {

bool is_unique(const Value & json);

ArrayValidation::ArrayValidation(const ValueRef & schema, bool allow_multi) :
    OfValidationCoroutine(schema, allow_multi),
    SizeValidationCoroutine(schema, "Item") {
    push_back_function(*this, &ArrayValidation::test_is_array);
    push_back_all_size_tests(schema);
    if (get::is_as_bool(schema("uniqueItems"))) {
        push_back_function(*this, &ArrayValidation::test_unique_items);
    }
    if (!schema("contains").isNull()) {
        push_back_function(*this, &ArrayValidation::test_contains);
    }
    if (schema("items").isObject()) {
        m_items.push_back(new Schema(schema["items"], true));
        push_back_function(*this, &ArrayValidation::test_items_object);
    } else if (schema("items").isArray()) {
        for (ArrayIndex i = 0; i < schema("items").size(); i++) {
            m_items.push_back(new Schema(schema["items"][i], true));
        }
        push_back_function(*this, &ArrayValidation::test_items_array);
    }
    m_allow_additional_items = get::is_as_bool(schema("additionalItems"));
    if (allow_multi) {
        add_all_of_tests();
    }
}

ArrayValidation::~ArrayValidation() {
    for (auto * ptr : m_items) {
        delete ptr;
    }
}

bool ArrayValidation::test_is_array(const ValueRef & json) const {
    if (!json->isArray()) {
        m_report(json, "is not an array!");
        return false;
    }
    return true;
}

bool ArrayValidation::test_unique_items(const ValueRef & json) const {
    if (!is_unique(json())) {
        m_report(json) << "has not unique items!";
    }
    return true;
}

bool ArrayValidation::test_contains(const ValueRef &) const {
    // ... not implemented
    return true;
}

bool ArrayValidation::test_items_object(const ValueRef & json) const {
    for (ArrayIndex i = 0; i < json->size(); i++) {
        const auto * schema = m_items[0];
        if (!schema->do_test(json[i])) {
            m_report_stream << schema->get_report();
            return false;
        }
    }
    return true;
}

bool ArrayValidation::test_items_array(const ValueRef & json) const {
    ArrayIndex i;
    for (i = 0; i < json->size() && i < m_items.size(); i++) {
        const auto * schema = m_items[i];
        if (!schema->do_test(json[i])) {
            m_report_stream << schema->get_report();
            return false;
        }
    }
    if (i > m_items.size()) {
        return m_allow_additional_items;
    }
    return true;
}

bool is_unique(const Value & json) {
    vector<const Value *> vec;
    vec.reserve(json.size());
    for (const auto & item : json) {
        vec.push_back(&item);
    }
    sort(vec.begin(), vec.end(), [](const Value * l, const Value * r) { return *l < *r; });
    return adjacent_find(vec.begin(), vec.end(), [](const Value * l, const Value * r) { return *l == *r; }) == vec.end();
}

} // namespace Json