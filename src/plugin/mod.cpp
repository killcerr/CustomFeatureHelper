#include "plugin/mod.h"

#include <memory>

#include "ll/api/mod/NativeMod.h"
#include "ll/api/mod/RegisterHelper.h"

namespace custom_feature_helper {

static std::unique_ptr<CustomFeatureHelper> instance;

CustomFeatureHelper& CustomFeatureHelper::getInstance() { return *instance; }

bool CustomFeatureHelper::load() {
    getSelf().getLogger().info("Loading...");
    // Code for loading the plugin goes here.
    return true;
}

bool CustomFeatureHelper::enable() {
    getSelf().getLogger().info("Enabling...");
    // Code for enabling the plugin goes here.
    return true;
}

bool CustomFeatureHelper::disable() {
    getSelf().getLogger().info("Disabling...");
    // Code for disabling the plugin goes here.
    return true;
}

} // namespace custom_feature_helper

LL_REGISTER_MOD(custom_feature_helper::CustomFeatureHelper, custom_feature_helper::instance);
