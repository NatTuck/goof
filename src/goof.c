
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <alloca.h>
#include <assert.h>

#include <erl_nif.h>

#include <GLFW/glfw3.h>
#include <GLES3/gl31.h>

#include "shaders.h"

typedef struct goof_state {
    GLFWwindow* window;
} goof_state;

static
ERL_NIF_TERM
make_ok_pair(ErlNifEnv* env, ERL_NIF_TERM val)
{
    ERL_NIF_TERM ok = enif_make_atom(env, "ok");
    return enif_make_tuple(env, 2, ok, val);
}

static
ERL_NIF_TERM
make_error_text(ErlNifEnv* env, const char* emsg)
{
    ERL_NIF_TERM err = enif_make_atom(env, "error");
    ERL_NIF_TERM msg = enif_make_atom(env, emsg);
    return enif_make_tuple(env, 2, err, msg);
}

static GLFWwindow* window;
unsigned int spgs[4];
unsigned int vaos[4];
unsigned int vbos[4];

void
on_glfw_error(int error, const char* description)
{
    puts("glfw error\r\n");
    puts(description);
}

void
on_framebuffer_resize(GLFWwindow* _window, int width, int height)
{
    printf("Resize: %d %d\n", width, height);
    glViewport(0, 0, width, height);
}

static
ERL_NIF_TERM
goof_init(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    assert(argc == 0);

    static int done = 0;
    int rv = 1;
    if (!done) {
        rv = glfwInit();
        glfwSetErrorCallback(on_glfw_error);

        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

        window = glfwCreateWindow(800, 600, "gltest window", NULL, NULL);
        if (!window)
        {
            glfwTerminate();
            return make_error_text(env, "create window failed");
        }

        glfwMakeContextCurrent(window);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glfwSwapInterval(1);
        glfwSetFramebufferSizeCallback(window, on_framebuffer_resize);
        glViewport(0, 0, 800, 600);

        spgs[0] = compile_shader_program(
            "src/glsl/vertex.glsl",
            "src/glsl/fragment.glsl");

        glGenVertexArrays(4, vaos);
        glBindVertexArray(vaos[0]);

        static float points[] = {
            0.3f, 0.3f, 1.0f, 0.5f, 0.5f, 1.0f,
            0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f,
            0.7f, 0.7f, 1.0f, 0.5f, 1.0f, 1.0f,
        };

        glGenBuffers(4, vbos);
        glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
        glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), points, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)8);
        glEnableVertexAttribArray(1);
    }
    if (rv) {
        return enif_make_atom(env, "ok");
    }
    else {
        return enif_make_atom(env, "error");
    }
}

static
ERL_NIF_TERM
goof_tick(ErlNifEnv * env, int argc, const ERL_NIF_TERM argv[])
{
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(spgs[0]);
    glBindVertexArray(vaos[0]);
    glDrawArrays(GL_POINTS, 0, 3);

    glfwSwapBuffers(window);

    glfwPollEvents();

    return enif_make_atom(env, "ok");
}

static
ERL_NIF_TERM
goof_cleanup(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    assert(argc == 0);

    glfwTerminate();
    return enif_make_atom(env, "ok");
}

static
ERL_NIF_TERM
goof_glfw_version(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    assert(argc == 0);
    const char* vers = glfwGetVersionString();
    size_t size = strlen(vers);
    ErlNifBinary text;
    int rv = enif_alloc_binary(size, &text);
    assert(rv);
    memcpy(text.data, vers, text.size);
    return enif_make_binary(env, &text);
}

static
int
goof_load(ErlNifEnv* env, void** priv_data, ERL_NIF_TERM load_info)
{


    return 0;
}

static ErlNifFunc funcs[] =
{
    {"init", 0, goof_init, 0},
    {"cleanup", 0, goof_cleanup, 0},
    {"tick", 0, goof_tick, 0},
    {"glfw_version", 0, goof_glfw_version, 0},
};

ERL_NIF_INIT(Elixir.Goof, funcs, goof_load, NULL, NULL, NULL)

