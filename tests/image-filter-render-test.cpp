#include <obs.h>
#include <obs-module.h>
#include <graphics/graphics.h>
#include <util/base.h>
#include <windows.h>

#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <cstring>
#include <atomic>

static std::atomic<int> render_errors{0};
using RenderCountFn = unsigned long long (*)();
using GaussianPassCountFn = unsigned long long (*)();

static void render_display(void *, uint32_t, uint32_t)
{
    obs_render_main_texture();
}

static void test_log_handler(int level, const char *format, va_list args, void *)
{
    char message[4096];
    vsnprintf(message, sizeof(message), format, args);
    std::cerr << message << '\n';
    if (level <= LOG_ERROR &&
        (std::strstr(message, "effect_setval_inline: invalid param") ||
         std::strstr(message, "No vertex shader specified")))
        ++render_errors;
}

static bool load_module(const char *binary, const char *data)
{
    obs_module_t *module = nullptr;
    const int result = obs_open_module(&module, binary, data);
    return result == MODULE_SUCCESS && obs_init_module(module);
}

int main(int argc, char **argv)
{
    if (argc != 8) {
        std::cerr << "Usage: image-filter-render-test <graphics> <libobs-data> <image-module> <image-data> <filter-module> <filter-data> <image>\n";
        return 2;
    }
    base_set_log_handler(test_log_handler, nullptr);
    if (!obs_startup("en-US", nullptr, nullptr))
        return 3;
    const std::string libobs_data = std::string(argv[2]) + "/";
    obs_add_data_path(libobs_data.c_str());

    obs_video_info video = {};
    video.adapter = 0;
    video.base_width = 640;
    video.base_height = 360;
    video.output_width = 640;
    video.output_height = 360;
    video.fps_num = 60;
    video.fps_den = 1;
    video.graphics_module = argv[1];
    video.output_format = VIDEO_FORMAT_RGBA;
    video.colorspace = VIDEO_CS_709;
    video.range = VIDEO_RANGE_FULL;
    video.scale_type = OBS_SCALE_BILINEAR;
    if (obs_reset_video(&video) != OBS_VIDEO_SUCCESS)
        return 4;
    if (!load_module(argv[3], argv[4]) || !load_module(argv[5], argv[6]))
        return 5;
    HMODULE filter_library = LoadLibraryA(argv[5]);
    auto render_count = reinterpret_cast<RenderCountFn>(
        GetProcAddress(filter_library, "motion_frosted_glass_render_count"));
    auto gaussian_pass_count = reinterpret_cast<GaussianPassCountFn>(
        GetProcAddress(filter_library, "motion_frosted_gaussian_pass_count"));
    if (!render_count || !gaussian_pass_count) {
        std::cerr << "Missing render instrumentation export\n";
        return 9;
    }

    obs_data_t *image_settings = obs_data_create();
    obs_data_set_string(image_settings, "file", argv[7]);
    obs_source_t *image = obs_source_create("image_source", "render-test-image", image_settings, nullptr);
    obs_data_release(image_settings);
    obs_source_t *filter = obs_source_create("shader_filter", "render-test-filter", nullptr, nullptr);
    if (!image || !filter)
        return 6;
    obs_source_filter_add(image, filter);
    obs_source_set_enabled(filter, true);
    obs_scene_t *scene = obs_scene_create("render-test-scene");
    obs_scene_add(scene, image);
    obs_source_t *scene_source = obs_scene_get_source(scene);
    obs_set_output_source(0, scene_source);
    obs_source_inc_showing(image);
    obs_source_inc_active(image);
    for (int attempt = 0; attempt < 100 &&
                          (!obs_source_get_width(image) || !obs_source_get_height(image)); ++attempt)
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    std::cout << "image_dimensions=" << obs_source_get_width(image) << 'x' << obs_source_get_height(image)
              << " filter_enabled=" << obs_source_enabled(filter)
              << " filter_count=" << obs_source_filter_count(image)
              << " parent_match=" << (obs_filter_get_parent(filter) == image)
              << " target_match=" << (obs_filter_get_target(filter) == image) << '\n';
    const auto initial_render_count = render_count();
    const auto initial_gaussian_pass_count = gaussian_pass_count();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    const wchar_t *window_class = L"MotionFrostedGlassRenderTest";
    WNDCLASSW wc = {};
    wc.lpfnWndProc = DefWindowProcW;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = window_class;
    RegisterClassW(&wc);
    HWND window = CreateWindowW(window_class, L"render-test", WS_OVERLAPPEDWINDOW,
                                0, 0, 640, 360, nullptr, nullptr, wc.hInstance, nullptr);
    gs_init_data display_info = {};
    display_info.cx = 640;
    display_info.cy = 360;
    display_info.format = GS_RGBA;
    display_info.zsformat = GS_ZS_NONE;
    display_info.window.hwnd = window;
    obs_display_t *display = obs_display_create(&display_info, 0);
    if (!display)
        return 7;
    obs_display_add_draw_callback(display, render_display, nullptr);
    for (int step = 0; step < 20; ++step) {
        obs_data_t *live = obs_data_create();
        obs_data_set_int(live, "region_mode", step % 2);
        obs_data_set_double(live, "region_x", (step * 3) % 70);
        obs_data_set_double(live, "region_y", (step * 5) % 70);
        obs_data_set_double(live, "region_width", 30 + (step * 7) % 70);
        obs_data_set_double(live, "region_height", 30 + (step * 11) % 70);
        obs_data_set_double(live, "blur_strength", step == 19 ? 64 : (step * 3) % 65);
        obs_data_set_int(live, "blur_quality", step % 3);
        obs_source_update(filter, live);
        obs_data_release(live);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    obs_display_destroy(display);
    DestroyWindow(window);

    obs_source_filter_remove(image, filter);
    obs_source_dec_active(image);
    obs_source_dec_showing(image);
    obs_set_output_source(0, nullptr);
    obs_scene_release(scene);
    obs_source_release(filter);
    obs_source_release(image);
    obs_shutdown();
    const auto callbacks = render_count() - initial_render_count;
    const auto gaussian_passes = gaussian_pass_count() - initial_gaussian_pass_count;
    FreeLibrary(filter_library);
    std::cout << "Rendered image-filter display; callbacks=" << callbacks << " gaussian_passes=" << gaussian_passes
              << " render_errors=" << render_errors.load() << '\n';
    return callbacks >= 300 && gaussian_passes >= callbacks * 4 && render_errors.load() == 0 ? 0 : 8;
}
