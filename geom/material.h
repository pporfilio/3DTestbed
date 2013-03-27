#ifndef MATERIAL_H
#define MATERIAL_H

#include "CS123Algebra.h"
#include <map>
#include <string>
#include <QFont>


/*
 * A Material contains information for drawing routines to know what values to
 * set for things like color and line/point size. I decided that every material
 * works for lines, points, and polygons, so for example lineWidth doesn't apply
 * to drawing solid polygons and is used as point size for points.
 *
 * I also wanted a global list of materials which I've implemented as a map from
 * strings to materials. This is really dangerous for a few reasons: anything
 * can add, remove, or change Materials in that map even as a bunch of objects are
 * pointing to or accessing by name a Material in the map. Also,
 * I use default materials in Vertex, Edge, and Polygon that have to be initialized
 * somewhere before any of those are drawn. I didn't want to make a check every time
 * I constructed any of those objects to see if allMaterials had been initialized,
 * or to make a check every time I tried to access a material.
 *
 */

class Material
{
public:
    Material(vec3<float> rgb, float lineWidth);
    vec3<float> getColor() const;
    float getLineWidth() const;
    void setColor(vec3<float> rgb);
    void setLineWidth(float lineWidth);
    void applyColor() const;
    void applyFontColor() const;

    void setFont(QFont font);
    QFont getFont() const;
    void setFontColor(vec3<float> color);
    vec3<float> getFontColor() const;

private:
    vec3<float> _color;
    float _lineWidth;
    vec3<float> _fontColor;
    QFont _font;
};

extern std::map<std::string, Material *> allMaterials;
extern std::string DEFAULT_POLY_MTL;
extern std::string DEFAULT_EDGE_MTL;
extern std::string DEFAULT_VERT_MTL;


extern void addMaterial(std::string key, Material *mtl);

#endif // MATERIAL_H
