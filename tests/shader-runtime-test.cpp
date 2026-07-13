#include <graphics/graphics.h>

#include <iostream>
#include <initializer_list>

static bool require_params(gs_effect_t *effect, const char *label,
                           std::initializer_list<const char *> names)
{
    bool ok = true;
    for (const char *name : names) {
        if (!gs_effect_get_param_by_name(effect, name)) {
            std::cerr << label << " missing runtime parameter: " << name << '\n';
            ok = false;
        }
    }
    return ok;
}

int main(int argc, char **argv)
{
    if (argc != 5) {
        std::cerr << "Usage: shader-runtime-test <graphics-module> <blur.effect> <glass.effect> <output.effect>\n";
        return 2;
    }
    graphics_t *graphics = nullptr;
    const int create_result = gs_create(&graphics, argv[1], 0);
    if (create_result != GS_SUCCESS) {
        std::cerr << "gs_create failed: " << create_result << '\n';
        return 4;
    }
    gs_enter_context(graphics);
    char *blur_errors = nullptr;
    char *glass_errors = nullptr;
    gs_effect_t *blur = gs_effect_create_from_file(argv[2], &blur_errors);
    gs_effect_t *glass = gs_effect_create_from_file(argv[3], &glass_errors);
    char *output_errors = nullptr;
    gs_effect_t *output = gs_effect_create_from_file(argv[4], &output_errors);
    if (!blur)
        std::cerr << "blur.effect: " << (blur_errors ? blur_errors : "unknown error") << '\n';
    if (!glass)
        std::cerr << "frosted-glass.effect: " << (glass_errors ? glass_errors : "unknown error") << '\n';
    if (!output)
        std::cerr << "output.effect: " << (output_errors ? output_errors : "unknown error") << '\n';
    bfree(blur_errors);
    bfree(glass_errors);
    bfree(output_errors);
    bool params_ok = blur && glass && output;
    if (blur)
        params_ok &= require_params(blur, "blur.effect", {"image", "texel_step", "source_rect", "center_weight",
                                                                  "pair_weights", "pair_offsets"});
    if (glass)
        params_ok &= require_params(glass, "frosted-glass.effect", {"image", "blurred_image", "uv_rect", "blur_rect"});
    if (output)
        params_ok &= require_params(output, "output.effect", {"image", "output_image"});
    gs_effect_destroy(blur);
    gs_effect_destroy(glass);
    gs_effect_destroy(output);
    gs_leave_context();
    gs_destroy(graphics);
    if (!params_ok)
        return 5;
    std::cout << "All shaders compiled with OBS Direct3D 11\n";
    return 0;
}
