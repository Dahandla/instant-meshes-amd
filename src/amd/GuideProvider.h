#pragma once

#include <Eigen/Core>
#include <cmath>

namespace amd {

/// Extension point — solver queries guide influence without knowing the source.
class GuideProvider {
public:
    virtual ~GuideProvider() = default;

    virtual bool enabled() const = 0;

    virtual float weight(const Eigen::Vector3f& position) const = 0;

    virtual Eigen::Vector3f direction(const Eigen::Vector3f& position) const = 0;
};

class NullGuideProvider : public GuideProvider {
public:
    bool enabled() const override { return false; }
    float weight(const Eigen::Vector3f&) const override { return 0.f; }
    Eigen::Vector3f direction(const Eigen::Vector3f&) const override {
        return Eigen::Vector3f::Zero();
    }
};

/// Optional soft energy term (Strategy B); CQ/CQw injection uses weight() directly.
inline float guideEnergy(
    const GuideProvider& guides,
    const Eigen::Vector3f& position,
    const Eigen::Vector3f& currentDirection)
{
    if (!guides.enabled())
        return 0.f;

    const Eigen::Vector3f desired = guides.direction(position);
    if (desired.squaredNorm() < 1e-12f)
        return 0.f;

    const float w = guides.weight(position);
    const float aligned = std::abs(currentDirection.normalized().dot(desired.normalized()));
    return w * (1.f - aligned);
}

}  // namespace amd
