#pragma once

#include "amd/GuideProvider.h"
#include "hierarchy.h"

namespace amd {

/// Replace all orientation constraints with guide field samples.
void applyGuidesToHierarchy(
    MultiResolutionHierarchy& mRes,
    const GuideProvider& provider,
    int rosy,
    int posy);

/// Fill guide constraints on vertices without an existing stroke/boundary lock.
void mergeGuidesToHierarchy(
    MultiResolutionHierarchy& mRes,
    const GuideProvider& provider,
    int rosy,
    int posy);

}  // namespace amd
