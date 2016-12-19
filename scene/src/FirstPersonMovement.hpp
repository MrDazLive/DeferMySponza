/*
@file      FirstPersonMovement.hpp
@author    Tyrone Davison
@date      October 2012
*/

#pragma once
#ifndef __FIRSTPERSONMOVEMENT__
#define __FIRSTPERSONMOVEMENT__

namespace scene {

/**
Provides position and direction from strafe and rotate commands.
*/
class FirstPersonMovement
{
public:

    FirstPersonMovement()
    {
        update();
    }

    void init(Vector3 position,
              float azimuth,
              float inclination)
    {
        position_ = position;
        azimuth_radians_ = azimuth;
        inclination_radians_ = inclination;
        update();
    }

    Vector3 position()
    {
        return position_;
    }

    Vector3 direction()
    {
        return forward_;
    }

    float azimuth()
    {
        return azimuth_radians_;
    }

    float inclination()
    {
        return inclination_radians_;
    }

    void moveForward(float distance)
    {
        position_.x += forward_.x * distance;
        position_.y += forward_.y * distance;
        position_.z += forward_.z * distance;
    }

    void moveRight(float distance)
    {
        position_.x += right_.x * distance;
        position_.y += right_.y * distance;
        position_.z += right_.z * distance;
    }

    void spinHorizontal(float angle_radians)
    {
        azimuth_radians_ += angle_radians;
        update();
    }

    void spinVertical(float angle_radians)
    {
        inclination_radians_ += angle_radians;
        update();
    }

private:
    Vector3 position_;
    Vector3 forward_;
    Vector3 right_;
    float azimuth_radians_{ 0 };
    float inclination_radians_{ 0 };

    void update()
    {
        const float azi = azimuth_radians_;
        const float inc = inclination_radians_;
        const float cos_azi = cosf(azi);
        const float sin_azi = sinf(azi);
        const float cos_inc = cosf(inc);
        const float sin_inc = sinf(inc);
        forward_ = Vector3(cos_inc * -sin_azi, sin_inc, cos_inc * -cos_azi);
        right_ = Vector3(cos_azi, 0, -sin_azi);
    }

};

} // end namespace scene

#endif
