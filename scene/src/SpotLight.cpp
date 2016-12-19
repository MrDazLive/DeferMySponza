#include <scene/scene.hpp>

using namespace scene;

SpotLight::SpotLight(LightId i) : id(i)
{
}

LightId SpotLight::getId() const
{
    return id;
}

bool SpotLight::isStatic() const
{
    return is_static;
}

void SpotLight::setStatic(bool b)
{
    is_static = b;
}

Vector3 SpotLight::getPosition() const
{
    return position;
}

void SpotLight::setPosition(Vector3 p)
{
    position = p;
}

Vector3 SpotLight::getDirection() const
{
    return direction;
}

void SpotLight::setDirection(Vector3 d)
{
    direction = d;
}

float SpotLight::getConeAngleDegrees() const
{
    return cone_angle_degrees;
}

void SpotLight::setConeAngleDegrees(float a)
{
    cone_angle_degrees = a;
}

float SpotLight::getRange() const
{
    return range;
}

void SpotLight::setRange(float r)
{
    range = r;
}

Vector3 SpotLight::getIntensity() const
{
    return intensity;
}

void SpotLight::setIntensity(Vector3 i)
{
    intensity = i;
}

bool SpotLight::getCastShadow() const
{
    return cast_shadow;
}

void SpotLight::setCastShadow(bool s)
{
    cast_shadow = s;
}
