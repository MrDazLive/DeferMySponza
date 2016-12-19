#include <scene/scene.hpp>

using namespace scene;

DirectionalLight::DirectionalLight(LightId i) : id(i)
{
}

LightId DirectionalLight::getId() const
{
    return id;
}

bool DirectionalLight::isStatic() const
{
    return is_static;
}

void DirectionalLight::setStatic(bool b)
{
    is_static = b;
}

Vector3 DirectionalLight::getDirection() const
{
    return direction;
}

void DirectionalLight::setDirection(Vector3 d)
{
    direction = d;
}

Vector3 DirectionalLight::getIntensity() const
{
    return intensity;
}

void DirectionalLight::setIntensity(Vector3 i)
{
    intensity = i;
}
