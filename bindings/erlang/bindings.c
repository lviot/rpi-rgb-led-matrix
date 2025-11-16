#include <erl_nif.h>
#include "led-matrix-c.h"
#include "options.h"
#include "types.h"

ErlNifResourceType *MATRIX_RES_TYPE;
ErlNifResourceType *CANVAS_RES_TYPE;
ErlNifResourceType *OPTIONS_RES_TYPE;
ErlNifResourceType *RT_OPTIONS_RES_TYPE;
ErlNifResourceType *FONT_RES_TYPE;

static void matrix_destructor(ErlNifEnv *env, void *res) {
    struct RGBLedMatrix **matrix_res = (struct RGBLedMatrix **)res;
    led_matrix_delete(*matrix_res);
}

static void options_destructor(ErlNifEnv *env, void *res) {
    struct RGBLedMatrixOptions *opts = *(struct RGBLedMatrixOptions **)res;

    if (opts->hardware_mapping) free((char *)opts->hardware_mapping);
    if (opts->led_rgb_sequence) free((char *)opts->led_rgb_sequence);
    if (opts->pixel_mapper_config) free((char *)opts->pixel_mapper_config);
    if (opts->panel_type) free((char *)opts->panel_type);
}

static void rt_options_destructor(ErlNifEnv *env, void *res) {
    struct RGBLedRuntimeOptions *opts = *(struct RGBLedRuntimeOptions **)res;

    if (opts->drop_priv_user) free((char *)opts->drop_priv_user);
    if (opts->drop_priv_group) free((char *)opts->drop_priv_group);
}

static void font_destructor(ErlNifEnv *env, void *res) {
    struct LedFont **font_res = (struct LedFont **)res;
    delete_font(*font_res);
}

static int load(ErlNifEnv *env, void **priv_data, ERL_NIF_TERM load_info) {
    int flags = ERL_NIF_RT_CREATE | ERL_NIF_RT_TAKEOVER;

    MATRIX_RES_TYPE = enif_open_resource_type(env, NULL, "matrix", matrix_destructor, flags, NULL);
    CANVAS_RES_TYPE = enif_open_resource_type(env, NULL, "canvas", NULL, flags, NULL);
    OPTIONS_RES_TYPE = enif_open_resource_type(env, NULL, "options", NULL, flags, NULL);
    RT_OPTIONS_RES_TYPE = enif_open_resource_type(env, NULL, "rt_options", NULL, flags, NULL);
    FONT_RES_TYPE = enif_open_resource_type(env, NULL, "font", font_destructor, flags, NULL);

    return 0;
}

static ERL_NIF_TERM create_options_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    struct RGBLedMatrixOptions *options_res = enif_alloc_resource(OPTIONS_RES_TYPE, sizeof(struct RGBLedMatrixOptions));
    ERL_NIF_TERM term;

    memset(options_res, 0, sizeof(struct RGBLedMatrixOptions));

    parse_options(env, argv[0], DEFAULT, options_res);

    term = enif_make_resource(env, options_res);

    enif_release_resource(options_res);
    return term;
}

static ERL_NIF_TERM create_rt_options_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    struct RGBLedRuntimeOptions *rt_options_res = enif_alloc_resource(RT_OPTIONS_RES_TYPE, sizeof(struct RGBLedRuntimeOptions));

    memset(rt_options_res, 0, sizeof(struct RGBLedRuntimeOptions));

    parse_options(env, argv[0], RUNTIME, rt_options_res);

    ERL_NIF_TERM term = enif_make_resource(env, rt_options_res);

    enif_release_resource(rt_options_res);
    return term;
}

static ERL_NIF_TERM led_matrix_create_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    int rows, chained, parallel;

    if (!enif_get_int(env, argv[0], &rows) ||
        !enif_get_int(env, argv[1], &chained) ||
        !enif_get_int(env, argv[2], &parallel)) {
        return enif_make_badarg(env);
    }

    struct RGBLedMatrix **matrix_res = enif_alloc_resource(MATRIX_RES_TYPE, sizeof(struct RGBLedMatrix *));

    *matrix_res = led_matrix_create(rows, chained, parallel);

    ERL_NIF_TERM term = enif_make_resource(env, matrix_res);

    enif_release_resource(matrix_res);

    return term;
}

static ERL_NIF_TERM led_matrix_create_from_options_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    struct RGBLedMatrixOptions **options_res;

    if (!enif_get_resource(env, argv[0], OPTIONS_RES_TYPE, (void *)&options_res)) {
        return enif_make_badarg(env);
    }

    struct RGBLedMatrix **matrix_res = enif_alloc_resource(MATRIX_RES_TYPE, sizeof(struct RGBLedMatrix *));

    *matrix_res = led_matrix_create_from_options(*options_res, NULL, NULL);

    ERL_NIF_TERM term = enif_make_resource(env, matrix_res);

    enif_release_resource(matrix_res);

    return term;
}

