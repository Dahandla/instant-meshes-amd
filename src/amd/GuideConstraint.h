#pragma once

#include <cstdint>
#include <Eigen/Core>

namespace amd {

enum class GuideType : uint8_t {
    Flow = 0,
    Loop = 1,
    Seam = 2,
    HardSurface = 3,
    Face = 4,
    Animation = 5,
    Mechanical = 6,
};

struct GuideConstraint {
    Eigen::Vector3f position{0.f, 0.f, 0.f};
    Eigen::Vector3f direction{0.f, 0.f, 0.f};
    float strength{0.f};
    float radius{0.f};
    uint16_t priority{50};
    GuideType type{GuideType::Flow};
};

}  // namespace amd
