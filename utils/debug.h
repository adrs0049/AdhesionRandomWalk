#ifndef DEBUG_H
#define DEBUG_H

// always include logging
// #include "log.h"

#ifdef DEBUG_BUILD

#include <iostream>
#include "dynamic_assert.h"

#define DEBUG(x) do { std::cerr << "File:"  <<  __FILE__ << ":" << __LINE__ << ": " << x; } while (0)
#define DEBUG2(x) do { std::cerr << "File:" << __FILE__ << ":" << __LINE__ << ": " << #x << "=" << x << std::endl; } while (0)

#define log_err(M, ...) fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define DEBUG3(x, ...) fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__ )

#define DBG_ONLY(x) x

//#include <cassert>

#define ASSERT_1(condition) \
	do { \
		Assert::dynamic((condition)); \
	} while (false)

#define ASSERT(condition, message) \
    do { \
        std::ostringstream msg; \
        msg << message; \
        Assert::dynamic((condition), Assert::compose(#condition, __FILE__, \
                __LINE__, msg.str())); \
    } while (false)

#define WARNING(condition, message) do {} while(false)

/*
#define ASSERT(condition, message) \
    do { \
        if ( ! (condition) ) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message << std::endl; \
			std::abort(); \
		} \
    } while (false)
*/
#define PRINT_FARGS std::printf("File:%s:%d: %s\n",__FILE__,__LINE__, __PRETTY_FUNCTION__);

#else
#define NDEBUG
#define DEBUG(x)
#define DEBUG2(x)
#define ASSERT(condition, message) \
	do { \
		Assert::dynamic((condition)); \
	} while(false);

#define TYPENAME(TYPE)
#define DBG_ONLY(x)
#define PRINT_FARGS
#endif

#endif