static ERL_NIF_TERM led_matrix_create_from_options_and_rt_options_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    struct RGBLedMatrixOptions *options_res;
    struct RGBLedRuntimeOptions *rt_options_res;

    if (!enif_get_resource(env, argv[0], OPTIONS_RES_TYPE, (void *)&options_res) ||
        !enif_get_resource(env, argv[1], RT_OPTIONS_RES_TYPE, (void *)&rt_options_res)) {
        return enif_make_badarg(env);
    }

    struct RGBLedMatrix **matrix_res = enif_alloc_resource(MATRIX_RES_TYPE, sizeof(struct RGBLedMatrix *));

    *matrix_res = led_matrix_create_from_options_and_rt_options(options_res, rt_options_res);

    ERL_NIF_TERM term = enif_make_resource(env, matrix_res);

    enif_release_resource(matrix_res);

    return term;
}

static ERL_NIF_TERM led_matrix_create_offscreen_canvas_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    struct RGBLedMatrix **matrix_res;

    if (!enif_get_resource(env, argv[0], MATRIX_RES_TYPE, (void *)&matrix_res)) {
        return enif_make_badarg(env);
    }

    struct LedCanvas **canvas_res = enif_alloc_resource(CANVAS_RES_TYPE, sizeof(struct LedCanvas *));

    *canvas_res = led_matrix_create_offscreen_canvas(*matrix_res);

    ERL_NIF_TERM term = enif_make_resource(env, canvas_res);

    enif_release_resource(canvas_res);

    return term;
}

static ERL_NIF_TERM led_canvas_get_size_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    struct LedCanvas **canvas_res;
    int width = 0;
    int height = 0;

    if (!enif_get_resource(env, argv[0], CANVAS_RES_TYPE, (void *)&canvas_res)) {
        return enif_make_badarg(env);
    }

    led_canvas_get_size(*canvas_res, &width, &height);

    return enif_make_tuple(env, 2, enif_make_int(env, width), enif_make_int(env, height));
}

static ERL_NIF_TERM led_canvas_set_pixel_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    struct LedCanvas **canvas_res;
    int x, y, r, g, b;

    if (!enif_get_resource(env, argv[0], CANVAS_RES_TYPE, (void *)&canvas_res) ||
        !enif_get_int(env, argv[1], &x) ||
        !enif_get_int(env, argv[2], &y) ||
        !enif_get_int(env, argv[3], &r) ||
        !enif_get_int(env, argv[4], &g) ||
        !enif_get_int(env, argv[5], &b)) {
        printf("yppp\n");
        return enif_make_badarg(env);
    }

    led_canvas_set_pixel(*canvas_res, x, y, r, g, b);

    return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM led_canvas_clear_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    struct LedCanvas **canvas_res;

    if (!enif_get_resource(env, argv[0], CANVAS_RES_TYPE, (void *)&canvas_res)) {
        return enif_make_badarg(env);
    }

    led_canvas_clear(*canvas_res);

    return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM led_canvas_fill_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    struct LedCanvas **canvas_res;
    int r, g, b;

    if (!enif_get_resource(env, argv[0], CANVAS_RES_TYPE, (void *)&canvas_res) ||
        !enif_get_int(env, argv[1], &r) ||
        !enif_get_int(env, argv[2], &g) ||
        !enif_get_int(env, argv[3], &b)) {
        return enif_make_badarg(env);
    }

    led_canvas_fill(*canvas_res, r, g, b);

    return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM led_matrix_swap_on_vsync_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    struct RGBLedMatrix **matrix_res;
    struct LedCanvas **canvas_res;
    struct LedCanvas **new_canvas_res;
    ERL_NIF_TERM term;
    int x, y, r, g, b;

    if (!enif_get_resource(env, argv[0], MATRIX_RES_TYPE, (void *)&matrix_res) ||
        !enif_get_resource(env, argv[1], CANVAS_RES_TYPE, (void *)&canvas_res)) {
        return enif_make_badarg(env);
    }

    new_canvas_res = enif_alloc_resource(CANVAS_RES_TYPE, sizeof(struct LedCanvas *));
    *new_canvas_res = led_matrix_swap_on_vsync(*matrix_res, *canvas_res);
    term = enif_make_resource(env, new_canvas_res);
    enif_release_resource(new_canvas_res);
    return term;
}

static ERL_NIF_TERM led_matrix_get_brightness_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    struct RGBLedMatrix **matrix_res;
    int brightness;

    if (!enif_get_resource(env, argv[0], MATRIX_RES_TYPE, (void *)&matrix_res)) {
        return enif_make_badarg(env);
    }

    brightness = led_matrix_get_brightness(*matrix_res);

    return enif_make_int(env, brightness);
}

