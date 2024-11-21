#include <cstdint>
#include <memory>
#include <string>

#ifdef WIN32
#define TP_EXPORT __declspec(dllexport)
#else
#define TP_EXPORT
#endif

#ifdef CPPSHARP_GENERATOR
#define SCRIPTING_SKIP(x)
#else
#define SCRIPTING_SKIP(x) x
#endif

#define CS_INTERNAL
#define CS_READONLY