#ifndef STRUCTS_HPP
#define STRUCTS_HPP

#include <map>
#include <glbinding/gl/gl.h>
// use gl definitions from glbinding 
using namespace gl;

// gpu representation of model
struct model_object {
  // vertex array object
  GLuint vertex_AO = 0;
  // vertex buffer object
  GLuint vertex_BO = 0;
  // index buffer object
  GLuint element_BO = 0;
  // primitive type to draw
  GLenum draw_mode = GL_NONE;
  // indices number, if EBO exists
  GLsizei num_elements = 0;
};

// gpu representation of texture
struct texture_object {
  // handle of texture object
  GLuint handle = 0;
  // binding point
  GLenum target = GL_NONE;
};

// shader handle and uniform storage
struct shader_program {
  shader_program(std::map<GLenum, std::string> paths)
   :shader_paths{paths}
   ,handle{0}
   {}

  // paths to shader sources
  std::map<GLenum, std::string> shader_paths;
  // object handle
  GLuint handle;
  // uniform locations mapped to name
  std::map<std::string, GLint> u_locs{};
};

// =====================================================
// Assignemnt 5 Framebuffer
struct framebuffer_object {
	// handle of framebuffer object
	GLuint handle = 0;

	// color attachment
	texture_object color_buffer;
	GLuint color_handle = 0;

	// depth attachment
	GLuint depth_handle;
};
// =====================================================

#endif