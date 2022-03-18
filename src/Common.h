#ifndef MARBARS_COMMON_H
#define MARBARS_COMMON_H

/**
 * glfw and opengl
 */

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


/**
 * glm
 */

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

/**
 * tools
 */

#define FORMAT(Str, ...) folly::sformat(Str, __VA_ARGS__);

namespace Marbas {

template<typename T>
#ifdef DEBUG
using Vector = std::vector<T>;
#else
using Vector = folly::fbvector<T>;
#endif

#ifdef DEBUG
using String = std::string;
#else
using String = folly::fbstring;
#endif

namespace FileSystem = std::filesystem;
using Path = std::filesystem::path;


}  // namespace Marbas

#include <glog/logging.h>

#endif
