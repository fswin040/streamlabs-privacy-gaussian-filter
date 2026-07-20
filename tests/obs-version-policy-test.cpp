#include "obs-version-policy.hpp"

#include <cstdint>
#include <iostream>

static constexpr uint32_t make_version(uint32_t major, uint32_t minor, uint32_t patch)
{
    return (major << 24U) | (minor << 16U) | patch;
}

int main()
{
    const uint32_t accepted[] = {
        make_version(31, 1, 0),
        make_version(31, 1, 2),
        make_version(31, 1, 3),
        make_version(31, 1, 99),
    };
    for (const uint32_t version : accepted) {
        if (!motion_frosted_obs_version_supported(version)) {
            std::cerr << "Expected OBS 31.1.x version to be supported\n";
            return 1;
        }
    }

    const uint32_t rejected[] = {
        make_version(31, 0, 99),
        make_version(31, 2, 0),
        make_version(32, 0, 0),
        make_version(0, 0, 0),
    };
    for (const uint32_t version : rejected) {
        if (motion_frosted_obs_version_supported(version)) {
            std::cerr << "Expected non-31.1.x version to be rejected\n";
            return 1;
        }
    }

    if (!motion_frosted_obs_runtime_supported(make_version(31, 1, 3), "31.1.2sl19b3")) {
        std::cerr << "Expected Streamlabs OBS build 31.1.2sl19b3 to be supported\n";
        return 1;
    }

    const char *rejected_builds[] = {
        "31.1.2ndi1",
        "31.1.2sl19b2",
        "31.1.2sl22",
        "31.1.3",
        "",
        nullptr,
    };
    for (const char *build : rejected_builds) {
        if (motion_frosted_obs_runtime_supported(make_version(31, 1, 3), build)) {
            std::cerr << "Expected non-sl19b3 build to be rejected\n";
            return 1;
        }
    }
    if (motion_frosted_obs_runtime_supported(make_version(31, 2, 0), "31.1.2sl19b3")) {
        std::cerr << "Expected non-31.1 API with sl19b3 marker to be rejected\n";
        return 1;
    }

    std::cout << "All OBS version policy tests passed\n";
    return 0;
}
