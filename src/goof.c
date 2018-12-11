
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <alloca.h>
#include <assert.h>

#include <erl_nif.h>
#include <GLFW/glfw3.h>

#include "view.h"

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

static
ERL_NIF_TERM
goof_init(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    assert(argc == 0);

    static int done = 0;
    int rv = 1;
    if (!done) {
        rv = view_init();
    }
    if (rv) {
        return enif_make_atom(env, "ok");
    }
    else {
        return enif_make_atom(env, "error");
    }
}

static
int
unpack_point(ErlNifEnv* env, ERL_NIF_TERM pt, float* pt_data)
{
    int rv;
    int nn;
    double tmp;

    const ERL_NIF_TERM* pt_tup;
    const ERL_NIF_TERM* pt_posn;
    const ERL_NIF_TERM* pt_color;

    rv = enif_get_tuple(env, pt, &nn, &pt_tup);
    if (!rv || nn != 2) {
        return 0;
    }

    rv = enif_get_tuple(env, pt_tup[0], &nn, &pt_posn);
    if (!rv || nn != 2) {
        return 0;
    }

    for (int ii = 0; ii < 2; ++ii) {
        rv = enif_get_double(env, pt_posn[ii], &tmp);
        if (!rv) {
            return 0;
        }
        printf("pos[%d] = %.02f\r\n", ii, tmp);
        pt_data[ii] = (float)tmp;
    }

    rv = enif_get_tuple(env, pt_tup[1], &nn, &pt_color);
    if (!rv || nn != 4) {
        return 0;
    }

    for (int ii = 0; ii < 4; ++ii) {
        rv = enif_get_double(env, pt_color[ii], &tmp);
        if (!rv) {
            return 0;
        }
        printf("color[%d] = %.02f\r\n", ii, tmp);
        pt_data[2 + ii] = (float)tmp;
    }

    return 1;
}

static
ERL_NIF_TERM
goof_set_points(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    assert(argc == 1);
    ERL_NIF_TERM xs = argv[0];

    int len;
    int rv = enif_get_list_length(env, xs, &len);
    if (!rv) {
        return enif_make_atom(env, "error");
    }

    printf("got list of %d points.\r\n", len);

    float* data = malloc(len * 6 * sizeof(float));

    ERL_NIF_TERM head;
    for (int ii = 0; ii < len; ++ii) {
        rv = enif_get_list_cell(env, xs, &head, &xs);
        if (!rv) {
            free(data);
            return enif_make_atom(env, "error");
        }

        rv = unpack_point(env, head, data + (6 * ii));
        if (!rv) {
            free(data);
            return enif_make_atom(env, "error");
        }
    }

    printf("data:\r\n");
    for (int ii = 0; ii < (len * 6); ++ii) {
        printf("%.02f\r\n", data[ii]);
    }

    view_set_points(data, len);

    return enif_make_atom(env, "ok");
}

static
ERL_NIF_TERM
goof_cleanup(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    assert(argc == 0);
    if (view_stop()) {
        return enif_make_atom(env, "ok");
    }
    else {
        return enif_make_atom(env, "error");
    }
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
    {"set_points", 1, goof_set_points, 0},
    {"glfw_version", 0, goof_glfw_version, 0},
};

ERL_NIF_INIT(Elixir.Goof, funcs, goof_load, NULL, NULL, NULL)

