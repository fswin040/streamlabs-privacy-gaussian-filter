#include "frosted-glass-filter.hpp"
#include "filter-settings.hpp"
#include "gaussian-blur.hpp"

#include <graphics/vec2.h>
#include <graphics/vec4.h>
#include <obs-module.h>

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <string>

namespace {
std::atomic<uint64_t> g_render_count{0};
std::atomic<uint64_t> g_gaussian_pass_count{0};
constexpr const char *kRegionMode = "region_mode";
constexpr const char *kRegionX = "region_x";
constexpr const char *kRegionY = "region_y";
constexpr const char *kRegionWidth = "region_width";
constexpr const char *kRegionHeight = "region_height";
constexpr const char *kBlurStrength = "blur_strength";
constexpr const char *kBlurQuality = "blur_quality";

struct FrostedGlassFilter {
    obs_source_t *context = nullptr;
    FilterSettings settings = default_filter_settings();
    gs_effect_t *blur_effect = nullptr;
    gs_effect_t *glass_effect = nullptr;
    gs_effect_t *output_effect = nullptr;
    gs_eparam_t *blur_image = nullptr;
    gs_eparam_t *blur_texel_step = nullptr;
    gs_eparam_t *blur_source_rect = nullptr;
    gs_eparam_t *blur_center_weight = nullptr;
    gs_eparam_t *blur_pair_weights = nullptr;
    gs_eparam_t *blur_pair_offsets = nullptr;
    gs_eparam_t *glass_image = nullptr;
    gs_eparam_t *glass_blurred_image = nullptr;
    gs_eparam_t *glass_uv_rect = nullptr;
    gs_eparam_t *glass_blur_rect = nullptr;
    gs_eparam_t *output_image = nullptr;
    gs_texrender_t *input = nullptr;
    gs_texrender_t *horizontal = nullptr;
    gs_texrender_t *blurred = nullptr;
    gs_texrender_t *output = nullptr;
    uint32_t width = 0;
    uint32_t height = 0;
    bool rendering = false;
};

gs_texrender_t *reset_texrender(gs_texrender_t *value)
{
    if (value)
        gs_texrender_reset(value);
    else
        value = gs_texrender_create(GS_RGBA, GS_ZS_NONE);
    return value;
}

gs_effect_t *load_effect(const char *relative_path)
{
    char *path = obs_module_file(relative_path);
    char *errors = nullptr;
    gs_effect_t *effect = path ? gs_effect_create_from_file(path, &errors) : nullptr;
    if (!effect)
        blog(LOG_ERROR, "[Motion Frosted Glass] Unable to load %s: %s", relative_path, errors ? errors : "unknown error");
    bfree(errors);
    bfree(path);
    return effect;
}

void set_defaults(obs_data_t *settings)
{
    const auto value = default_filter_settings();
    obs_data_set_default_int(settings, kRegionMode, static_cast<long long>(value.region_mode));
    obs_data_set_default_double(settings, kRegionX, value.region.x * 100.0);
    obs_data_set_default_double(settings, kRegionY, value.region.y * 100.0);
    obs_data_set_default_double(settings, kRegionWidth, value.region.width * 100.0);
    obs_data_set_default_double(settings, kRegionHeight, value.region.height * 100.0);
    obs_data_set_default_double(settings, kBlurStrength, value.blur_strength);
    obs_data_set_default_int(settings, kBlurQuality, static_cast<long long>(value.blur_quality));
}

FilterSettings read_settings(obs_data_t *data)
{
    FilterSettings value = default_filter_settings();
    value.region_mode = static_cast<RegionMode>(obs_data_get_int(data, kRegionMode));
    value.region = {
        static_cast<float>(obs_data_get_double(data, kRegionX) / 100.0),
        static_cast<float>(obs_data_get_double(data, kRegionY) / 100.0),
        static_cast<float>(obs_data_get_double(data, kRegionWidth) / 100.0),
        static_cast<float>(obs_data_get_double(data, kRegionHeight) / 100.0),
    };
    value.blur_strength = static_cast<float>(obs_data_get_double(data, kBlurStrength));
    value.blur_quality = static_cast<BlurQuality>(obs_data_get_int(data, kBlurQuality));
    return normalize_filter_settings(value);
}

void update(void *data, obs_data_t *settings)
{
    static_cast<FrostedGlassFilter *>(data)->settings = read_settings(settings);
}

void *create(obs_data_t *settings, obs_source_t *source)
{
    auto *filter = new FrostedGlassFilter;
    filter->context = source;
    filter->settings = read_settings(settings);
    obs_enter_graphics();
    filter->blur_effect = load_effect("effects/blur.effect");
    filter->glass_effect = load_effect("effects/frosted-glass.effect");
    filter->output_effect = load_effect("effects/output.effect");
    filter->output_image = filter->output_effect
                               ? gs_effect_get_param_by_name(filter->output_effect, "output_image")
                               : nullptr;
    if (filter->blur_effect) {
        filter->blur_image = gs_effect_get_param_by_name(filter->blur_effect, "image");
        filter->blur_texel_step = gs_effect_get_param_by_name(filter->blur_effect, "texel_step");
        filter->blur_source_rect = gs_effect_get_param_by_name(filter->blur_effect, "source_rect");
        filter->blur_center_weight = gs_effect_get_param_by_name(filter->blur_effect, "center_weight");
        filter->blur_pair_weights = gs_effect_get_param_by_name(filter->blur_effect, "pair_weights");
        filter->blur_pair_offsets = gs_effect_get_param_by_name(filter->blur_effect, "pair_offsets");
    }
    if (filter->glass_effect) {
        filter->glass_image = gs_effect_get_param_by_name(filter->glass_effect, "image");
        filter->glass_blurred_image = gs_effect_get_param_by_name(filter->glass_effect, "blurred_image");
        filter->glass_uv_rect = gs_effect_get_param_by_name(filter->glass_effect, "uv_rect");
        filter->glass_blur_rect = gs_effect_get_param_by_name(filter->glass_effect, "blur_rect");
    }
    filter->input = gs_texrender_create(GS_RGBA, GS_ZS_NONE);
    filter->horizontal = gs_texrender_create(GS_RGBA, GS_ZS_NONE);
    filter->blurred = gs_texrender_create(GS_RGBA, GS_ZS_NONE);
    filter->output = gs_texrender_create(GS_RGBA, GS_ZS_NONE);
    obs_leave_graphics();
    return filter;
}

void destroy(void *data)
{
    auto *filter = static_cast<FrostedGlassFilter *>(data);
    obs_enter_graphics();
    gs_effect_destroy(filter->blur_effect);
    gs_effect_destroy(filter->glass_effect);
    gs_effect_destroy(filter->output_effect);
    gs_texrender_destroy(filter->input);
    gs_texrender_destroy(filter->horizontal);
    gs_texrender_destroy(filter->blurred);
    gs_texrender_destroy(filter->output);
    obs_leave_graphics();
    delete filter;
}

bool region_modified(obs_properties_t *properties, obs_property_t *, obs_data_t *settings)
{
    const bool custom = obs_data_get_int(settings, kRegionMode) == static_cast<long long>(RegionMode::Custom);
    for (const char *name : {kRegionX, kRegionY, kRegionWidth, kRegionHeight})
        obs_property_set_visible(obs_properties_get(properties, name), custom);
    return true;
}

bool reset_defaults(obs_properties_t *, obs_property_t *, void *data)
{
    auto *filter = static_cast<FrostedGlassFilter *>(data);
    if (!filter || !filter->context)
        return false;
    obs_data_t *settings = obs_source_get_settings(filter->context);
    obs_data_clear(settings);
    set_defaults(settings);
    obs_source_update(filter->context, settings);
    obs_data_release(settings);
    return true;
}

obs_properties_t *properties(void *)
{
    obs_properties_t *props = obs_properties_create();
    obs_property_t *region = obs_properties_add_list(props, kRegionMode, obs_module_text("RegionMode"), OBS_COMBO_TYPE_LIST,
                                                     OBS_COMBO_FORMAT_INT);
    obs_property_list_add_int(region, obs_module_text("RegionFull"), static_cast<long long>(RegionMode::FullSource));
    obs_property_list_add_int(region, obs_module_text("RegionCustom"), static_cast<long long>(RegionMode::Custom));
    obs_property_set_modified_callback(region, region_modified);
    obs_properties_add_float_slider(props, kRegionX, obs_module_text("RegionX"), 0.0, 99.0, 0.1);
    obs_properties_add_float_slider(props, kRegionY, obs_module_text("RegionY"), 0.0, 99.0, 0.1);
    obs_properties_add_float_slider(props, kRegionWidth, obs_module_text("RegionWidth"), 1.0, 100.0, 0.1);
    obs_properties_add_float_slider(props, kRegionHeight, obs_module_text("RegionHeight"), 1.0, 100.0, 0.1);
    obs_properties_add_float_slider(props, kBlurStrength, obs_module_text("BlurStrength"), 0.0, 64.0, 0.5);
    obs_property_t *quality = obs_properties_add_list(props, kBlurQuality, obs_module_text("BlurQuality"), OBS_COMBO_TYPE_LIST,
                                                      OBS_COMBO_FORMAT_INT);
    obs_property_list_add_int(quality, obs_module_text("QualityLow"), 0);
    obs_property_list_add_int(quality, obs_module_text("QualityMedium"), 1);
    obs_property_list_add_int(quality, obs_module_text("QualityHigh"), 2);
    obs_properties_add_button(props, "reset_defaults", obs_module_text("ResetDefaults"), reset_defaults);
    return props;
}

bool capture_input(FrostedGlassFilter *filter)
{
    obs_source_t *target = obs_filter_get_target(filter->context);
    if (!target)
        return false;
    filter->width = obs_source_get_base_width(target);
    filter->height = obs_source_get_base_height(target);
    if (!filter->width || !filter->height)
        return false;
    filter->input = reset_texrender(filter->input);
    if (!obs_source_process_filter_begin(filter->context, GS_RGBA, OBS_ALLOW_DIRECT_RENDERING) ||
        !gs_texrender_begin(filter->input, filter->width, filter->height))
        return false;
    gs_ortho(0.0f, static_cast<float>(filter->width), 0.0f, static_cast<float>(filter->height), -100.0f, 100.0f);
    obs_source_process_filter_tech_end(filter->context, obs_get_base_effect(OBS_EFFECT_DEFAULT), filter->width,
                                       filter->height, "Draw");
    gs_texrender_end(filter->input);
    return gs_texrender_get_texture(filter->input) != nullptr;
}

bool blur_pass(FrostedGlassFilter *filter, gs_texture_t *source, gs_texrender_t *destination, float x, float y,
                float sigma, uint32_t render_width, uint32_t render_height, NormalizedRect source_rect)
{
    if (!filter->blur_effect || !source)
        return false;
    destination = reset_texrender(destination);
    gs_effect_set_texture(filter->blur_image, source);
    vec2 step = {x, y};
    gs_effect_set_vec2(filter->blur_texel_step, &step);
    vec4 source_uv = {source_rect.x, source_rect.y, source_rect.width, source_rect.height};
    gs_effect_set_vec4(filter->blur_source_rect, &source_uv);
    const GaussianLinearSamples samples = gaussian_linear_samples(sigma);
    vec2 pair_weights = {samples.pair_weights[0], samples.pair_weights[1]};
    vec2 pair_offsets = {samples.pair_offsets[0], samples.pair_offsets[1]};
    gs_effect_set_float(filter->blur_center_weight, samples.center_weight);
    gs_effect_set_vec2(filter->blur_pair_weights, &pair_weights);
    gs_effect_set_vec2(filter->blur_pair_offsets, &pair_offsets);
    if (!gs_texrender_begin(destination, render_width, render_height))
        return false;
    gs_ortho(0.0f, static_cast<float>(render_width), 0.0f, static_cast<float>(render_height), -100.0f, 100.0f);
    while (gs_effect_loop(filter->blur_effect, "Draw"))
        gs_draw_sprite(source, 0, render_width, render_height);
    gs_texrender_end(destination);
    g_gaussian_pass_count.fetch_add(1, std::memory_order_relaxed);
    return true;
}

bool composite(FrostedGlassFilter *filter, gs_texture_t *original, gs_texture_t *blurred, NormalizedRect blur_rect)
{
    if (!filter->glass_effect || !original || !blurred)
        return false;
    filter->output = reset_texrender(filter->output);
    gs_effect_t *effect = filter->glass_effect;
    gs_effect_set_texture(filter->glass_image, original);
    gs_effect_set_texture(filter->glass_blurred_image, blurred);
    const auto rect = filter->settings.region_mode == RegionMode::FullSource
                          ? NormalizedRect{0.0f, 0.0f, 1.0f, 1.0f}
                          : filter->settings.region;
    vec4 uv_rect = {rect.x, rect.y, rect.width, rect.height};
    gs_effect_set_vec4(filter->glass_uv_rect, &uv_rect);
    vec4 blur_uv_rect = {blur_rect.x, blur_rect.y, blur_rect.width, blur_rect.height};
    gs_effect_set_vec4(filter->glass_blur_rect, &blur_uv_rect);
    if (!gs_texrender_begin(filter->output, filter->width, filter->height))
        return false;
    gs_ortho(0.0f, static_cast<float>(filter->width), 0.0f, static_cast<float>(filter->height), -100.0f, 100.0f);
    while (gs_effect_loop(effect, "Draw"))
        gs_draw_sprite(original, 0, filter->width, filter->height);
    gs_texrender_end(filter->output);
    return true;
}

void render(void *data, gs_effect_t *)
{
    auto *filter = static_cast<FrostedGlassFilter *>(data);
    if (gaussian_should_bypass(filter->settings.blur_strength)) {
        obs_source_skip_video_filter(filter->context);
        return;
    }
    if (filter->rendering || !filter->blur_effect || !filter->glass_effect || !filter->output_effect ||
        !filter->output_image) {
        obs_source_skip_video_filter(filter->context);
        return;
    }
    filter->rendering = true;
    g_render_count.fetch_add(1, std::memory_order_relaxed);
    bool ok = capture_input(filter);
    gs_texture_t *original = ok ? gs_texrender_get_texture(filter->input) : nullptr;
    const int iterations = gaussian_iteration_count(filter->settings.blur_quality);
    const int divisor = gaussian_downsample_divisor(filter->settings.blur_strength);
    const NormalizedRect blur_rect = filter->settings.region_mode == RegionMode::Custom
                                         ? gaussian_padded_region(filter->settings.region, filter->width, filter->height,
                                                                  filter->settings.blur_strength,
                                                                  filter->settings.blur_quality)
                                         : NormalizedRect{0.0f, 0.0f, 1.0f, 1.0f};
    const uint32_t roi_width = std::max<uint32_t>(1, static_cast<uint32_t>(filter->width * blur_rect.width + 0.5f));
    const uint32_t roi_height = std::max<uint32_t>(1, static_cast<uint32_t>(filter->height * blur_rect.height + 0.5f));
    const uint32_t blur_width = std::max<uint32_t>(1, roi_width / static_cast<uint32_t>(divisor));
    const uint32_t blur_height = std::max<uint32_t>(1, roi_height / static_cast<uint32_t>(divisor));
    const float sigma = gaussian_pass_sigma(filter->settings.blur_strength, iterations);
    gs_texture_t *iteration_source = original;
    NormalizedRect iteration_rect = blur_rect;
    for (int iteration = 0; ok && iteration < iterations; ++iteration) {
        ok = blur_pass(filter, iteration_source, filter->horizontal, 1.0f / blur_width, 0.0f, sigma, blur_width,
                        blur_height, iteration_rect);
        gs_texture_t *horizontal = ok ? gs_texrender_get_texture(filter->horizontal) : nullptr;
        ok = ok && blur_pass(filter, horizontal, filter->blurred, 0.0f, 1.0f / blur_height, sigma, blur_width,
                              blur_height, {0.0f, 0.0f, 1.0f, 1.0f});
        iteration_source = ok ? gs_texrender_get_texture(filter->blurred) : nullptr;
        iteration_rect = {0.0f, 0.0f, 1.0f, 1.0f};
    }
    gs_texture_t *blurred = ok ? gs_texrender_get_texture(filter->blurred) : nullptr;
    ok = ok && composite(filter, original, blurred, blur_rect);
    if (ok) {
        gs_texture_t *output = gs_texrender_get_texture(filter->output);
        if (obs_source_process_filter_begin(filter->context, GS_RGBA, OBS_ALLOW_DIRECT_RENDERING)) {
            gs_effect_set_texture(filter->output_image, output);
            obs_source_process_filter_end(filter->context, filter->output_effect, filter->width, filter->height);
        } else {
            ok = false;
        }
    }
    if (!ok)
        obs_source_skip_video_filter(filter->context);
    filter->rendering = false;
}

const char *name(void *)
{
    return obs_module_text("MotionFrostedGlass");
}

obs_source_info make_source_info()
{
    obs_source_info value = {};
    // Streamlabs Desktop 1.21.x only exposes a hard-coded filter allowlist.
    // shader_filter is allowlisted and absent from the bundled runtime, so this
    // compatibility ID makes the filter reachable without patching the app.
    value.id = "shader_filter";
    value.type = OBS_SOURCE_TYPE_FILTER;
    value.output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_SRGB;
    value.get_name = name;
    value.create = create;
    value.destroy = destroy;
    value.get_defaults = set_defaults;
    value.get_properties = properties;
    value.update = update;
    value.video_render = render;
    return value;
}

obs_source_info info = make_source_info();
}

extern "C" __declspec(dllexport) unsigned long long motion_frosted_glass_render_count()
{
    return static_cast<unsigned long long>(g_render_count.load(std::memory_order_relaxed));
}

extern "C" __declspec(dllexport) unsigned long long motion_frosted_gaussian_pass_count()
{
    return static_cast<unsigned long long>(g_gaussian_pass_count.load(std::memory_order_relaxed));
}

const obs_source_info *motion_frosted_glass_source_info()
{
    return &info;
}
