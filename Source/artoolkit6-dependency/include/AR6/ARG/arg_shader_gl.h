/*
 *  arg_shader_gl.h
 *  ARToolKit6
 *
 *  This file is part of ARToolKit.
 *
 *  ARToolKit is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ARToolKit is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with ARToolKit.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  As a special exception, the copyright holders of this library give you
 *  permission to link this library with independent modules to produce an
 *  executable, regardless of the license terms of these independent modules, and to
 *  copy and distribute the resulting executable under terms of your choice,
 *  provided that you also meet, for each linked independent module, the terms and
 *  conditions of the license of that module. An independent module is a module
 *  which is neither derived from nor based on this library. If you modify this
 *  library, you may extend this exception to your version of the library, but you
 *  are not obligated to do so. If you do not wish to do so, delete this exception
 *  statement from your version.
 *
 *  Copyright 2015-2016 Daqri, LLC.
 *  Copyright 2011-2015 ARToolworks, Inc.
 *
 *  Author(s): Philip Lamb
 *
 */

#ifndef __AR6_ARG_arg_shader_gl_h__
#define __AR6_ARG_arg_shader_gl_h__

// ============================================================================
//  Public includes.
// ============================================================================

#include <AR6/AR/ar.h>      // ARUint8, AR_PIXEL_FORMAT, ARParam, arParamObserv2Ideal()

#if (HAVE_GLES2 || HAVE_GL3)

#if HAVE_GLES2
#  include <AR6/ARG/glStateCache2.h>
#elif HAVE_GL3
#  ifdef __APPLE__
#    include <OpenGL/gl3.h>
#  else
#    define GL_GLEXT_PROTOTYPES
#    include <GL/glcorearb.h>
#  endif
#endif

#ifdef __cplusplus
extern "C" {
#endif
    
// ============================================================================
//  Public types and definitions.
// ============================================================================


// ============================================================================
//  Public functions.
// ============================================================================


/* Shader Utilities */
GLint arglGLCompileShaderFromFile(GLuint *shader, GLenum type, const char *pathname);
GLint arglGLCompileShaderFromString(GLuint *shader, GLenum type, const char *s);
GLint arglGLLinkProgram(GLuint prog);
GLint arglGLValidateProgram(GLuint prog);
void arglGLDestroyShaders(GLuint vertShader, GLuint fragShader, GLuint prog);

#ifdef __cplusplus
}
#endif

#endif // HAVE_GLES2 || HAVE_GL3

#endif // !__AR6_ARG_arg_shader_gl_h__
