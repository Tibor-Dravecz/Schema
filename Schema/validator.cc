#include "validator.hh"

#include "coroutine.hpp"

#include "validation_helper.hh"
#include "object_validation.hh"
#include "array_validation.hh"
#include "string_validation.hh"
#include "integer_validation.hh"
#include "number_validation.hh"
#include "bool_validation.hh"
#include "null_validation.hh"

#include "of_validation.hpp"

using std::string;
using std::unique_ptr;

namespace Json {

Schema::Schema()
    : OfValidationCoroutine(ValueRef(Value()), false) {}

Schema::Schema(const Value & schema)
    : Schema(ValueRef(schema), true) {}

Schema::Schema(const ValueRef & schema, bool allow_multi)
    : OfValidationCoroutine(schema, allow_multi && schema("type").isNull()) {
    if (schema("type").isNull()) {
        if (allow_multi) {
            add_all_of_tests();
        }
    } else if (schema("type").isString()) {
        const auto type = schema("type").asString();
        if (type == "object") {
            m_valco.reset(new ObjectValidation(schema, true));
        } else if (type == "array") {
            m_valco.reset(new ArrayValidation(schema, true));
        } else if (type == "string") {
            m_valco.reset(new StringValidation(schema, true));
        } else if (type == "number") {
            m_valco.reset(new NumberValidation(schema, true));
        } else if (type == "integer") {
            m_valco.reset(new IntegerValidation(schema, true));
        } else if (type == "boolean") {
            m_valco.reset(new BoolValidation(schema));
        } else if (type == "null") {
            m_valco.reset(new NullValidation(schema, true));
        } else if (type == "not_null") {
            m_valco.reset(new NullValidation(schema, false));
        }
        if (m_valco) {
            push_back_function(*this, &Schema::test);
        }
    }
}

void Schema::set_schema(const Value & schema) {
    set_schema(ValueRef(schema));
}

void Schema::set_schema(const ValueRef & schema) {
    m_functs.clear();
    if (schema("type").isNull()) {
        reset(schema, true);
        add_all_of_tests();
    } else {
        reset(schema, false);
        if (schema("type").isString()) {
            const auto type = schema("type").asString();
            if (type == "object") {
                m_valco.reset(new ObjectValidation(schema, true));
            } else if (type == "array") {
                m_valco.reset(new ArrayValidation(schema, true));
            } else if (type == "string") {
                m_valco.reset(new StringValidation(schema, true));
            } else if (type == "number") {
                m_valco.reset(new NumberValidation(schema, true));
            } else if (type == "integer") {
                m_valco.reset(new IntegerValidation(schema, true));
            } else if (type == "boolean") {
                m_valco.reset(new BoolValidation(schema));
            } else if (type == "null") {
                m_valco.reset(new NullValidation(schema, true));
            } else if (type == "not_null") {
                m_valco.reset(new NullValidation(schema, false));
            } else {
                m_valco.reset(nullptr);
            }if (m_valco) {
                push_back_function(*this, &Schema::test);
            }
        }
    }
}

bool Schema::validate(const Value & json) {
    return do_test(ValueRef(json));
}

bool Schema::validate(const ValueRef & json) {
    return do_test(json);
}

bool Schema::test(const ValueRef & json) const {
    if (m_valco) {
        const auto result = m_valco->do_test(json);
        if (!result) {
            m_report_stream << m_valco->get_report();
        }
        return result;
    }
    return true;
}

} // namespace Json