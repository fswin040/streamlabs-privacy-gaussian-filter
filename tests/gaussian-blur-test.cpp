#include "filter-settings.hpp"
#include "gaussian-blur.hpp"

#include <cmath>
#include <cstdlib>
#include <iostream>

static void require(bool condition, const char *message)
{
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(1);
    }
}

int main()
{
    require(gaussian_should_bypass(0.0f), "zero strength bypasses all blur passes");
    require(gaussian_should_bypass(-1.0f), "normalized negative strength bypasses blur passes");
    require(!gaussian_should_bypass(0.5f), "positive strength renders blur passes");
    require(gaussian_downsample_divisor(0.0f) == 1, "zero strength stays full resolution");
    require(gaussian_downsample_divisor(1.0f) == 2, "light blur uses half resolution");
    require(gaussian_downsample_divisor(16.0f) == 2, "half-resolution upper threshold");
    require(gaussian_downsample_divisor(17.0f) == 4, "medium blur uses quarter resolution");
    require(gaussian_downsample_divisor(40.0f) == 4, "quarter-resolution upper threshold");
    require(gaussian_downsample_divisor(41.0f) == 8, "privacy blur uses eighth resolution");
    require(gaussian_downsample_divisor(64.0f) == 8, "maximum remains eighth resolution");
    require(gaussian_iteration_count(BlurQuality::Low) == 2, "low quality uses 2 iterations");
    require(gaussian_iteration_count(BlurQuality::Medium) == 4, "medium quality uses 4 iterations");
    require(gaussian_iteration_count(BlurQuality::High) == 6, "high quality uses 6 iterations");

    for (int iterations : {2, 4, 6}) {
        float previous = 0.0f;
        for (float strength : {0.0f, 8.0f, 24.0f, 64.0f}) {
            const float sigma = gaussian_pass_sigma(strength, iterations);
            if (strength == 0.0f)
                require(sigma == 0.0f, "zero strength disables Gaussian blur");
            else
                require(sigma >= 0.55f && sigma <= 2.75f, "sigma stays inside dense-kernel range");
            require(sigma >= previous, "sigma grows monotonically with strength");
            previous = sigma;
        }
    }
    require(gaussian_pass_sigma(32.0f, 2) == gaussian_pass_sigma(32.0f, 6),
            "quality adds convolution passes without weakening sigma");
    const float maximum_effective_sigma = gaussian_pass_sigma(64.0f, 6) * std::sqrt(6.0f) *
                                          static_cast<float>(gaussian_downsample_divisor(64.0f));
    require(maximum_effective_sigma >= 50.0f, "maximum high quality reaches privacy-grade effective radius");

    for (float sigma : {0.55f, 1.5f, 2.75f}) {
        const GaussianLinearSamples samples = gaussian_linear_samples(sigma);
        const float total = samples.center_weight +
                            2.0f * (samples.pair_weights[0] + samples.pair_weights[1]);
        require(std::abs(total - 1.0f) < 0.0001f, "paired Gaussian weights normalize to one");
        require(samples.pair_offsets[0] > 1.0f && samples.pair_offsets[0] < 2.0f,
                "first bilinear pair combines offsets one and two");
        require(samples.pair_offsets[1] > 3.0f && samples.pair_offsets[1] < 4.0f,
                "second bilinear pair combines offsets three and four");
    }

    const NormalizedRect roi = gaussian_padded_region({0.4f, 0.4f, 0.1f, 0.1f}, 1920, 1080, 40.0f,
                                                       BlurQuality::High);
    require(roi.x < 0.4f && roi.y < 0.4f, "ROI adds padding before the custom region");
    require(roi.x + roi.width > 0.5f && roi.y + roi.height > 0.5f,
            "ROI adds padding after the custom region");
    require(roi.width < 1.0f && roi.height < 1.0f, "small custom region remains smaller than full source");
    const NormalizedRect edge_roi = gaussian_padded_region({0.0f, 0.0f, 0.1f, 0.1f}, 1920, 1080, 64.0f,
                                                            BlurQuality::High);
    require(edge_roi.x == 0.0f && edge_roi.y == 0.0f, "ROI clamps padding at source origin");
    require(edge_roi.x + edge_roi.width <= 1.0f && edge_roi.y + edge_roi.height <= 1.0f,
            "ROI remains inside source bounds");
    std::cout << "All Gaussian blur tests passed\n";
    return 0;
}
