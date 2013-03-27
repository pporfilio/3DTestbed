#include "material.h"
#include <GLU.h>
#include "glm.h"


std::map<std::string, Material *> allMaterials;
std::string DEFAULT_POLY_MTL;
std::string DEFAULT_EDGE_MTL;
std::string DEFAULT_VERT_MTL;

void addMaterial(std::string key, Material *mtl) {
    allMaterials.insert(std::pair<std::string, Material *>(key, mtl));
}


Material::Material(vec3<float> rgb, float lineWidth):
    _color(rgb), _lineWidth(lineWidth)
{
    _font = QFont("Deja Vu Sans Mono", 16, 4);
    _fontColor = vec3<float>(1, 1, 1);
}

vec3<float> Material::getColor() const {
    return _color;
}

float Material::getLineWidth() const {
    return _lineWidth;
}

void Material::setColor(vec3<float> rgb) {
    _color = rgb;
}

void Material::setLineWidth(float lineWidth) {
    _lineWidth = lineWidth;
}

void Material::applyColor() const {
    glColor3f(_color.x, _color.y, _color.z);
}

void Material::applyFontColor() const {
    glColor3f(_fontColor.x, _fontColor.y, _fontColor.z);
}

void Material::setFont(QFont font) {
    _font = font;
}

QFont Material::getFont() const {
    return _font;
}

void Material::setFontColor(vec3<float> color) {
    _fontColor = color;
}

vec3<float> Material::getFontColor() const {
    return _fontColor;
}
