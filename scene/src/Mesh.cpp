#include <scene/scene.hpp>

using namespace scene;

Mesh::Mesh(MeshId i) : id(i)
{
}

MeshId Mesh::getId() const
{
    return id;
}

const std::vector<Vector3>& Mesh::getPositionArray() const
{
    return position_array;
}

void Mesh::assignPositionArray(std::vector<Vector3>&& p)
{
    position_array = p;
}

const std::vector<Vector3>& Mesh::getNormalArray() const
{
    return normal_array;
}

void Mesh::assignNormalArray(std::vector<Vector3>&& n)
{
    normal_array = n;
}

const std::vector<Vector3>& Mesh::getTangentArray() const
{
    return tangent_array;
}

void Mesh::assignTangentArray(std::vector<Vector3>&& t)
{
    tangent_array = t;
}

const std::vector<Vector2>& Mesh::getTextureCoordinateArray() const
{
    return texcoord_array;
}

void Mesh::assignTextureCoordinateArray(std::vector<Vector2>&& t)
{
    texcoord_array = t;
}

const std::vector<unsigned int> Mesh::getElementArray() const
{
    return element_array;
}

void Mesh::assignElementArray(std::vector<unsigned int>&& e)
{
    element_array = e;
}
