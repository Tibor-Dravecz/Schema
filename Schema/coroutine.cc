#include <regex>
#include <memory>
#include <algorithm>

#include "coroutine.hh"
#include "validation_helper.hh"

using std::string;
using std::stringstream;

namespace Json {

bool ValidationCoroutine::do_test(const ValueRef & json) const {
    m_report_stream.clear();
    for (const delegate<bool(const ValueRef &)> & fun : m_functs) {
        if (!fun(json)) {
            return false;
        }
    }
    return true;
}

string ValidationCoroutine::get_report() const {
    return m_report_stream.str();
}

void ValidationCoroutine::m_report(const ValueRef & json, const string & str) const {
    m_report_stream << json.get_position() << " " << str << "\n";
}

stringstream & ValidationCoroutine::m_report(const ValueRef & json) const {
    m_report_stream << json.get_position();
    return m_report_stream;
}

} // namespace Json