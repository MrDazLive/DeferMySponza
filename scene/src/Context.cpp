#include <scene/scene.hpp>
#include "FirstPersonMovement.hpp"

#include <tcf/tcf.hpp>
#include <tcf/SimpleScene.hpp>

#include <random>
#include <cmath>

using namespace scene;

/******************************************************************************
*
*
* STOP!
*
* You shouldn't be reading this source file.
* You don't need to know anything about the implementation of the Context.
* Do not base any of your code on how this is implemented,
* to do so would break the concept of encapsulation.
*
* This file contains lots of poor coding practice. It's here just as a proxy
* of a real scene model that would be found in a computer game.
*
*
*****************************************************************************/

Vector3 normalize(const Vector3& v)
{
    float norm = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    return Vector3(v.x / norm, v.y / norm, v.z / norm);
}

Vector3 operator-(const Vector3& lhs, const Vector3& rhs)
{
    return Vector3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

Context::Context()
{
    start_time_ = std::chrono::system_clock::now();
    time_seconds_ = 0.f;

    if (!readFile("sponza_with_friends_2x.tcf")) {
        throw std::runtime_error("Failed to read sponza.tcf data file");
    }

    animate_camera_ = false;

    camera_movement_ = std::make_shared<FirstPersonMovement>();
    camera_movement_->init(Vector3(80, 50, 0), 1.5f, 0.5f);

    camera_.setVerticalFieldOfViewInDegrees(75.f);
    camera_.setNearPlaneDistance(1);
    camera_.setFarPlaneDistance(1000);

    update();
}

Context::~Context()
{
}

bool Context::readFile(std::string filepath)
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
    } catch (...) {
        if (reader) reader->release();
        if (tcf_scene) tcf_scene->release();
        return false;
    }

    instances_.clear();
    instances_by_mesh_.clear();

    instances_by_mesh_.reserve(tcf_scene->meshCount());
    for (unsigned int i = 0; i < tcf_scene->meshCount(); ++i) {
        const auto * mesh = tcf_scene->findMeshByIndex(i);
        std::vector<InstanceId> instances;
        instances.reserve(mesh->instanceCount());
        instances_.reserve(instances_.size() + mesh->instanceCount());
        for (unsigned int j = 0; j < mesh->instanceCount(); ++j) {
            const auto& model = mesh->transformationArray()[j];
            Instance new_model(100 + instances_.size());
            new_model.setMeshId(300 + instances_by_mesh_.size());
            new_model.setMaterialId(200);
            new_model.setTransformationMatrix(
                Matrix4x3(model.m00, model.m01, model.m02,
                          model.m10, model.m11, model.m12,
                          model.m20, model.m21, model.m22,
                          model.m30, model.m31, model.m32));
            instances.push_back(new_model.getId());
            instances_.push_back(new_model);
        }
        instances_by_mesh_.push_back(std::move(instances));
    }

    for (auto& instance : instances_)
    {
        instance.setStatic(instance.getMeshId() != 300);
    }

    int redShapes[] = { 35, 36, 37, 38, 39, 40, 41, 42, 69, 70, 71, 72, 73, 74,
        75, 76, 77, 78, 79 };
    int greenShapes[] = { 8, 19, 31, 33, 54, 57, 67, 68, 66, 80 };
    int yellowShapes[] = { 4, 5, 6, 7, 23, 24, 25, 26, 27, 28, 29, 30, 44, 45,
        46, 47, 48, 49, 50, 51, 52, 53 };
    int happyShapes[] = { 82 };
    int bunnyShapes[] = { 81 };
    int dragonShapes[] = { 83 };
    int *shapes[6] = { redShapes, greenShapes, yellowShapes,
        happyShapes, bunnyShapes, dragonShapes };
    int numberOfShapes[] = {
        sizeof(redShapes) / sizeof(int),
        sizeof(greenShapes) / sizeof(int),
        sizeof(yellowShapes) / sizeof(int),
        sizeof(happyShapes) / sizeof(int),
        sizeof(bunnyShapes) / sizeof(int),
        sizeof(dragonShapes) / sizeof(int) };
    Vector3 diffuse_colours[6] = {
        Vector3(1.f, 0.33f, 0.f),
        Vector3(0.2f, 0.8f, 0.2f),
        Vector3(0.8f, 0.8f, 0.2f),
        Vector3(0.8f, 0.4f, 0.4f),
        Vector3(0.4f, 0.8f, 0.4f),
        Vector3(0.4f, 0.4f, 0.8f)
    };
    Vector3 specular_colours[6] = {
        Vector3(0, 0, 0),
        Vector3(1, 1, 1),
        Vector3(0.8f, 0.8f, 0.2f),
        Vector3(0.8f, 0.4f, 0.4f),
        Vector3(0.4f, 0.8f, 0.4f),
        Vector3(0.4f, 0.4f, 0.8f)
    };
    float shininess[6] = { 0.f, 64.f, 128.f, 64.f, 0.f, 0.f };
    Material new_material(200);
    new_material.setDiffuseColour(Vector3(0.8f, 0.8f, 0.8f));
    materials_.push_back(new_material);
    for (int j = 0; j<6; ++j) {
        Material new_material(200 + j + 1);
        new_material.setDiffuseColour(diffuse_colours[j]);
        new_material.setSpecularColour(specular_colours[j]);
        new_material.setShininess(shininess[j]);
        materials_.push_back(new_material);
        for (int i = 0; i<numberOfShapes[j]; ++i) {
            int index = shapes[j][i];
            instances_[index].setMaterialId(new_material.getId());
        }
    }

    reader->release();
    tcf_scene->release();
    
    return true;
}

