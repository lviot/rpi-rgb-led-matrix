//
// Created by Louis Viot on 16/11/2025.
//

#include "options.h"
#include "types.h"

/*
* (options data type, field name, erlang type, c type)
*/
#define OPTIONS_KEYS                                                                \
        X(DEFAULT, hardware_mapping,         ERL_NIF_TERM_TYPE_BITSTRING, CHAR_PTR) \
        X(DEFAULT, rows,                     ERL_NIF_TERM_TYPE_INTEGER,   INT     ) \
        X(DEFAULT, cols,                     ERL_NIF_TERM_TYPE_INTEGER,   INT     ) \
        X(DEFAULT, chain_length,             ERL_NIF_TERM_TYPE_INTEGER,   INT     ) \
        X(DEFAULT, parallel,                 ERL_NIF_TERM_TYPE_INTEGER,   INT     ) \
        X(DEFAULT, pwm_bits,                 ERL_NIF_TERM_TYPE_INTEGER,   INT     ) \
        X(DEFAULT, pwm_lsb_nanoseconds,      ERL_NIF_TERM_TYPE_INTEGER,   INT     ) \
        X(DEFAULT, pwm_dither_bits,          ERL_NIF_TERM_TYPE_INTEGER,   INT     ) \
        X(DEFAULT, brightness,               ERL_NIF_TERM_TYPE_INTEGER,   INT     ) \
        X(DEFAULT, scan_mode,                ERL_NIF_TERM_TYPE_INTEGER,   INT     ) \
        X(DEFAULT, row_address_type,         ERL_NIF_TERM_TYPE_INTEGER,   INT     ) \
        X(DEFAULT, multiplexing,             ERL_NIF_TERM_TYPE_INTEGER,   INT     ) \
        X(DEFAULT, disable_hardware_pulsing, ERL_NIF_TERM_TYPE_ATOM,      BOOL    ) \
        X(DEFAULT, show_refresh_rate,        ERL_NIF_TERM_TYPE_ATOM,      BOOL    ) \
        X(DEFAULT, inverse_colors,           ERL_NIF_TERM_TYPE_ATOM,      BOOL    ) \
        X(DEFAULT, led_rgb_sequence,         ERL_NIF_TERM_TYPE_BITSTRING, CHAR_PTR) \
        X(DEFAULT, pixel_mapper_config,      ERL_NIF_TERM_TYPE_BITSTRING, CHAR_PTR) \
        X(DEFAULT, panel_type,               ERL_NIF_TERM_TYPE_BITSTRING, CHAR_PTR) \
        X(DEFAULT, limit_refresh_rate_hz,    ERL_NIF_TERM_TYPE_INTEGER,   INT     ) \
        X(DEFAULT, disable_busy_waiting,     ERL_NIF_TERM_TYPE_ATOM,      BOOL    ) \
        X(RUNTIME, gpio_slowdown,            ERL_NIF_TERM_TYPE_INTEGER,   INT     ) \
        X(RUNTIME, daemon,                   ERL_NIF_TERM_TYPE_INTEGER,   INT     ) \
        X(RUNTIME, drop_privileges,          ERL_NIF_TERM_TYPE_INTEGER,   INT     ) \
        X(RUNTIME, do_gpio_init,             ERL_NIF_TERM_TYPE_ATOM,      BOOL    ) \
        X(RUNTIME, drop_priv_user,           ERL_NIF_TERM_TYPE_BITSTRING, CHAR_PTR) \
        X(RUNTIME, drop_priv_group,          ERL_NIF_TERM_TYPE_BITSTRING, CHAR_PTR)

#define SET_FIELD(opts_type, opts, field, value) SET_FIELD_##opts_type(opts, field, value)
#define SET_FIELD_DEFAULT(opts, field, value) ((struct RGBLedMatrixOptions *)opts)->field = value
#define SET_FIELD_RUNTIME(opts, field, value) ((struct RGBLedRuntimeOptions *)opts)->field = value

