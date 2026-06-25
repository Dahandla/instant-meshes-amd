#include "amd/apply_guides.h"

#include <iostream>

namespace amd {

namespace {

uint32_t applyGuideSamples(
    MultiResolutionHierarchy& mRes,
    const GuideProvider& provider,
    bool merge_with_existing)
{
    if (!provider.enabled())
        return 0;

    const MatrixXf& V = mRes.V();
    const MatrixXf& N = mRes.N();
    const uint32_t n = mRes.size();
    uint32_t constrained = 0;

    for (uint32_t v = 0; v < n; ++v) {
        const float existing = mRes.CQw()[v];
        if (merge_with_existing && existing >= 1.f - 1e-6f)
            continue;

        const Vector3f p = V.col(v);
        const float w = provider.weight(p);
        if (w <= 1e-6f)
            continue;
        if (merge_with_existing && existing > w)
            continue;

        Vector3f dir = provider.direction(p);
        const Vector3f n_v = N.col(v);
        dir -= n_v * n_v.dot(dir);
        if (dir.squaredNorm() < 1e-12f)
            continue;
        dir.normalize();

        mRes.CQ().col(v) = dir;
        mRes.CQw()[v] = std::min(1.f, w);
        constrained++;
    }

    return constrained;
}

}  // namespace

void applyGuidesToHierarchy(
    MultiResolutionHierarchy& mRes,
    const GuideProvider& provider,
    int rosy,
    int posy)
{
    mRes.clearConstraints();
    const uint32_t constrained = applyGuideSamples(mRes, provider, false);
    std::cout << "[AMD] Applied guide constraints to " << constrained
              << " / " << mRes.size() << " vertices" << std::endl;
    mRes.propagateConstraints(rosy, posy);
}

void mergeGuidesToHierarchy(
    MultiResolutionHierarchy& mRes,
    const GuideProvider& provider,
    int rosy,
    int posy)
{
    const uint32_t constrained = applyGuideSamples(mRes, provider, true);
    if (constrained > 0) {
        std::cout << "[AMD] Merged guide constraints on " << constrained
                  << " vertices" << std::endl;
    }
    mRes.propagateConstraints(rosy, posy);
}

}  // namespace amd
