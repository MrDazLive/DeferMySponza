#include <scene/scene.hpp>

using namespace scene;

PointLight::PointLight(LightId i) : id(i)
{
}

LightId PointLight::getId() const
{
    return id;
}

bool PointLight::isStatic() const
{
    return is_static;
}

void PointLight::setStatic(bool b)
{
    is_static = b;
}

Vector3 PointLight::getPosition() const
{
    return position;
}

void PointLight::setPosition(Vector3 p)
{
    position = p;
}

float PointLight::getRange() const
{
    return range;
}

void PointLight::setRange(float r)
{
    range = r;
}

Vector3 PointLight::getIntensity() const
{
    return intensity;
}

void PointLight::setIntensity(Vector3 i)
{
    intensity = i;
}
