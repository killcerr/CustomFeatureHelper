#pragma once
#include "mc/util/Random.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/levelgen/feature/helpers/RenderParams.h"


#include <optional>
namespace custom_feature_helper {
namespace details {
struct Wappper {
    std::string              name;
    std::vector<std::string> rules;
    void*                    self;
    std::optional<BlockPos> (*mPlace)(
        void*                 self,
        class BlockSource&    source,
        class BlockPos const& pos,
        class Random&         random,
        class RenderParams&   renderParams
    );
};
__declspec(dllexport) void register_feature(const Wappper&);
} // namespace details
template <typename T>
concept CustomFeature = requires(
    T                     f,
    class BlockSource&    source,
    class BlockPos const& pos,
    class Random&         random,
    class RenderParams&   renderParams
) {
    { f.place(source, pos, random, renderParams) } -> std::same_as<std::optional<BlockPos>>;
};
template <CustomFeature T>
inline void register_feature(const std::string& name, const std::vector<std::string>& rules, auto&&... args) {
    if constexpr (sizeof...(args) > 0)
        details::register_feature(
            {name,
             rules,
             new T(args...),
             [](void*                 self,
                class BlockSource&    source,
                class BlockPos const& pos,
                class Random&         random,
                class RenderParams&   renderParams) -> std::optional<BlockPos> {
                 return reinterpret_cast<T*>(self)->place(source, pos, random, renderParams);
             }}
        );
    else
        details::register_feature(
            {name,
             rules,
             new T(),
             [](void*                 self,
                class BlockSource&    source,
                class BlockPos const& pos,
                class Random&         random,
                class RenderParams&   renderParams) -> std::optional<BlockPos> {
                 return reinterpret_cast<T*>(self)->place(source, pos, random, renderParams);
             }}
        );
}
template <typename T>
concept FeaturePlaceCallBack = requires(
    T                     f,
    class BlockSource&    source,
    class BlockPos const& pos,
    class Random&         random,
    class RenderParams&   renderParams
) {
    { f(source, pos, random, renderParams) } -> std::same_as<std::optional<BlockPos>>;
};
template <FeaturePlaceCallBack T>
inline void register_feature(const std::string& name, const std::vector<std::string>& rules, T callback) {
    struct Feature {
        T                       callback;
        std::optional<BlockPos> place(
            class BlockSource&    source,
            class BlockPos const& pos,
            class Random&         random,
            class RenderParams&   renderParams
        ) {
            return callback(source, pos, random, renderParams);
        }
    };
    register_feature<Feature>(name, rules, callback);
}
} // namespace custom_feature_helper