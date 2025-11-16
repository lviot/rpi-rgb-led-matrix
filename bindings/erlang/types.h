//
// Created by Louis Viot on 22/11/2025.
//

#ifndef RPI_RGB_LED_MATRIX_TYPES_H
#define RPI_RGB_LED_MATRIX_TYPES_H

#include <erl_nif.h>
#include <stdbool.h>
#include "errors.h"

error_e get_charlist(ErlNifEnv *, ERL_NIF_TERM, char **);

error_e get_atom(ErlNifEnv *, ERL_NIF_TERM, char **);

error_e get_bool(ErlNifEnv *, ERL_NIF_TERM, bool *);


#endif //RPI_RGB_LED_MATRIX_TYPES_H