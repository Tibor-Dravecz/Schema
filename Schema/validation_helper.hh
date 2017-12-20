#pragma once

#include <jsoncpp/json/value.h>

namespace Json {

namespace get {

template<typename Tp_>
bool is(const Value &);

template<typename Tp_>
Tp_ as(const Value &);

bool is_as_bool(const Value &);

} // namespace get

std::string array_as_string(const std::vector<std::string> & array);
std::string json_array_as_string(const Value & json_array);

} // namespace Json