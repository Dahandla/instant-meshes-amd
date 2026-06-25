#include "amd/GuideLoader.h"

#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>

namespace amd {

namespace {

constexpr std::array<char, 4> kMagic{{'A', 'M', 'D', 'G'}};
constexpr std::uint32_t kVersion = 1;

bool readU32(std::ifstream& in, std::uint32_t& value) {
    in.read(reinterpret_cast<char*>(&value), sizeof(value));
    return static_cast<bool>(in);
}

}  // namespace

bool GuideLoader::load(const std::string& filename) {
    clear();

    std::ifstream in(filename, std::ios::binary);
    if (!in)
        return false;

    std::array<char, 4> magic{};
    in.read(magic.data(), magic.size());
    if (!in || magic != kMagic)
        return false;

    std::uint32_t version = 0;
    std::uint32_t count = 0;
    if (!readU32(in, version) || version != kVersion)
        return false;
    if (!readU32(in, count))
        return false;

    m_guides.reserve(count);
    for (std::uint32_t i = 0; i < count; ++i) {
        GuideConstraint g;
        float pos[3];
        float dir[3];
        float strength = 0.f;
        float radius = 0.f;
        std::uint16_t priority = 50;
        std::uint8_t type = 0;
        std::uint8_t padding[3];

        in.read(reinterpret_cast<char*>(pos), sizeof(pos));
        in.read(reinterpret_cast<char*>(dir), sizeof(dir));
        in.read(reinterpret_cast<char*>(&strength), sizeof(strength));
        in.read(reinterpret_cast<char*>(&radius), sizeof(radius));
        in.read(reinterpret_cast<char*>(&priority), sizeof(priority));
        in.read(reinterpret_cast<char*>(&type), sizeof(type));
        in.read(reinterpret_cast<char*>(padding), sizeof(padding));
        if (!in)
            return false;

        g.position = Eigen::Vector3f(pos[0], pos[1], pos[2]);
        g.direction = Eigen::Vector3f(dir[0], dir[1], dir[2]);
        g.strength = strength;
        g.radius = radius;
        g.priority = priority;
        g.type = static_cast<GuideType>(type);
        m_guides.push_back(g);
    }

    std::cout << "[AMD] Loaded " << m_guides.size()
              << " guide constraints (avg strength "
              << averageStrength() << ")" << std::endl;
    return true;
}

float GuideLoader::averageStrength() const {
    if (m_guides.empty())
        return 0.f;
    float sum = 0.f;
    for (const auto& g : m_guides)
        sum += g.strength;
    return sum / static_cast<float>(m_guides.size());
}

void GuideLoader::clear() {
    m_guides.clear();
}

}  // namespace amd
