#include "validation_helper.hh"

namespace Json {

namespace get {

template<>
bool is<int32_t>(const Value & json) {
    return json.isInt64();
}

template<>
bool is<int64_t>(const Value & json) {
    return json.isInt();
}

template<>
bool is<uint32_t>(const Value & json) {
    return json.isUInt64();
}

template<>
bool is<uint64_t>(const Value & json) {
    return json.isUInt();
}

template<>
bool is<double>(const Value & json) {
    return json.isDouble();
}

template<>
int32_t as<int32_t>(const Value & json) {
    return json.asInt();
}

template<>
int64_t as<int64_t>(const Value & json) {
    return json.asInt64();
}

template<>
uint32_t as<uint32_t>(const Value & json) {
    return json.asUInt();
}

template<>
uint64_t as<uint64_t>(const Value & json) {
    return json.asUInt64();
}

template<>
double as<double>(const Value & json) {
    return json.asDouble();
}

bool is_as_bool(const Value & json) {
    return json.isBool() && json.asBool();
}

} // namespace get

std::string array_as_string(const std::vector<std::string> & array) {
    std::string rtn = "[ ";
    for (const auto & member : array) {
        rtn += member + ", ";
    }
    rtn = rtn.substr(0, rtn.length() - 2);
    rtn += " ]";
    return rtn;
}

std::string json_array_as_string(const Value & json_array) {
    if (!json_array.isArray()) { return ""; }
    std::string rtn = "[ ";
    for (const auto & json : json_array) {
        if (!json.isString()) { continue; }
        rtn += json.asString() + ", ";
    }
    rtn = rtn.substr(0, rtn.length() - 2);
    rtn += " ]";
    return rtn;
}

} // namespace Json