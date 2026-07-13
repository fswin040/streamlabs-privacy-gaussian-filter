#include <obs.h>

#include <iostream>
#include <string>

int main(int argc, char **argv)
{
    if (argc != 3) {
        std::cerr << "Usage: module-load-test <plugin.dll> <plugin-data-directory>\n";
        return 2;
    }
    if (!obs_startup("zh-TW", nullptr, nullptr)) {
        std::cerr << "obs_startup failed\n";
        return 3;
    }
    obs_module_t *module = nullptr;
    const int open_result = obs_open_module(&module, argv[1], argv[2]);
    std::cout << "obs_open_module=" << open_result << '\n';
    if (open_result != MODULE_SUCCESS) {
        obs_shutdown();
        return 4;
    }
    const bool initialized = obs_init_module(module);
    std::cout << "obs_init_module=" << (initialized ? "true" : "false") << '\n';
    bool found = false;
    for (size_t index = 0;; ++index) {
        const char *id = nullptr;
        if (!obs_enum_filter_types(index, &id))
            break;
        if (id && std::string(id) == "shader_filter")
            found = true;
    }
    std::cout << "filter_registered=" << (found ? "true" : "false") << '\n';
    const char *display_name = obs_source_get_display_name("shader_filter");
    std::cout << "display_name=" << (display_name ? display_name : "<null>") << '\n';
    const uint32_t flags = obs_get_source_output_flags("shader_filter");
    std::cout << "output_flags=" << flags << " video=" << ((flags & OBS_SOURCE_VIDEO) ? "true" : "false") << '\n';
    obs_shutdown();
    return initialized && found ? 0 : 5;
}
