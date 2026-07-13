#include "filter-settings.hpp"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>

namespace {
void require(bool condition, const char *message)
{
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(1);
    }
}
}

int main()
{
    static_assert(sizeof(FilterSettings) == 28, "pure blur settings must not contain glass or animation fields");
    const auto defaults = default_filter_settings();
    require(defaults.region_mode == RegionMode::FullSource, "default region is full source");
    require(defaults.blur_quality == BlurQuality::Medium, "default quality is medium");
    require(defaults.region.x == 0.0f && defaults.region.y == 0.0f, "default region origin");
    require(defaults.region.width == 1.0f && defaults.region.height == 1.0f, "default region size");

    auto invalid = defaults;
    invalid.region_mode = static_cast<RegionMode>(99);
    invalid.blur_quality = static_cast<BlurQuality>(99);
    invalid.region = {-0.5f, 0.95f, 2.0f, 0.0f};
    invalid.blur_strength = std::numeric_limits<float>::infinity();
    const auto normalized = normalize_filter_settings(invalid);

    require(normalized.region_mode == RegionMode::FullSource, "invalid region mode resets");
    require(normalized.blur_quality == BlurQuality::Medium, "invalid quality resets");
    require(normalized.region.x == 0.0f, "negative x clamps");
    require(normalized.region.y == 0.95f, "valid y remains");
    require(normalized.region.width == 1.0f, "oversized width clamps to edge");
    require(std::abs(normalized.region.height - 0.01f) < 0.0001f, "zero height becomes one percent");
    require(normalized.blur_strength == defaults.blur_strength, "non-finite strength resets");

    std::cout << "All filter settings tests passed\n";
    return 0;
}
