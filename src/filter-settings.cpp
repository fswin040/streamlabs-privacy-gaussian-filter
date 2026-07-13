#include "filter-settings.hpp"

#include <algorithm>
#include <cmath>

namespace {
constexpr float kMinimumRegionSize = 0.01f;

float finite_or(float value, float fallback)
{
    return std::isfinite(value) ? value : fallback;
}

float clamp(float value, float minimum, float maximum)
{
    return std::clamp(value, minimum, maximum);
}

}

FilterSettings default_filter_settings()
{
    return {
        RegionMode::FullSource,
        {0.0f, 0.0f, 1.0f, 1.0f},
        18.0f,
        BlurQuality::Medium,
    };
}

FilterSettings normalize_filter_settings(FilterSettings value)
{
    const FilterSettings defaults = default_filter_settings();

    if (value.region_mode != RegionMode::FullSource && value.region_mode != RegionMode::Custom)
        value.region_mode = defaults.region_mode;
    if (value.blur_quality != BlurQuality::Low && value.blur_quality != BlurQuality::Medium &&
        value.blur_quality != BlurQuality::High)
        value.blur_quality = defaults.blur_quality;

    value.region.x = clamp(finite_or(value.region.x, 0.0f), 0.0f, 1.0f - kMinimumRegionSize);
    value.region.y = clamp(finite_or(value.region.y, 0.0f), 0.0f, 1.0f - kMinimumRegionSize);
    value.region.width = clamp(finite_or(value.region.width, 1.0f), kMinimumRegionSize, 1.0f - value.region.x);
    value.region.height = clamp(finite_or(value.region.height, 1.0f), kMinimumRegionSize, 1.0f - value.region.y);
    value.blur_strength = clamp(finite_or(value.blur_strength, defaults.blur_strength), 0.0f, 64.0f);
    return value;
}
