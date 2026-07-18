#pragma once

#include <sstream>
#include <stdexcept>
#include <string>

inline void Expect(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

inline std::string ReplaceOnce(std::string text, const std::string& from,
                               const std::string& to) {
    const std::size_t position = text.find(from);
    Expect(position != std::string::npos, "fixture contains replacement text");
    text.replace(position, from.size(), to);
    return text;
}

template <typename Actual, typename Expected>
void ExpectEqual(const Actual& actual, const Expected& expected, const std::string& message) {
    if (!(actual == expected)) {
        std::ostringstream out;
        out << message << " (actual=" << actual << ", expected=" << expected << ')';
        throw std::runtime_error(out.str());
    }
}
