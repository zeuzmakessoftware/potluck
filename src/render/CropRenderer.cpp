#include "render/CropRenderer.hpp"

#include "render/Palette.hpp"

#include <cmath>

namespace ultradope {

void DrawCrop(const SoilPlot& plot, Vector3 position, std::size_t plotIndex) {
    if (plot.crop == CropType::None) return;
    const int stage = VisualGrowthStage(plot);
    if (stage == 6) {
        DrawCylinder(position, 0.08F, 0.05F, 0.7F, 6, Color{103, 82, 63, 255});
        DrawSphere({position.x - 0.2F, 0.65F, position.z}, 0.22F, Color{128, 105, 72, 255});
        return;
    }

    const float phase = static_cast<float>(plotIndex % 7U) * 0.38F;
    const float height = 0.28F + static_cast<float>(stage) * 0.3F;
    const Color leaf = plot.crop == CropType::VioletComet ? Color{76, 133, 91, 255} :
        (plot.crop == CropType::Sunspun ? Color{101, 165, 74, 255} : palette::Leaf);
    DrawCylinder(position, 0.055F, 0.04F, height, 6, Color{53, 104, 55, 255});
    const int clusters = 1 + stage * 2;
    for (int i = 0; i < clusters; ++i) {
        const float angle = phase + static_cast<float>(i) * 2.399F;
        const float level = 0.25F + static_cast<float>(i % 4) * height * 0.18F;
        const float radius = 0.18F + static_cast<float>(stage) * 0.06F;
        Vector3 leafPosition{
            position.x + std::cos(angle) * radius,
            position.y + level,
            position.z + std::sin(angle) * radius
        };
        DrawSphere(leafPosition, 0.11F + static_cast<float>(stage) * 0.025F, leaf);
    }
    if (stage >= 4) {
        Color flower = plot.crop == CropType::VioletComet ? palette::Violet :
            (plot.crop == CropType::Sunspun ? palette::Gold : palette::LeafLight);
        DrawSphere({position.x, position.y + height, position.z}, 0.18F, flower);
        if (stage == 5) {
            DrawSphereWires({position.x, position.y + height, position.z}, 0.26F, 6, 6, palette::Cream);
        }
    }
}

}  // namespace ultradope
