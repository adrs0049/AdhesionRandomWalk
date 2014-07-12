#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG_BUILD
#define DEBUG(x) do { std::cerr << "File:"  <<  __FILE__ << ":" << __LINE__ << ": " << x; } while (0)
#define DEBUG2(x) do { std::cerr << "File:" << __FILE__ << ":" << __LINE__ << ": " << #x << "=" << x << std::endl; } while (0)

#define log_err(M, ...) fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define DEBUG3(x, ...) fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__ )

#include <cassert>
#define ASSERT(x) assert(x);

#define PRINT_FARGS std::printf("File:%s:%d: %s\n",__FILE__,__LINE__, __PRETTY_FUNCTION__);

// Note from c-template-name-pretty-prin
// stackoverflow
#include <typeinfo>
#include <string>
#include <iostream>
#include <cxxabi.h>
#define TYPENAME(TYPE) typeid_name(typeid(TYPE).name())
std::string indent(std::string str, const std::string &indent = "  ") {
    std::string indent_ = std::string("\n");
    size_t token = 0;

    bool one_line = false;
    while ((token = str.find_first_of("<>,", token)) != std::string::npos) {
        size_t size = str.size();
        size_t close, open, comma;

        switch(str[token]) {
        case '<':
            close = str.find(">", token+1);
            open = str.find("<", token+1);
            comma = str.find(",", token+1);
            one_line = !(close > open) && !(comma < close);

            if (one_line) break;
            indent_.append(indent);

        case ',':
            str.insert(token + 1, indent_);
            break;

        case '>':
            if (!one_line) {
                indent_.erase(indent_.size() - indent.size());
                str.insert(token, indent_);
            }
            one_line = false;
        }

        token += 1 + str.size() - size;

        const size_t nw = str.find_first_not_of(" ", token);
        if(nw != std::string::npos) {
            str.erase(token, nw-token);
        }
    }

    return str;
}
std::string typeid_name(const char* name) {
// #ifdef HAVE_CXA_DEMANGLE
    size_t size;
    int status;
    char *buf = abi::__cxa_demangle(name, NULL, &size, &status);
    if (status  != 0) throw status;
    std::string string(buf);
    free(buf);
    return indent(string);
// #else
//     return name;
// #endif
}

#else
#define NDEBUG
#define DEBUG(x)
#define DEBUG2(x)
#define ASSERT(x)
#define TYPENAME(TYPE)
#define PRINT_FARGS
#endif

#endif 