void Context::update()
{
    const auto clock_time = std::chrono::system_clock::now() - start_time_;
    const auto clock_millisecs
        = std::chrono::duration_cast<std::chrono::milliseconds>(clock_time);
    const float prev_time = time_seconds_;
    time_seconds_ = 0.001f * clock_millisecs.count();
    const float dt = time_seconds_ - prev_time;

    if (animate_camera_) {
        const float t = -0.3f * time_seconds_;
        const float ct = cosf(t);
        const float rx = ct < 0 ? -120.f : 120.f;
        const float st = sinf(t);
        const float rz = st < 0 ? -20.f : 20.f;
        const float m = 0.1f;
        const float ry = st < 0 ? -30.f : 30.f;
        Vector3 look_at(0, 30, 0);
        Vector3 cam_pos(
            rx * powf(fabs(ct), m),
            50 + ry * powf(fabs(st), m),
            25.f + rz * powf(fabs(st), m));
        camera_.setPosition(cam_pos);
        camera_.setDirection(normalize(look_at - cam_pos));
    } else {
        auto camera_translation_speed = getCamera().getLinearVelocity();
        auto camera_rotation_speed = getCamera().getRotationalVelocity();
        camera_movement_->moveForward(camera_translation_speed.z * dt);
        camera_movement_->moveRight(camera_translation_speed.x * dt);
        camera_movement_->spinHorizontal(camera_rotation_speed.x * dt);
        camera_movement_->spinVertical(camera_rotation_speed.y * dt);
        camera_.setPosition(camera_movement_->position());
        camera_.setDirection(camera_movement_->direction());
    }

    const float t = time_seconds_;

    const int num_of_directional_lights = 2;
    if (directional_lights_.empty())
    {
        directional_lights_.assign({ DirectionalLight(401), DirectionalLight(402) });

        directional_lights_[0].setDirection(normalize(Vector3(-10, -5, -2)));

        directional_lights_[1].setDirection(normalize(Vector3(10, 5, 2)));
    }

    const int num_of_point_lights = 20;
    if (point_lights_.empty())
    {
        auto r = std::default_random_engine(0);
        auto rand = std::uniform_real_distribution<float>(0.6f, 1.f);
        const LightId base_id = directional_lights_.back().getId();
        for (int i = 0; i < num_of_point_lights; ++i)
        {
            auto light = PointLight(LightId(base_id + i));
            light.setRange(20.f);
            light.setIntensity(Vector3(rand(r), rand(r), rand(r)));
            point_lights_.push_back(light);
        }
    }

    for (int i = 0; i < num_of_point_lights; ++i)
    {
        auto& light = point_lights_[i];
		float A = time_seconds_ + i * 6.28f / num_of_point_lights;
        light.setPosition(Vector3(120.f * cosf(A), 10.f, 40.f * sinf(A)));
    }

    const int num_of_spot_lights = 5;
    if (spot_lights_.empty())
    {
        const LightId base_id = point_lights_.back().getId();
        for (int i = 0; i < num_of_spot_lights; ++i)
        {
            auto light = SpotLight(LightId(base_id + i));
            light.setRange(300);
            spot_lights_.push_back(light);
        }

        spot_lights_[0].setConeAngleDegrees(60.f);

        spot_lights_[1].setConeAngleDegrees(60.f);

        spot_lights_[2].setPosition(Vector3(-97.75f, 8.34f, -5.25f + 3.f * cosf(3 + t)));
        spot_lights_[2].setDirection(normalize(Vector3(0.97f, 0.26f, 0.f)));
        spot_lights_[2].setConeAngleDegrees(90.f);
        spot_lights_[2].setCastShadow(false);
        spot_lights_[2].setStatic(true);

        spot_lights_[3].setPosition(Vector3(-20.56f, 4.17f, 0.44f + 3.f * cosf(4 + t)));
        spot_lights_[3].setDirection(normalize(Vector3(0.94f, 0.09f, -0.33f)));
        spot_lights_[3].setConeAngleDegrees(90.f);
        spot_lights_[3].setStatic(true);

        spot_lights_[4].setPosition(Vector3(37.88f, 3.36f, -10.53f + 3.f * cosf(3 + t)));
        spot_lights_[4].setDirection(normalize(Vector3(0.84f, 0.37f, 0.42f)));
        spot_lights_[4].setConeAngleDegrees(90.f);
        spot_lights_[4].setCastShadow(false);
        spot_lights_[4].setStatic(true);
    }

    spot_lights_[0].setPosition(Vector3(75.f, 110.f, -5.f + 15.f * cosf(t)));
    spot_lights_[0].setDirection(normalize(Vector3(-40.f, 0.f, -5.f) - spot_lights_[0].getPosition()));

    spot_lights_[1].setPosition(Vector3(-75.f, 110.f, -5.f + 15.f * cosf(1 + t)));
    spot_lights_[1].setDirection(normalize(Vector3(40.f, 0.f, -5.f) - spot_lights_[1].getPosition()));

    for (auto& instance : instances_)
    {
        if (instance.getMeshId() != 300) continue;

        auto xform = instance.getTransformationMatrix();
        const float bounce_y = 4;
        xform.m31 = 6.6f + bounce_y * (0.5f + 0.5f * cosf(t));
        instance.setTransformationMatrix(xform);
    }
}

