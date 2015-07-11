
#include "dynamic_assert.h"

namespace Assert {

    std::string compose(const std::string& condition, const char* file,
                        int line, const std::string& message)
    {
        std::ostringstream os("");
        os << "Assertion " << condition << " failed in (" << file << "," << line << "):" << message;
        return os.str();
    }

    std::string compose(const char* file, int line, const std::string& message)
    {
        std::ostringstream os ("(");
        os << file << "," << line << "):" << message;
        return os.str();
    }

    template<bool condition, class Except>
    void dynamic(bool assertion, const std::string& message)
    {
        if (assertion || current_mode == Assert::Mode::ignore_)
            return;
        if (current_mode == Assert::Mode::throw_)
            throw Except{message};
        if (current_mode == Assert::Mode::terminate_)
			std::terminate();
    }

    template<>
    void dynamic<false, Error>(bool, const std::string&)
    {}

    void dynamic(bool b, const std::string& s)
    {
        dynamic<true, Error>(b, s);
    }

    void dynamic(bool b)
    {
        dynamic<true, Error>(b);
    }



} // namespace end

