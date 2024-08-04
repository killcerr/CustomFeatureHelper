#pragma once
#include "mc/util/Random.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/levelgen/feature/helpers/RenderParams.h"


#include <optional>
namespace custom_feature_helper {
namespace details {
struct Wappper {
    std::string name;
    void*       self;
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
inline void register_feature(std::string name, auto&&... args) {
    details::register_feature(
        {name,
         new T(std::forward(args)...),
         [](void*                 self,
            class BlockSource&    source,
            class BlockPos const& pos,
            class Random&         random,
            class RenderParams&   renderParams) -> std::optional<BlockPos> {
             return reinterpret_cast<T*>(self)->place(source, pos, random, renderParams);
         }}
    );
}
} // namespace custom_feature_helper