static ERL_NIF_TERM led_matrix_set_brightness_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    struct RGBLedMatrix **matrix_res;
    int brightness;

    if (!enif_get_resource(env, argv[0], MATRIX_RES_TYPE, (void *)&matrix_res) ||
        !enif_get_int(env, argv[1], &brightness)) {
        return enif_make_badarg(env);
    }

    led_matrix_set_brightness(*matrix_res, brightness);

    return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM set_image_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    struct LedCanvas **canvas_res;
    int offset_x;
    int offset_y;
    ErlNifBinary image_bin;
    int image_width;
    int image_height;
    int is_bgr;

    if (!enif_get_resource(env, argv[0], CANVAS_RES_TYPE, (void *)&canvas_res) ||
        !enif_get_int(env, argv[1], &offset_x) ||
        !enif_get_int(env, argv[2], &offset_y) ||
        !enif_inspect_binary(env, argv[3], &image_bin) ||
        !enif_get_int(env, argv[4], &image_width) ||
        !enif_get_int(env, argv[5], &image_height) ||
        !enif_get_int(env, argv[6], &is_bgr)) {
        return enif_make_badarg(env);
    }

    set_image(*canvas_res, offset_x, offset_y, image_bin.data, image_bin.size, image_width, image_height, (char)is_bgr);

    return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM load_font_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    struct LedFont *font;
    struct LedFont **font_res;
    ERL_NIF_TERM term;
    char *path;
    error_e err;

    err = get_charlist(env, argv[0], &path);

    if (err) {
        return enif_make_badarg(env);
    }

    font_res = enif_alloc_resource(FONT_RES_TYPE, sizeof(struct Font *));
    *font_res = load_font(path);
    term = enif_make_resource(env, font_res);
    enif_release_resource(font_res);
    return term;
}

static ERL_NIF_TERM baseline_font_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    struct LedFont **font_res;
    int baseline;

    if (!enif_get_resource(env, argv[0], FONT_RES_TYPE, (void *)&font_res)) {
        return enif_make_badarg(env);
    }

    baseline = baseline_font(*font_res);

    return enif_make_int(env, baseline);
}

static ERL_NIF_TERM height_font_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    struct LedFont **font_res;
    int height;

    if (!enif_get_resource(env, argv[0], FONT_RES_TYPE, (void *)&font_res)) {
        return enif_make_badarg(env);
    }

    height = height_font(*font_res);

    return enif_make_int(env, height);
}

static ERL_NIF_TERM character_width_font_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    struct LedFont **font_res;
    uint32_t unicode_codepoint;
    int character_width;

    if (!enif_get_resource(env, argv[0], FONT_RES_TYPE, (void *)&font_res) ||
        !enif_get_uint(env, argv[1], &unicode_codepoint)) {
        return enif_make_badarg(env);
    }

    character_width = character_width_font(*font_res, unicode_codepoint);

    return enif_make_int(env, character_width);
}

static ERL_NIF_TERM create_outline_font_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    struct LedFont **font_res;
    struct LedFont **new_font_res;
    ERL_NIF_TERM term;

    if (!enif_get_resource(env, argv[0], FONT_RES_TYPE, (void *)&font_res)) {
        return enif_make_badarg(env);
    }

    new_font_res = enif_alloc_resource(FONT_RES_TYPE, sizeof(struct LedFont *));
    *new_font_res = create_outline_font(*font_res);
    term = enif_make_resource(env, new_font_res);
    enif_release_resource(new_font_res);
    return term;
}

static ERL_NIF_TERM draw_text_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    struct LedCanvas **canvas_res;
    struct LedFont **font_res;
    int x;
    int y;
    uint32_t r;
    uint32_t g;
    uint32_t b;
    char *utf8_text;
    int kerning_offset;
    int offset_x;

    if (!enif_get_resource(env, argv[0], CANVAS_RES_TYPE, (void *)&canvas_res) ||
        !enif_get_resource(env, argv[1], FONT_RES_TYPE, (void *)&font_res) ||
        !enif_get_int(env, argv[2], &x) ||
        !enif_get_int(env, argv[3], &y) ||
        !enif_get_uint(env, argv[4], &r) ||
        !enif_get_uint(env, argv[5], &g) ||
        !enif_get_uint(env, argv[6], &b) ||
        get_charlist(env, argv[7], &utf8_text) ||
        !enif_get_int(env, argv[8], &kerning_offset)) {
        return enif_make_badarg(env);
    }

    if (r > 255 || g > 255 || b > 255) {
        return enif_make_badarg(env);
    }

    offset_x = draw_text(*canvas_res, *font_res, x, y, r, g, b, utf8_text, kerning_offset);

    return enif_make_int(env, offset_x);
}

