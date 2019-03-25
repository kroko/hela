#ifndef MotorStrap_hpp
#define MotorStrap_hpp

// Motor strap should only include OpenGL(ES)
// ! NOT APIs/interfaces - EGL / GLFW as we do not want user code to see EGL/GLFW stuff
// which pollutes autocompletion stuff in editors (serious first world problems!)

// ============================================================================
#if defined(HELA_LINUX_ARM_FBDEV_MALI_ALLWINNER)

// ----------------------------------------------------------------------------
// Headers

// gl* functions are global and accessible to user code
#define GL_GLEXT_PROTOTYPES
// #include <GLES/gl.h>
// #include <GLES/glext.h>
// Exclusively GLES2 headers
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>


// ! DON'T INCLUDE HERE
// Place in runner, which will not propagate and pollute user code with egl* functions
// Nothing to do abut EGL defines though, meh
// #define EGL_EGLEXT_PROTOTYPES
// #include <EGL/egl.h>
// #include <EGL/eglext.h>

// ----------------------------------------------------------------------------
// GL_OES_vertex_array_object

#if defined(GL_OES_vertex_array_object) // && (GL_OES_vertex_array_object == 1)
  // pass while building "HELA_USE_GL_OES_vertex_array_object=0"
  // to disable usage of VAO, even if it is available
  #if !defined(HELA_USE_GL_OES_vertex_array_object)
    #define HELA_USE_GL_OES_vertex_array_object 1
  #endif
  // pass while building "HELA_USE_GL_OES_vertex_array_object_POLYFILL=0"
  // to disable usage of OES VAO related function polyfill
  #if !defined(HELA_USE_GL_OES_vertex_array_object_POLYFILL)
    #define HELA_USE_GL_OES_vertex_array_object_POLYFILL 1
  #endif
#else
  // if no extension, then force undef appropriate HELA_USE_* because these can be forced through build flags
  #if defined(HELA_USE_GL_OES_vertex_array_object)
    #undef HELA_USE_GL_OES_vertex_array_object
  #endif
  #if defined(HELA_USE_GL_OES_vertex_array_object_POLYFILL)
    #undef HELA_USE_GL_OES_vertex_array_object_POLYFILL
  #endif
#endif

// ----------------------------------------------------------------------------
// GL_OES_vertex_array_object polyfill

#if (HELA_USE_GL_OES_vertex_array_object == 1) && defined(HELA_USE_GL_OES_vertex_array_object_POLYFILL) && (HELA_USE_GL_OES_vertex_array_object_POLYFILL == 1)
// #pragma message("YOLO")
// don't bind, simply declare, define and redirect to OES
extern void glGenVertexArrays(GLsizei n, GLuint *arrays);
extern void glBindVertexArray(GLuint array);
extern void glDeleteVertexArrays(GLsizei n, const GLuint *arrays);
extern GLboolean glIsVertexArray(GLuint array);
#endif

// ----------------------------------------------------------------------------
// one channel textures

// this is wrong, but at least we build
#if !defined(GL_RED)
  #if defined(GL_RED_EXT)
    #define GL_RED GL_RED_EXT
  #else
    #pragma message("GL_RED GL_RED_EXT")
  #endif
#endif

// ----------------------------------------------------------------------------
// sRGB internals polyfill
// Redirect GLES to use known internal non-sRGB format

#if !defined(GL_SRGB)
  #define GL_SRGB GL_RGB
#endif

#if !defined(GL_SRGB_ALPHA)
  #define GL_SRGB_ALPHA GL_RGBA
#endif

// ----------------------------------------------------------------------------
// half float datatype polyfill
// Allwinner has GL_OES_vertex_half_float

#if !defined(GL_HALF_FLOAT)
  #if defined(GL_HALF_FLOAT_OES)
    #define GL_HALF_FLOAT GL_HALF_FLOAT_OES
  #else
    #pragma message("GL_HALF_FLOAT GL_HALF_FLOAT_OES")
  #endif
#endif

// ----------------------------------------------------------------------------
// half float textures
// Allwinner does not have GL_OES_texture_half_float or OES_texture_half_float, but has these datatypes specified

#if !defined(GL_R16F)
  #if defined(GL_R16F_EXT)
    #define GL_R16F GL_R16F_EXT
  #else
    #pragma message("GL_R16F GL_R16F_EXT")
  #endif
#endif

#if !defined(GL_RGB16F)
  #if defined(GL_RGB16F_EXT)
    #define GL_RGB16F GL_RGB16F_EXT
  #else
    #pragma message("GL_RGB16F GL_RGB16F_EXT")
  #endif
#endif

#if !defined(GL_RGBA16F)
  #if defined(GL_RGBA16F_EXT)
    #define GL_RGBA16F GL_RGBA16F_EXT
  #else
    #pragma message("GL_RGBA16F GL_RGBA16F_EXT")
  #endif
#endif

// ----------------------------------------------------------------------------
// float textures
// Allwinner does not have GL_OES_texture_float or OES_texture_float, but has these datatypes specified

#if !defined(GL_R32F)
  #if defined(GL_R32F_EXT)
    #define GL_R32F GL_R32F_EXT
  #else
    #pragma message("GL_R32F GL_R32F_EXT")
  #endif
#endif

#if !defined(GL_RGB32F)
  #if defined(GL_RGB32F_EXT)
    #define GL_RGB32F GL_RGB32F_EXT
  #else
    #pragma message("GL_RGB32F GL_RGB32F_EXT")
  #endif
#endif

#if !defined(GL_RGBA32F)
  #if defined(GL_RGBA32F_EXT)
    #define GL_RGBA32F GL_RGBA32F_EXT
  #else
    #pragma message("GL_RGBA32F GL_RGBA32F_EXT")
  #endif
#endif

// ----------------------------------------------------------------------------
// OES_vertex_half_float

#if !defined(GL_HALF_FLOAT)
  #if defined(HALF_FLOAT_OES)
    #define GL_HALF_FLOAT HALF_FLOAT_OES
  #endif
#endif

// ----------------------------------------------------------------------------
#if !defined(GL_DEPTH24_STENCIL8)
  #if defined(GL_DEPTH24_STENCIL8_OES)
    #define GL_DEPTH24_STENCIL8 GL_DEPTH24_STENCIL8_OES
  #else
    #pragma message("No GL_DEPTH24_STENCIL8 and GL_DEPTH24_STENCIL8_OES! Beware!")
  #endif
#endif

// ============================================================================
#else

// ----------------------------------------------------------------------------
// Headers

// gl* functions are global and accessible to user code
#include "glad/glad.h"

// ! DON'T INCLUDE HERE
// Place in runner, which will not propagate and pollute user code with glfw* functions
// Nothing to do abut GLFW defines though, meh
// #include "GLFW/glfw3.h"

#endif

#endif // MotorStrap_hpp
