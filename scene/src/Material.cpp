#include <scene/scene.hpp>

using namespace scene;

Material::Material(MaterialId i) : id(i)
{
    diffuse_colour = Vector3(1, 1, 1);
    specular_colour = Vector3(1, 1, 1);
    shininess = 0;
}

MaterialId Material::getId() const
{
    return id;
}

Vector3 Material::getDiffuseColour() const
{
    return diffuse_colour;
}

void Material::setDiffuseColour(Vector3 c)
{
    diffuse_colour = c;
}

Vector3 Material::getSpecularColour() const
{
    return specular_colour;
}

void Material::setSpecularColour(Vector3 c)
{
    specular_colour = c;
}

float Material::getShininess() const
{
	return shininess;
}

void Material::setShininess(float s)
{
	shininess = s;
}

int Material::getMainTextureId() const {
	return main_texture_id;
}

void Material::setMainTextureId(int t) {
	main_texture_id = t;
}

int Material::getNormalTextureId() const {
	return normal_texture_id;
}

void Material::setNormalTextureId(int t) {
	normal_texture_id = t;
}

bool Material::isShiny() const
{
    return shininess > 0;
}
