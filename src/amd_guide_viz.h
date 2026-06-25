#pragma once

#include "glutil.h"
#include <string>

/// Visualize AI Mesh Doctor .guides.bin sidecars in the Instant Meshes viewport.
class AmdGuideViz {
public:
    void clear();
    bool loadFromFile(const std::string& path, Float edgeLength, SerializableGLShader& shader);
    void draw(SerializableGLShader& shader, const Eigen::Matrix4f& mvp) const;

    bool empty() const { return m_triangleCount == 0; }
    std::size_t constraintCount() const { return m_constraintCount; }

private:
    uint32_t m_triangleCount = 0;
    std::size_t m_constraintCount = 0;
};
