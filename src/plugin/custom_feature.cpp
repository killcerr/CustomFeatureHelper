#include "plugin/custom_feature.h"
#include "ll/api/memory/Hook.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/biome/components/BiomeDecorationFeature.h"
#include "mc/world/level/levelgen/feature/IFeature.h"
#include "mc/world/level/levelgen/feature/registry/FeatureRegistry.h"
#include "mc/world/level/levelgen/feature/registry/VanillaFeatures.h"

#include <vector>

std::vector<custom_feature_helper::details::Wappper> wappers;

IFeature::IFeature()                      = default;
IFeature::IFeature(class IFeature const&) = default;

thread_local BlockSource* bs;

struct CustomFeatureBase : IFeature {
    custom_feature_helper::details::Wappper* mCustomFeature;
    virtual std::optional<class BlockPos>
    place(class IBlockWorldGenAPI&, class BlockPos const& pos, class Random& random, class RenderParams& renderParams)
        const override {
        return mCustomFeature->mPlace(mCustomFeature->self, *bs, pos, random, renderParams);
    }
    CustomFeatureBase(custom_feature_helper::details::Wappper* customFeature) : mCustomFeature(customFeature) {}
};

LL_AUTO_STATIC_HOOK(
    DecorateHook,
    HookPriority::Normal,
    "?decorate@BiomeDecorationSystem@@YAXAEAVLevelChunk@@AEAVBlockSource@@AEAVRandom@@AEAV?$vector@PEBVBiome@@V?$"
    "allocator@PEBVBiome@@@std@@@std@@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@6@"
    "AEBVIPreliminarySurfaceProvider@@@Z",
    void,
    class LevelChunk&                              lc,
    class BlockSource&                             source,
    class Random&                                  random,
    gsl::span<struct BiomeDecorationFeature const> featureList,
    std::string const&                             pass,
    void*                                          p
) {
    bs = &source;
    // printf("%s:%p\n", pass.c_str(), featureList.data());
    // std::vector<BiomeDecorationFeature> features;
    // std::for_each(featureList.begin(), featureList.end(), [&](auto& f) { features.push_back(f); });
    origin(lc, source, random, featureList, pass, p);
}
namespace custom_feature_helper::details {
void register_feature(const Wappper& wapper) { wappers.emplace_back(wapper); }
} // namespace custom_feature_helper::details
LL_AUTO_STATIC_HOOK(
    FeatureRegisterHook,
    HookPriority::Normal,
    &VanillaFeatures::registerFeatures,
    void,
    class FeatureRegistry&       registry,
    class BaseGameVersion const& baseGameVersion,
    class Experiments const&     experiments
) {
    for (auto& wapper : wappers) {
        registry.registerFeature<CustomFeatureBase>(wapper.name, CustomFeatureBase{&wapper});
    }
    origin(registry, baseGameVersion, experiments);
}