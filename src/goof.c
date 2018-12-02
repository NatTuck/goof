
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <alloca.h>
#include <assert.h>

#include <GLFW/glfw3.h>
#include <erl_nif.h>

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

void on_glfw_error(int error, const char* description)
{
    puts("glfw error\r\n");
    puts(description);
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

static ErlNifFunc funcs[] =
{
    {"init", 0, goof_init, 0},
    {"cleanup", 0, goof_cleanup, 0},
    {"glfw_version", 0, goof_glfw_version, 0},
};

ERL_NIF_INIT(Elixir.Goof, funcs, NULL, NULL, NULL, NULL)

