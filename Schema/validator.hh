#pragma once

#include <memory>

#include "of_validation.hh"

namespace Json {

class Schema : public OfValidationCoroutine<Schema> {
public:
    Schema();
    Schema(const Value & schema);
    Schema(const ValueRef & schema, bool allow_multi);
    void set_schema(const Value & schema);
    bool validate(const Value & json);
    bool validate(const ValueRef & json);
    virtual ~Schema() {}
private:
    void set_schema(const ValueRef & schema);
    bool test(const ValueRef & json) const;
    std::unique_ptr<ValidationCoroutine> m_valco;
};

} // namespace Json