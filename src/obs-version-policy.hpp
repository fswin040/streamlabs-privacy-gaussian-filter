#pragma once

#include <cstdint>

bool motion_frosted_obs_version_supported(uint32_t version) noexcept;
bool motion_frosted_obs_runtime_supported(uint32_t version, const char *build_string) noexcept;
