#include <obs-module.h>
#include "frosted-glass-filter.hpp"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("motion-frosted-glass", "en-US")

MODULE_EXPORT const char *obs_module_description(void)
{
    return "Optimized privacy-grade Gaussian blur video filter for Streamlabs Desktop";
}

bool obs_module_load(void)
{
    obs_register_source(motion_frosted_glass_source_info());
    blog(LOG_INFO, "[Motion Frosted Glass] module loaded");
    return true;
}
