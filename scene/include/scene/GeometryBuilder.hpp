#pragma once

#include "scene_fwd.hpp"
#include <string>
#include <vector>

namespace scene {

class GeometryBuilder
{
public:

    GeometryBuilder();

    ~GeometryBuilder();

    const std::vector<Mesh>& getAllMeshes() const;

    const Mesh& getMeshById(MeshId id) const;

private:

    bool readFile(std::string filepath);

    std::vector<Mesh> meshes_;

};

} // end namespace scene
