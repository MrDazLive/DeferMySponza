#pragma once

#include "scene_fwd.hpp"

namespace scene {

class Material
{
public:
    Material(MaterialId i);

    MaterialId getId() const;

    bool isStatic() const { return true; }

    Vector3 getDiffuseColour() const;
    void setDiffuseColour(Vector3 c);

    Vector3 getSpecularColour() const;
    void setSpecularColour(Vector3 c);

	float getShininess() const;
	void setShininess(float s);

	int getMainTextureId() const;
	void setMainTextureId(int t);

	int getNormalTextureId() const;
	void setNormalTextureId(int t);

    bool isShiny() const;


private:
    Vector3 diffuse_colour;
    float shininess;
    Vector3 specular_colour;
	int main_texture_id{ 128 };

	Vector2 pad{ 128, 128 };
	unsigned int normal_texture_id{ 128 };
    MaterialId id;

};

} // end namespace scene
