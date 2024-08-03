#pragma once

#include "ll/api/Mod/NativeMod.h"

namespace custom_feature_helper {

class CustomFeatureHelper {

public:
    static CustomFeatureHelper& getInstance();

    CustomFeatureHelper(ll::mod::NativeMod& self) : mSelf(self) {}

    [[nodiscard]] ll::mod::NativeMod& getSelf() const { return mSelf; }

    /// @return True if the Mod is loaded successfully.
    bool load();

    /// @return True if the Mod is enabled successfully.
    bool enable();

    /// @return True if the Mod is disabled successfully.
    bool disable();

    // TODO: Implement this method if you need to unload the Mod.
    // /// @return True if the Mod is unloaded successfully.
    // bool unload();

private:
    ll::mod::NativeMod& mSelf;
};

} // namespace custom_feature_helper
