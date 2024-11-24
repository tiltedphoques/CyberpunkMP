#include <cstdint>
#include <memory>
#include <string>

#ifdef CPPSHARP_GENERATOR
#define SCRIPTING_SKIP(x)
#define TP_EXPORT 
#else
#define SCRIPTING_SKIP(x) x
#ifdef WIN32
#define TP_EXPORT __declspec(dllexport)
#else
#define TP_EXPORT __attribute__((visibility("default")))
#endif
#endif

#define CS_INTERNAL
#define CS_READONLY