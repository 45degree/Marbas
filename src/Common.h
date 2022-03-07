#ifndef MARBARS_COMMON_H
#define MARBARS_COMMON_H

#include <GLFW/glfw3.h>

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

#include <GL/glew.h>
#include <folly/FBVector.h>
#include <folly/FBString.h>
#include <folly/Format.h>
#include <filesystem>

#ifdef WIN32
#include <GL/GL.h>
#else
#include <GL/gl.h>
#endif

namespace Marbas {

template<typename T>
using Vector = folly::fbvector<T>;

using String = folly::fbstring;

namespace FileSystem = std::filesystem;

#define FORMAT(Str, ...) folly::sformat(Str, __VA_ARGS__);

}  // namespace Marbas

#include <glog/logging.h>

#endif
