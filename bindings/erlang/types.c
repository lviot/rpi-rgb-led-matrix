//
// Created by Louis Viot on 22/11/2025.
//

#include "types.h"

error_e get_charlist(ErlNifEnv *env, ERL_NIF_TERM term, char **string)
{
    unsigned length;

    if (!enif_get_string_length(env, term, &length, ERL_NIF_LATIN1)) {
        return BAD_ARG_EXPECT_CHARLIST;
    }
    *string = malloc(length + 1);
    if (!*string) {
        return MEM_ALLOC;
    }
    if (!enif_get_string(env, term, *string, length + 1, ERL_NIF_LATIN1)) {
        return BAD_ARG_EXPECT_CHARLIST;
    }
    return NONE;
}

error_e get_atom(ErlNifEnv *env, ERL_NIF_TERM term, char **atom)
{
    unsigned length;

    if (!enif_get_atom_length(env, term, &length, ERL_NIF_LATIN1)) {
        return BAD_ARG_EXPECT_ATOM;
    }
    *atom = malloc(length + 1);
    if (!atom) {
        return MEM_ALLOC;
    }
    if (!enif_get_atom(env, term, *atom, length + 1, ERL_NIF_LATIN1)) {
        return BAD_ARG_EXPECT_ATOM;
    }
    return NONE;
}

error_e get_bool(ErlNifEnv *env, ERL_NIF_TERM term, bool *value)
{
    char *atom;
    error_e err;

    err = get_atom(env, term, &atom);
    if (err) {
        return err;
    }
    *value = strcmp(atom, "true") == 0;
    free(atom);
    return NONE;
}