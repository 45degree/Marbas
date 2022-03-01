#ifndef MARBARS_COMMON_H
#define MARBARS_COMMON_H

#include <GLFW/glfw3.h>
// #include <glad/glad.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <folly/FBVector.h>
#include <folly/FBString.h>
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

}

#include <glog/logging.h>

#endif
