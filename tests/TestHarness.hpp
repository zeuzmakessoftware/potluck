#pragma once

#include <sstream>
#include <stdexcept>
#include <string>

inline void Expect(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

template <typename Actual, typename Expected>
void ExpectEqual(const Actual& actual, const Expected& expected, const std::string& message) {
    if (!(actual == expected)) {
        std::ostringstream out;
        out << message << " (actual=" << actual << ", expected=" << expected << ')';
        throw std::runtime_error(out.str());
    }
}