static ERL_NIF_TERM vertical_draw_text_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    struct LedCanvas **canvas_res;
    struct LedFont **font_res;
    int x;
    int y;
    uint32_t r;
    uint32_t g;
    uint32_t b;
    char *utf8_text;
    int kerning_offset;
    int offset_x;

    if (!enif_get_resource(env, argv[0], CANVAS_RES_TYPE, (void *)&canvas_res) ||
        !enif_get_resource(env, argv[1], FONT_RES_TYPE, (void *)&font_res) ||
        !enif_get_int(env, argv[2], &x) ||
        !enif_get_int(env, argv[3], &y) ||
        !enif_get_uint(env, argv[4], &r) ||
        !enif_get_uint(env, argv[5], &g) ||
        !enif_get_uint(env, argv[6], &b) ||
        get_charlist(env, argv[7], &utf8_text) ||
        !enif_get_int(env, argv[8], &kerning_offset)) {
        return enif_make_badarg(env);
    }

    if (r > 255 || g > 255 || b > 255) {
        return enif_make_badarg(env);
    }

    offset_x = vertical_draw_text(*canvas_res, *font_res, x, y, r, g, b, utf8_text, kerning_offset);

    return enif_make_int(env, offset_x);
}

static ERL_NIF_TERM draw_circle_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    struct LedCanvas **canvas_res;
    int x;
    int y;
    int radius;
    uint32_t r;
    uint32_t g;
    uint32_t b;

    if (!enif_get_resource(env, argv[0], CANVAS_RES_TYPE, (void *)&canvas_res) ||
        !enif_get_int(env, argv[1], &x) ||
        !enif_get_int(env, argv[2], &y) ||
        !enif_get_int(env, argv[3], &radius) ||
        !enif_get_uint(env, argv[4], &r) ||
        !enif_get_uint(env, argv[5], &g) ||
        !enif_get_uint(env, argv[6], &b)) {
        return enif_make_badarg(env);
    }

    if (r > 255 || g > 255 || b > 255) {
        return enif_make_badarg(env);
    }

    draw_circle(*canvas_res, x, y, radius, r, g, b);

    return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM draw_line_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    struct LedCanvas **canvas_res;
    int x0;
    int y0;
    int x1;
    int y1;
    uint32_t r;
    uint32_t g;
    uint32_t b;

    if (!enif_get_resource(env, argv[0], CANVAS_RES_TYPE, (void *)&canvas_res) ||
        !enif_get_int(env, argv[1], &x0) ||
        !enif_get_int(env, argv[2], &y0) ||
        !enif_get_int(env, argv[3], &x1) ||
        !enif_get_int(env, argv[4], &y1) ||
        !enif_get_uint(env, argv[5], &r) ||
        !enif_get_uint(env, argv[6], &g) ||
        !enif_get_uint(env, argv[7], &b)) {
        return enif_make_badarg(env);
        }

    if (r > 255 || g > 255 || b > 255) {
        return enif_make_badarg(env);
    }

    draw_line(*canvas_res, x0, y0, x1, y1, r, g, b);

    return enif_make_atom(env, "ok");
}

static ErlNifFunc nif_funcs[] = {
    {"create_options", 1, create_options_nif},
    {"create_rt_options", 1, create_rt_options_nif},
    {"led_matrix_create", 3, led_matrix_create_nif},
    {"led_matrix_create_from_options", 1, led_matrix_create_from_options_nif},
    {"led_matrix_create_from_options_and_rt_options", 2, led_matrix_create_from_options_and_rt_options_nif},
    {"led_matrix_create_offscreen_canvas", 1, led_matrix_create_offscreen_canvas_nif},
    {"led_canvas_get_size", 1, led_canvas_get_size_nif},
    {"led_canvas_set_pixel", 6, led_canvas_set_pixel_nif},
    {"led_canvas_clear", 1, led_canvas_clear_nif},
    {"led_canvas_fill", 4, led_canvas_fill_nif},
    {"led_matrix_swap_on_vsync", 2, led_matrix_swap_on_vsync_nif},
    {"led_matrix_get_brightness", 1, led_matrix_get_brightness_nif},
    {"led_matrix_set_brightness", 2, led_matrix_set_brightness_nif},
    {"set_image", 7, set_image_nif},
    {"load_font", 1, load_font_nif},
    {"baseline_font", 1, baseline_font_nif},
    {"height_font", 1, height_font_nif},
    {"character_width_font", 2, character_width_font_nif},
    {"create_outline_font", 1, create_outline_font_nif},
    {"draw_text", 9, draw_text_nif},
    {"vertical_draw_text", 9, vertical_draw_text_nif},
    {"draw_circle", 7, draw_circle_nif},
    {"draw_line", 8, draw_line_nif},
};

ERL_NIF_INIT(Elixir.RpiRgb, nif_funcs, &load, NULL, NULL, NULL)
