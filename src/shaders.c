
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <alloca.h>
#include <errno.h>
#include <stdio.h>

#include <GLES3/gl31.h>

char*
read_file(const char* path)
{
    printf("open '%s'\r\n", path);
    int fd = open(path, O_RDONLY);
    assert(fd != -1);

    struct stat stats;
    long rv = fstat(fd, &stats);
    assert(rv != -1);

    size_t size = stats.st_size;
    char* text = malloc(size);

    printf("file size: %ld\r\n", size);

    int offs = 0;
    while (offs < size) {
        rv = read(fd, text + offs, size - offs);
        if (rv == -1) {
            perror("huh");
            assert(errno == EINTR);
            rv = 0;
        }
        offs += rv;
    }

    close(fd);

    return text;
}

int
compile_shader(int type, const char* path)
{
    unsigned int shader = glCreateShader(type);
    char* code = read_file(path);
    const char** srcs = (const char**) &code;
    glShaderSource(shader, 1, srcs, 0);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        int size;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &size);
        char* text = alloca(size);

        glGetShaderInfoLog(shader, size, 0, text);
        printf("Shader compile errors for [%s]\r\n%s\r\n", path, text);
        assert(0);
    }

    free(code);
    return shader;
}

int
compile_shader_program(const char* vsrc, const char* fsrc)
{
    int vs = compile_shader(GL_VERTEX_SHADER, vsrc);
    int fs = compile_shader(GL_FRAGMENT_SHADER, fsrc);

    int sp = glCreateProgram();
    glAttachShader(sp, vs);
    glAttachShader(sp, fs);

    glLinkProgram(sp);

    int success;
    glGetProgramiv(sp, GL_LINK_STATUS, &success);

    if (!success) {
        int size;
        glGetProgramiv(sp, GL_INFO_LOG_LENGTH, &size);
        char* text = alloca(size);

        glGetProgramInfoLog(sp, size, 0, text);
        printf("Shader program link errors:\r\n%s\r\n", text);
        assert(0);
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return sp;
}

