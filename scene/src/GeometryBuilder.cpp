#include <scene/scene.hpp>
#include <tcf/tcf.hpp>
#include <tcf/SimpleScene.hpp>

using namespace scene;

/******************************************************************************
*
*
* STOP!
*
* You shouldn't be reading this source file.
* You don't need to know about the implementation of the GeometryBuilder.
* Do not base any of your code on how this is implemented,
* to do so would break the concept of encapsulation.
*
*
*****************************************************************************/

GeometryBuilder::GeometryBuilder()
{
    if (!readFile("sponza_with_friends_2x.tcf")) {
        throw std::runtime_error("Failed to read sponza.tcf data file");
    }
}

GeometryBuilder::~GeometryBuilder()
{

}

const std::vector<Mesh>& GeometryBuilder::getAllMeshes() const
{
    return meshes_;
}

const Mesh& GeometryBuilder::getMeshById(MeshId id) const
{
    return meshes_[id - 300];
}

bool GeometryBuilder::readFile(std::string filepath)
{
    tcf::Reader * reader = tcf::createReader();
    tcf::SimpleScene * tcf_scene = nullptr;

    try {
        reader->openFile(filepath.c_str());
        reader->skipChunk(); // don't care about HEAD
        if (reader->hasChunk()) {
            reader->openChunk();
            if (chunkIsSimpleScene(reader)) {
                tcf_scene = readSimpleScene(reader);
            }
        }
        reader->closeFile();
    }
    catch (...) {
        if (reader) reader->release();
        if (tcf_scene) tcf_scene->release();
        return false;
    }

    meshes_.clear();

    meshes_.reserve(tcf_scene->meshCount());
    for (unsigned int i = 0; i < tcf_scene->meshCount(); ++i) {
        const auto * mesh = tcf_scene->findMeshByIndex(i);
        Mesh new_mesh(300 + meshes_.size());
        if (mesh->indexArray() != nullptr) {
            new_mesh.assignElementArray(std::vector<unsigned int>(
                mesh->indexArray(),
                mesh->indexArray() + mesh->indexCount()));
        }
        if (mesh->positionArray() != nullptr) {
            new_mesh.assignPositionArray(std::vector<Vector3>(
                (const Vector3 *)mesh->positionArray(),
                (const Vector3 *)mesh->positionArray() + mesh->vertexCount()));
        }
        if (mesh->normalArray() != nullptr) {
            new_mesh.assignNormalArray(std::vector<Vector3>(
                (const Vector3 *)mesh->normalArray(),
                (const Vector3 *)mesh->normalArray() + mesh->vertexCount()));
        }
        if (mesh->tangentArray() != nullptr) {
            new_mesh.assignTangentArray(std::vector<Vector3>(
                (const Vector3 *)mesh->tangentArray(),
                (const Vector3 *)mesh->tangentArray() + mesh->vertexCount()));
        }
        if (mesh->uvArray() != nullptr) {
            new_mesh.assignTextureCoordinateArray(std::vector<Vector2>(
                (const Vector2 *)mesh->uvArray(),
                (const Vector2 *)mesh->uvArray() + mesh->vertexCount()));
        }
        meshes_.push_back(new_mesh);
    }

    reader->release();
    tcf_scene->release();

    return true;
}