bool Context::toggleCameraAnimation()
{
    return animate_camera_ = !animate_camera_;
}

float Context::getTimeInSeconds() const
{
    return time_seconds_;
}

Vector3 Context::getUpDirection() const
{
    return Vector3(0.f, 1.f, 0.f);
}

Vector3 Context::getAmbientLightIntensity() const
{
    return Vector3(0.1f, 0.1f, 0.05f);
}

const Camera& Context::getCamera() const
{
    return camera_;
}

Camera& Context::getCamera()
{
    return camera_;
}

const std::vector<DirectionalLight>& Context::getAllDirectionalLights() const
{
    return directional_lights_;
}

const std::vector<PointLight>& Context::getAllPointLights() const
{
    return point_lights_;
}

const std::vector<SpotLight>& Context::getAllSpotLights() const
{
    return spot_lights_;
}

const std::vector<Material>& Context::getAllMaterials() const
{
    return materials_;
}

const Material& Context::getMaterialById(MaterialId id) const
{
    return materials_[id - 200];
}

const std::vector<Instance>& Context::getAllInstances() const
{
    return instances_;
}

const Instance& Context::getInstanceById(InstanceId id) const
{
    return instances_[id - 100];
}

const std::vector<InstanceId> Context::getInstancesByMeshId(MeshId id) const
{
    return instances_by_mesh_[id - 300];
}
