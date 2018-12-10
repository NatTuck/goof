#ifndef GLTEST_SHADERS
#define GLTEST_SHADERS

int compile_shader(int type, const char* path);
int compile_shader_program(const char* vsrc, const char* fsrc);

#endif
