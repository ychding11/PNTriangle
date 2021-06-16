
#pragma once

#include <GL/glew.h>

GLuint loadStandardShaders(const char *vert_file_path, const char *frag_file_path);

GLuint loadTessShaders(const char *tess_vert_file_path, const char *tess_ctrl_file_path, const char *tess_eval_file_path, const char *tess_frag_file_path);
