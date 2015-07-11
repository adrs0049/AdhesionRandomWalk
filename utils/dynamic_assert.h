
#pragma once

#include <string>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <sstream>

// fix this somehow, can cmake set this value?
#define CURRENT_LEVEL 0

namespace Assert {

    enum class Mode { throw_, terminate_, ignore_ };
    constexpr Mode current_mode = Mode::throw_;
    const int current_level = CURRENT_LEVEL;
    constexpr int default_level = 1;

    constexpr bool level(int n)
        { return n<=current_level; }

    struct Error : std::runtime_error {
        Error(const std::string& p) : std::runtime_error(p) {}
    };

    std::string compose(const std::string& condition, const char* file,
                        int line, const std::string& message);
	std::string compose(const char* file, int line, const std::string& message);

    template<bool condition = level(default_level), class Except = Error>
    void dynamic(bool assertion, const std::string& message = "Assert::dynamic failed");

	template<>
    void dynamic<false, Error>(bool, const std::string&);
    void dynamic(bool b, const std::string& s);
    void dynamic(bool b);

} // end namespace

