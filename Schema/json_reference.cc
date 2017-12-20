#include "json_reference.hh"

using std::string;
using std::to_string;
using std::invalid_argument;

namespace Json {

ValueRef::ValueRef()
    : m_proot(nullptr), m_pjson(nullptr), m_position("#") {}

ValueRef::ValueRef(const ValueRef & other)
    : m_proot(other.m_proot), m_pjson(other.m_pjson), m_position(other.m_position) {}

ValueRef::ValueRef(const Value & root)
    : m_proot(&root), m_pjson(&root), m_position("#") {}

ValueRef::ValueRef(const Value * root, const Value & json, const std::string & position)
    : m_proot(root), m_pjson(&json), m_position(position) {}

void ValueRef::step(const string & index) {
    const auto & ValueRef = *m_pjson;
    if (ValueRef.isArray()) {
        m_pjson = &(ValueRef[stoi(index)]);
    } else {
        m_pjson = &(ValueRef[index]);
    }
    m_position += "/" + index;
}

void ValueRef::step(int index) {
    m_pjson = &(get_json()[index]);
    m_position += "/" + to_string(index);
}

void ValueRef::set_root(const Value & json) {
    m_proot = &json;
    m_pjson = &json;
    m_position = "#";
}

void ValueRef::re_root() {
    m_pjson = m_proot;
    m_position = "#";
}

void ValueRef::forward(const string & index) {
    if (index != "") {
        string element = "";
        for (unsigned i = 0; i < index.length(); i++) {
            if (index[i] == '/') {
                if (element.length() > 0) {
                    step(element);
                }
                element = "";
            } else {
                element += index[i];
            }
        }
        if (element.length() > 0) {
            step(element);
        }
    }
}

void ValueRef::goto_ref(const std::string & index) {
    size_t i = 0;
    if (index[0] == '#') {
        m_pjson = m_proot;
        m_position = "#";
    } else {
        i++;
    }
    forward(index.substr(i));
}

ValueRef ValueRef::operator[](const std::string & index) const {
    return ValueRef(m_proot, get_json()[index], m_position + "/" + index);
}

ValueRef ValueRef::operator[](int index) const {
    return ValueRef(m_proot, get_json()[index], m_position + "/" + to_string(index));
}

const Value & ValueRef::operator()(const std::string & index) const {
    return get_json()[index];
}

const Value & ValueRef::operator()(int index) const {
    return get_json()[index];
}

const Value & ValueRef::operator()() const {
    return get_json();
}

const Value * const ValueRef::operator->() const {
    return m_pjson;
}

const string & ValueRef::get_position() const {
    return m_position;
}

const Value & ValueRef::get_json() const {
    if (m_pjson) { return *m_pjson; }
    throw invalid_argument("ValueRef contain nullptr while \"get_json() const\" was called!");
}

} // namespace Json