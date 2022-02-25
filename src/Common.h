#ifndef MARBARS_COMMON_H
#define MARBARS_COMMON_H

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <folly/FBVector.h>
#include <folly/FBString.h>

#ifdef WIN32
#include <GL/GL.h>
#else
#include <GL/gl.h>
#endif

namespace Marbas {

template<typename T>
using Vector = folly::fbvector<T>;

using String = folly::fbstring;

}

#include <glog/logging.h>

#endif
