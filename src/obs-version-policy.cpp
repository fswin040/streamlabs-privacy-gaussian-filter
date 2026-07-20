#include "obs-version-policy.hpp"

#include <cstring>

namespace {
constexpr const char *supported_streamlabs_build = "31.1.2sl19b3";
}

bool motion_frosted_obs_version_supported(uint32_t version) noexcept
{
    const uint32_t major = version >> 24U;
    const uint32_t minor = (version >> 16U) & 0xFFU;
    return major == 31U && minor == 1U;
}

bool motion_frosted_obs_runtime_supported(uint32_t version, const char *build_string) noexcept
{
    return motion_frosted_obs_version_supported(version) && build_string != nullptr &&
           std::strcmp(build_string, supported_streamlabs_build) == 0;
}
