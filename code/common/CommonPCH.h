#pragma once

#include <iostream>
#include <filesystem>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <glm/glm.hpp>

#ifdef WIN32
#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)
#else
#define EXTERN_DLL_EXPORT extern "C"
#endif