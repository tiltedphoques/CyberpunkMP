#pragma once

#include <iostream>
#include <filesystem>
#include <sstream>

#include <Core/Platform.h>
#include <Core/Stl.h>
#include <Core/Initializer.h>
#include <Core/Buffer.h>
#include <Core/ViewBuffer.h>
#include <Core/Serialization.h>
#include <Core/ScratchAllocator.h>
#include <Math/Math.h>
#include <Core/TaskQueue.h>
#include <Core/Filesystem.h>
#include <Core/Hash.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <cxxopts.hpp>

#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/compiler/importer.h>