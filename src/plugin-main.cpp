#include <obs-module.h>
#include "frosted-glass-filter.hpp"
#include "obs-version-policy.hpp"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("motion-frosted-glass", "en-US")

MODULE_EXPORT const char *obs_module_description(void)
{
    return "Optimized privacy-grade Gaussian blur video filter for Streamlabs Desktop";
}

bool obs_module_load(void)
{
    const uint32_t runtime_version = obs_get_version();
    if (!motion_frosted_obs_version_supported(runtime_version)) {
        blog(LOG_ERROR,
             "[Motion Frosted Glass] unsupported OBS Core API %s; this build requires 31.1.x",
             obs_get_version_string());
        return false;
    }

    obs_register_source(motion_frosted_glass_source_info());
    blog(LOG_INFO, "[Motion Frosted Glass] module loaded on OBS Core API %s", obs_get_version_string());
    return true;
}
