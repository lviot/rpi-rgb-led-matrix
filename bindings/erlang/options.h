//
// Created by Louis Viot on 16/11/2025.
//

#ifndef RPI_RGB_LED_MATRIX_OPTIONS_H
#define RPI_RGB_LED_MATRIX_OPTIONS_H

#include <erl_nif.h>
#include "led-matrix-c.h"

typedef enum {
    DEFAULT,
    RUNTIME,
} opts_type_e;

typedef enum {
    CHAR_PTR,
    INT,
    BOOL,
} c_type_e;

int parse_options(ErlNifEnv* env, ERL_NIF_TERM list, opts_type_e opts_type, void *opts);

#endif //RPI_RGB_LED_MATRIX_OPTIONS_H