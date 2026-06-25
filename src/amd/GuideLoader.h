#pragma once

#include <string>
#include <vector>

#include "amd/GuideConstraint.h"

namespace amd {

class GuideLoader {
public:
    bool load(const std::string& filename);

    const std::vector<GuideConstraint>& guides() const { return m_guides; }
    std::size_t count() const { return m_guides.size(); }
    float averageStrength() const;

    void clear();

private:
    std::vector<GuideConstraint> m_guides;
};

}  // namespace amd
