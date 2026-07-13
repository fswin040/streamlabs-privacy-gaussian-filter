#include "obs-version-policy.hpp"

bool motion_frosted_obs_version_supported(uint32_t version) noexcept
{
    const uint32_t major = version >> 24U;
    const uint32_t minor = (version >> 16U) & 0xFFU;
    return major == 31U && minor == 1U;
}
