#pragma once

#include <jsoncpp/json/value.h>

namespace Json {

//namespace std = ::std;

class ValueRef {
public:
    ValueRef();
    ValueRef(ValueRef &&) = default;
    explicit ValueRef(const ValueRef &);
    explicit ValueRef(const Value & root);


    //Ref & operator=(const Ref &);
    void step(const std::string &);
    void step(int);
    void set_root(const Value &);
    void re_root();
    void forward(const std::string &);
    void goto_ref(const std::string &);

    auto operator[](const std::string &) const->ValueRef;
    auto operator[](int) const->ValueRef;

    auto operator->() const -> const Value * const;

    auto operator()(const std::string &) const -> const Value &;
    auto operator()(int) const -> const Value &;
    auto operator()() const -> const Value &;

    const std::string & get_position() const;
private:
    explicit ValueRef(const Value * root, const Value &, const std::string &);

    const Value & get_json() const;

    const Value * m_proot;
    const Value * m_pjson;
    std::string m_position;
};

} // namespace Json
