#pragma once

enum class RegionMode { FullSource = 0, Custom = 1 };
enum class BlurQuality { Low = 0, Medium = 1, High = 2 };

struct NormalizedRect {
    float x;
    float y;
    float width;
    float height;
};

struct FilterSettings {
    RegionMode region_mode;
    NormalizedRect region;
    float blur_strength;
    BlurQuality blur_quality;
};

FilterSettings default_filter_settings();
FilterSettings normalize_filter_settings(FilterSettings value);
