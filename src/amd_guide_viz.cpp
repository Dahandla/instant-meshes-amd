#include "amd_guide_viz.h"

#include "amd/GuideLoader.h"
#include "common.h"

#include <cmath>

namespace {

Vector4u8 guideColor(amd::GuideType type) {
    Vector4u8 c;
    switch (type) {
    case amd::GuideType::Flow:
        c << 255, 80, 40, 255;
        break;
    case amd::GuideType::Loop:
        c << 80, 200, 255, 255;
        break;
    case amd::GuideType::Seam:
        c << 255, 220, 60, 255;
        break;
    default:
        c << 180, 120, 255, 255;
        break;
    }
    return c;
}

}  // namespace

void AmdGuideViz::clear() {
    m_triangleCount = 0;
    m_constraintCount = 0;
}

bool AmdGuideViz::loadFromFile(const std::string& path, Float edgeLength, SerializableGLShader& shader) {
    clear();

    amd::GuideLoader loader;
    if (!loader.load(path))
        return false;

    const auto& guides = loader.guides();
    m_constraintCount = guides.size();
    if (guides.empty())
        return true;

    const Float thickness = std::max(edgeLength * 0.12f, 1e-4f);
    const Float defaultLen = std::max(edgeLength * 2.5f, 1e-3f);

    std::vector<Vector3f> segmentStarts;
    std::vector<Vector3f> segmentEnds;
    std::vector<Vector4u8> segmentColors;
    segmentStarts.reserve(guides.size());
    segmentEnds.reserve(guides.size());
    segmentColors.reserve(guides.size());

    for (const auto& guide : guides) {
        Vector3f dir = guide.direction;
        const Float dirLen = dir.norm();
        if (dirLen < 1e-8f)
            continue;

        dir /= dirLen;
        const Float arrowLen = std::max(guide.radius, defaultLen) * std::max(guide.strength, 0.05f);
        segmentStarts.push_back(guide.position);
        segmentEnds.push_back(guide.position + dir * arrowLen);
        segmentColors.push_back(guideColor(guide.type));
    }

    const size_t nSegments = segmentStarts.size();
    if (nSegments == 0)
        return true;

    MatrixXf position(3, nSegments * 4);
    MatrixXu8 color(4, nSegments * 4);
    MatrixXu indices(3, nSegments * 2);

    for (size_t i = 0; i < nSegments; ++i) {
        const Vector3f& start = segmentStarts[i];
        const Vector3f& end = segmentEnds[i];
        Vector3f tangent = (end - start);
        if (tangent.squaredNorm() < 1e-12f)
            tangent = Vector3f::UnitX();
        else
            tangent.normalize();

        Vector3f side = tangent.cross(std::abs(tangent.dot(Vector3f::UnitY())) > 0.9f
                                          ? Vector3f::UnitX()
                                          : Vector3f::UnitY());
        if (side.squaredNorm() < 1e-12f)
            side = Vector3f::UnitZ();
        side.normalize();
        side *= thickness;

        const size_t base = i * 4;
        position.col(base + 0) = start + side;
        position.col(base + 1) = start - side;
        position.col(base + 2) = end + side;
        position.col(base + 3) = end - side;
        for (int k = 0; k < 4; ++k)
            color.col(static_cast<int>(base + k)) = segmentColors[i];

        const size_t tri = i * 2;
        indices.col(tri + 0) << static_cast<uint32_t>(base + 0), static_cast<uint32_t>(base + 1),
            static_cast<uint32_t>(base + 2);
        indices.col(tri + 1) << static_cast<uint32_t>(base + 2), static_cast<uint32_t>(base + 1),
            static_cast<uint32_t>(base + 3);
    }

    shader.bind();
    shader.uploadAttrib("position", position, 0);
    shader.uploadAttrib("color", color, 0);
    shader.uploadIndices(indices);
    m_triangleCount = static_cast<uint32_t>(nSegments * 2);
    return true;
}

void AmdGuideViz::draw(SerializableGLShader& shader, const Eigen::Matrix4f& mvp) const {
    if (m_triangleCount == 0)
        return;

    shader.bind();
    shader.setUniform("mvp", mvp);
    shader.setUniform("alpha", 0.9f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    shader.drawIndexed(GL_TRIANGLES, 0, m_triangleCount);
    glDisable(GL_BLEND);
}
