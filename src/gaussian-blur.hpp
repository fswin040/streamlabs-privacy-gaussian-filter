#pragma once

#include "filter-settings.hpp"
#include <cstdint>

struct GaussianLinearSamples {
    float center_weight;
    float pair_weights[2];
    float pair_offsets[2];
};

int gaussian_iteration_count(BlurQuality quality);
int gaussian_downsample_divisor(float strength);
float gaussian_pass_sigma(float strength, int iterations);
bool gaussian_should_bypass(float strength);
GaussianLinearSamples gaussian_linear_samples(float sigma);
NormalizedRect gaussian_padded_region(NormalizedRect region, uint32_t source_width, uint32_t source_height,
                                      float strength, BlurQuality quality);
