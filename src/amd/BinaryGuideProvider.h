#pragma once

#include "amd/GuideConstraint.h"
#include "amd/GuideProvider.h"

#include <string>
#include <vector>

namespace amd {

/// Guide influence from AI Mesh Doctor .guides.bin sample points.
class BinaryGuideProvider : public GuideProvider {
public:
    bool load(const std::string& filename, float defaultRadius = 0.05f);
    void clear();

    bool enabled() const override { return !m_guides.empty(); }
    float weight(const Eigen::Vector3f& position) const override;
    Eigen::Vector3f direction(const Eigen::Vector3f& position) const override;

    std::size_t sampleCount() const { return m_guides.size(); }

private:
    static float priorityScale(uint16_t priority);
    static float falloffWeight(float distance, float radius, float strength, uint16_t priority);

    std::vector<GuideConstraint> m_guides;
    float m_defaultRadius = 0.05f;
};

}  // namespace amd
