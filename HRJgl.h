//
// Created by 黄睿佳 on 2020/12/17.
//

#ifndef HRJRENDERER_HRJGL_H
#define HRJRENDERER_HRJGL_H

#include "geometry.h"
#include "tgaimage.h"


struct HRJShader{
    virtual vec4 vertex(int iface, int nthvert) = 0;
    virtual bool fragment(vec3 bar, TGAColor &color) = 0;
};

vec3 barycentric(vec2 *pts, vec2 P);
mat<4,4> getLookat(vec3 eye, vec3 center, vec3 up);
mat<4,4> getProjection(double coeff);
mat<4,4> getModelView(const double &width, const double &height);
void tiangle(vec4 *pts, HRJShader &shader, TGAImage &image, float *zbuffer);

#endif //HRJRENDERER_HRJGL_H
