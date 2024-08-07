#include "plugin/custom_feature.h"
#include "ll/api/memory/Hook.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/Level.h"
#include "mc/world/level/biome/components/BiomeDecorationFeature.h"
#include "mc/world/level/levelgen/feature/IFeature.h"
#include "mc/world/level/levelgen/feature/registry/FeatureRegistry.h"
#include "mc/world/level/levelgen/feature/registry/VanillaFeatures.h"
#include "mc/world/level/storage/Experiments.h"
#include "nlohmann/json.hpp"

#include <cstdlib>
#include <print>
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
    "?decorateBiome@BiomeDecorationSystem@@YA_NAEAVLevelChunk@@AEAVBlockSource@@AEAVRandom@@V?$span@$$"
    "CBUBiomeDecorationFeature@@$0?0@gsl@@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PEBVBiome@@"
    "AEBVIPreliminarySurfaceProvider@@@Z",
    void,
    class LevelChunk&                              lc,
    class BlockSource&                             source,
    class Random&                                  random,
    gsl::span<struct BiomeDecorationFeature const> featureList,
    std::string const&                             pass,
    class Biome const*                             biome,
    class IPreliminarySurfaceProvider const&       provider
) {
    bs = &source;
    origin(lc, source, random, featureList, pass, biome, provider);
}
namespace custom_feature_helper::details {
void register_feature(const Wappper& wapper) {
    wappers.emplace_back(wapper);
    std::filesystem::path rulesPath = "./definitions/feature_rules/";
    for (const auto& rule : wapper.rules) {
        auto name =
            nlohmann::json::parse(rule, nullptr, true, true)["minecraft:feature_rules"]["description"]["identifier"]
                .get<std::string>();
        name = name.substr(name.find(':') + 1);
        // std::println("{}", name);
        std::ofstream fout(rulesPath / (name + ".json"));
        fout << rule;
        fout.close();
    }
}
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
        registry.registerFeature<CustomFeatureBase>(wapper.name, &wapper);
    }
    origin(registry, baseGameVersion, experiments);
}
auto init = [] {
    std::atexit([] {
        std::filesystem::path rulesPath = "./definitions/feature_rules/";
        for (const auto& wapper : wappers) {
            for (const auto& rule : wapper.rules) {
                auto name = nlohmann::json::parse(rule, nullptr, true, true)["minecraft:feature_rules"]["description"]
                                                                            ["identifier"]
                                                                                .get<std::string>();
                name = name.substr(name.find(':') + 1);
                std::filesystem::remove(rulesPath / (name + ".json"));
            }
        }
    });
    return true;
}();