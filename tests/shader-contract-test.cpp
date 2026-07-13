#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {
std::string read_file(const std::filesystem::path &path)
{
    std::ifstream input(path);
    std::ostringstream contents;
    contents << input.rdbuf();
    return contents.str();
}

void require_contains(const std::string &text, const std::string &token, const char *file)
{
    if (text.find(token) == std::string::npos) {
        std::cerr << "FAIL: " << file << " is missing " << token << '\n';
        std::exit(1);
    }
}

void require_absent(const std::string &text, const std::string &token, const char *file)
{
    if (text.find(token) != std::string::npos) {
        std::cerr << "FAIL: " << file << " still contains obsolete " << token << '\n';
        std::exit(1);
    }
}
}

int main()
{
    const auto blur = read_file(std::filesystem::path(EFFECT_DIR) / "blur.effect");
    const auto glass = read_file(std::filesystem::path(EFFECT_DIR) / "frosted-glass.effect");
    const auto output = read_file(std::filesystem::path(EFFECT_DIR) / "output.effect");
    for (const auto &token : {"image", "texel_step", "source_rect", "center_weight", "pair_weights", "pair_offsets", "Draw"})
        require_contains(blur, token, "blur.effect");
    for (const auto &token : {"exp(", "offset = -4", "offset <= 4", "sigma"})
        require_absent(blur, token, "blur.effect");
    for (const auto &token : {"image", "blurred_image", "uv_rect", "blur_rect", "Draw"})
        require_contains(glass, token, "frosted-glass.effect");
    for (const auto &token : {"corner_radius", "glass_opacity", "time_seconds", "animation_style",
                              "animation_speed", "primary_color", "secondary_color", "border_width",
                              "border_brightness", "grain_strength", "source_size"})
        require_absent(glass, token, "frosted-glass.effect");
    for (const auto &token : {"image", "output_image", "Draw"})
        require_contains(output, token, "output.effect");
    std::cout << "All shader contract tests passed\n";
    return 0;
}
