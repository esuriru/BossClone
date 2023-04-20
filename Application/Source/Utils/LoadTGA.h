#ifndef LOAD_TGA_H
#define LOAD_TGA_H

#include <glad/glad.h>
#include <vector>
#include <string>

GLuint LoadTGA(const char *file_path);
GLuint LoadTGACubemap(const std::vector<std::string>& file_paths);

#endif