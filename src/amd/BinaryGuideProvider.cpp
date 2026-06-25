#include "amd/BinaryGuideProvider.h"

#include "amd/GuideLoader.h"

namespace amd {

bool BinaryGuideProvider::load(const std::string& filename, float defaultRadius) {
    clear();
    m_defaultRadius = std::max(defaultRadius, 1e-4f);

    GuideLoader loader;
    if (!loader.load(filename))
        return false;

    m_guides = loader.guides();
    return true;
}

void BinaryGuideProvider::clear() {
    m_guides.clear();
}

float BinaryGuideProvider::priorityScale(uint16_t priority) {
    return std::max(0.1f, priority / 50.f);
}

float BinaryGuideProvider::falloffWeight(
    float distance, float radius, float strength, uint16_t priority)
{
    if (radius <= 0.f)
        return 0.f;
    const float ratio = distance / radius;
    const float raw = strength * std::max(0.f, 1.f - ratio);
    return raw * priorityScale(priority);
}

float BinaryGuideProvider::weight(const Eigen::Vector3f& position) const {
    float best = 0.f;
    for (const auto& guide : m_guides) {
        const float radius = guide.radius > 0.f ? guide.radius : m_defaultRadius;
        const float distance = (guide.position - position).norm();
        const float w = falloffWeight(distance, radius, guide.strength, guide.priority);
        if (w > best)
            best = w;
    }
    return std::min(1.f, best);
}

Eigen::Vector3f BinaryGuideProvider::direction(const Eigen::Vector3f& position) const {
    float bestWeight = 0.f;
    Eigen::Vector3f bestDirection = Eigen::Vector3f::Zero();
    for (const auto& guide : m_guides) {
        const float radius = guide.radius > 0.f ? guide.radius : m_defaultRadius;
        const float distance = (guide.position - position).norm();
        const float w = falloffWeight(distance, radius, guide.strength, guide.priority);
        if (w > bestWeight) {
            bestWeight = w;
            bestDirection = guide.direction;
        }
    }
    return bestDirection;
}

}  // namespace amd
