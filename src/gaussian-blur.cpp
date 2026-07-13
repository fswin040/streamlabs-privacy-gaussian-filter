#include "gaussian-blur.hpp"

#include <algorithm>
#include <cmath>

int gaussian_iteration_count(BlurQuality quality)
{
    switch (quality) {
    case BlurQuality::Low:
        return 2;
    case BlurQuality::High:
        return 6;
    case BlurQuality::Medium:
    default:
        return 4;
    }
}

int gaussian_downsample_divisor(float strength)
{
    const float safe_strength = std::clamp(strength, 0.0f, 64.0f);
    if (safe_strength == 0.0f)
        return 1;
    if (safe_strength <= 16.0f)
        return 2;
    if (safe_strength <= 40.0f)
        return 4;
    return 8;
}

float gaussian_pass_sigma(float strength, int iterations)
{
    const float safe_strength = std::clamp(strength, 0.0f, 64.0f);
    if (safe_strength == 0.0f)
        return 0.0f;
    (void)iterations;
    const float sigma = 0.55f + safe_strength / 20.0f;
    return std::clamp(sigma, 0.55f, 2.75f);
}

bool gaussian_should_bypass(float strength)
{
    return !std::isfinite(strength) || strength <= 0.0f;
}

GaussianLinearSamples gaussian_linear_samples(float sigma)
{
    const float safe_sigma = std::clamp(sigma, 0.55f, 2.75f);
    float weights[5] = {};
    float total = 0.0f;
    for (int offset = 0; offset <= 4; ++offset) {
        const float distance = static_cast<float>(offset);
        weights[offset] = std::exp(-(distance * distance) / (2.0f * safe_sigma * safe_sigma));
        total += weights[offset] * (offset == 0 ? 1.0f : 2.0f);
    }
    for (float &weight : weights)
        weight /= total;

    GaussianLinearSamples result = {};
    result.center_weight = weights[0];
    for (int pair = 0; pair < 2; ++pair) {
        const int first = 1 + pair * 2;
        const int second = first + 1;
        const float combined = weights[first] + weights[second];
        result.pair_weights[pair] = combined;
        result.pair_offsets[pair] = (static_cast<float>(first) * weights[first] +
                                     static_cast<float>(second) * weights[second]) /
                                    combined;
    }
    return result;
}

NormalizedRect gaussian_padded_region(NormalizedRect region, uint32_t source_width, uint32_t source_height,
                                      float strength, BlurQuality quality)
{
    if (source_width == 0 || source_height == 0)
        return {0.0f, 0.0f, 1.0f, 1.0f};
    const float support = 4.0f * static_cast<float>(gaussian_iteration_count(quality)) *
                          static_cast<float>(gaussian_downsample_divisor(strength)) + 2.0f;
    const float pad_x = support / static_cast<float>(source_width);
    const float pad_y = support / static_cast<float>(source_height);
    const float left = std::clamp(region.x - pad_x, 0.0f, 1.0f);
    const float top = std::clamp(region.y - pad_y, 0.0f, 1.0f);
    const float right = std::clamp(region.x + region.width + pad_x, 0.0f, 1.0f);
    const float bottom = std::clamp(region.y + region.height + pad_y, 0.0f, 1.0f);
    return {left, top, right - left, bottom - top};
}