#define SETTER_ERL_NIF_TERM_TYPE_INTEGER(opts_type, field, _) \
static error_e set_##field(opts_type_e opts_type, void *opts, ErlNifEnv *env, ERL_NIF_TERM term) { \
    int value; \
    if (!enif_get_int(env, term, &value)) return BAD_ARG_EXPECT_INTEGER; \
    SET_FIELD(opts_type, opts, field, value); \
    return 0; \
}

#define SETTER_ERL_NIF_TERM_TYPE_BITSTRING(opts_type, field, _) \
static error_e set_##field(opts_type_e opts_type, void *opts, ErlNifEnv *env, ERL_NIF_TERM term) { \
    char *string; \
    error_e err = get_charlist(env, term, &string); \
    if (err) return err; \
    SET_FIELD(opts_type, opts, field, string); \
    return 0; \
}

#define SETTER_ERL_NIF_TERM_TYPE_ATOM(opts_type, field, c_type) SETTER_ERL_NIF_TERM_TYPE_ATOM_TO_##c_type(opts_type, field)
#define SETTER_ERL_NIF_TERM_TYPE_ATOM_TO_BOOL(opts_type, field) \
static error_e set_##field(opts_type_e opts_type, void *opts, ErlNifEnv *env, ERL_NIF_TERM term) { \
    bool value; \
    error_e err = get_bool(env, term, &value); \
    if (err) return err; \
    SET_FIELD(opts_type, opts, field, value); \
    return 0; \
}
#define SETTER_ERL_NIF_TERM_TYPE_ATOM_TO_CHAR_PTR(opts_type, field) \
static error_e set_##field(opts_type_e opts_type, void *opts, ErlNifEnv *env, ERL_NIF_TERM term) { \
    char *atom; \
    error_e err = get_atom(env, term, &atom); \
    if (err) return err; \
    SET_FIELD(opts_type, opts, field, atom); \
    return NONE; \
}

#define DISPATCH_SETTER(opts_type, field, erl_type, c_type) SETTER_##erl_type(opts_type, field, c_type)

#define X(opts_type, field, erl_type, c_type) DISPATCH_SETTER(opts_type, field, erl_type, c_type)
OPTIONS_KEYS
#undef X

typedef error_e (*setter_ptr)(opts_type_e, void *, ErlNifEnv *, ERL_NIF_TERM);

typedef struct {
    const char* field;
    setter_ptr setter;
    opts_type_e opts_type;
	c_type_e c_type;
} setter_entry_t;

#define X(opts_type, field, erl_type, c_type) { #field, set_##field, opts_type, c_type },
static const setter_entry_t setter_table[] = {
    OPTIONS_KEYS
};
#undef X

static const size_t setter_table_size =
    sizeof(setter_table) / sizeof(setter_table[0]);


static error_e set_field(ErlNifEnv *env, opts_type_e opts_type, void *opts, const char* key, ERL_NIF_TERM term)
{
    for (size_t i = 0; i < setter_table_size; i++) {
        if (strcmp(setter_table[i].field, key) == 0 && setter_table[i].opts_type == opts_type) {
            return setter_table[i].setter(opts_type, opts, env, term);
        }
    }
    return -1;
}

int parse_options(ErlNifEnv *env, ERL_NIF_TERM list, opts_type_e opts_type, void *opts)
{
    unsigned length;
    ERL_NIF_TERM head, tail;
    char *field;
    error_e err;
    int tuple_arity;
    const ERL_NIF_TERM *tuple_array;

    if (!enif_get_list_length(env, list, &length)) {
        return enif_make_badarg(env);
    }

    for (size_t i = 0; i < length; i++) {
        if (!enif_get_list_cell(env, list, &head, &tail)) {
            return enif_make_badarg(env);
        }
        if (!enif_get_tuple(env, head, &tuple_arity, &tuple_array) || tuple_arity != 2) {
            return enif_make_badarg(env);
        }

        err = get_atom(env, tuple_array[0], &field);
        if (err) {
            // format err
            return enif_make_badarg(env);
        }
        if (set_field(env, opts_type, opts, field, tuple_array[1]) != 0) {
            return enif_make_badarg(env);
        }

        free(field);

        list = tail;
    }
}

