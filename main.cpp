
#include "tgaimage.h"
#include "model.h"
#include <vector>
#include "geometry.h"
#include "HRJgl.h"
#include <algorithm>
#include <iostream>
//#include "HRJmath.h"
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(0, 255,   0,   255);

extern int width = 1024;
extern int height = 1024;

const vec3 light_dir(0,0,3); // light source
//const vec3       eye(0,0,3); // camera position
//const vec3    center(0,0,0); // camera direction
const vec3        up(0,1,0); // camera up vector

extern mat<4,4> projection = getProjection(-1.f/(vec3(1,0.5,3)).norm());
extern mat<4,4> modelView=getLookat(vec3(1,0.5,3),vec3(0,0,0),vec3(0,1,0));
extern mat<4,4> viewport=getModelView(width,height);

//教程:https://zhuanlan.zhihu.com/p/65495373


struct GouraudShader: HRJShader{
    vec3 varying_normal[3];
    vec2 varying_uv[3];  // triangle uv coordinates, written by the vertex shader, read by the fragment shader
    vec3 varying_tri[3];
    const Model &model;
    vec3 light;
    GouraudShader(const Model &m) : model(m) {
        light = proj<3>(projection*modelView*embed<4>(light_dir)); // transform the light vector to the normalized device coordinates
        light.normalize();
    }
    vec4 vertex(int iface, int nthvert) {
        varying_tri[nthvert] = proj<3>(projection*modelView*(embed<4>(model.vert(iface,nthvert))));
        varying_normal[nthvert] = model.normal(iface, nthvert) ; // get diffuse lighting intensity
        varying_uv[nthvert] = model.uv(iface,nthvert);
        vec4 gl_Vertex = embed<4>(model.vert(iface, nthvert)); // read the vertex from .obj file
        return projection*modelView*gl_Vertex; // transform it to screen coordinates
    }

    bool fragment(vec3 bar, TGAColor &color) {
        vec3 bn = varying_normal[0]*bar.x+varying_normal[1]*bar.y+varying_normal[2]*bar.z;
        vec2 uv = varying_uv[0]*bar.x+varying_uv[1]*bar.y+varying_uv[2]*bar.z;

        bn=bn.normalize();

        mat<3,3> AI = mat<3,3>{ {varying_tri[1] - varying_tri[0], varying_tri[2] - varying_tri[0], bn} }.invert();
        vec3 i = AI * vec3(varying_uv[1][0] - varying_uv[0][0], varying_uv[2][0] - varying_uv[0][0], 0);
        vec3 j = AI * vec3(varying_uv[1][1] - varying_uv[0][1], varying_uv[2][1] - varying_uv[0][1], 0);
        mat<3,3> B = mat<3,3>{ {i.normalize(), j.normalize(), bn} }.transpose();

        vec3 n = (B * model.normal(uv)).normalize(); // transform the normal from the texture to the tangent space

        float intensity = bn*light;
        double diff = std::max(0., n*light); // diffuse light intensity
        vec3 r = (n*(n*light)*2 - light).normalize(); // reflected light direction, specular mapping is described here: https://github.com/ssloy/tinyrenderer/wiki/Lesson-6-Shaders-for-the-software-renderer
        double spec = std::pow(std::max(r.z, 0.), 5+model.specular(uv)); // specular intensity, note that the camera lies on the z-axis (in ndc), therefore simple r.z

        TGAColor c;
        if(intensity>0)
            c = model.diffuse(uv)*intensity; // well duh
        //std::cout<<intensity<<" ";
        for (int i=0; i<3; i++)
            color[i] = std::min<int>(10 + c[i]*(diff+spec), 255); // (a bit of ambient light, diff + spec), clamp the result
//        if(intensity>0)
//            color = model.diffuse(uv)*intensity; // well duh

        return false;                              // no, we do not discard this pixel
    }
};


int main() {
    float zbuffer[1024*1024]{-99999}; // note that the z-buffer is initialized with minimal possible values
    for(int i = 0; i < 1024*1024; i++)
        zbuffer[i]=-99999;
    TGAImage framebuffer(width, height, TGAImage::RGB); // the output image
    Model model("/Users/huangruijia/Desktop/CLion/HRJrenderer/obj/african_head.obj");
    for (int m=1; m<2; m++) { // iterate through all input objects

        GouraudShader shader(model);
        std::cout<<model.nfaces()<<std::endl;
        for (int i=0; i<model.nfaces(); i++) { // for every triangle
            vec4 clip_vert[3]; // triangle coordinates (clip coordinates), written by VS, read by FS
            for (int j=0; j<3; j++)
                clip_vert[j] = shader.vertex(i, j); // call the vertex shader for each triangle vertex
            tiangle(clip_vert, shader, framebuffer, zbuffer); // actual rasterization routine call
        }
    }
    Model model1("/Users/huangruijia/Desktop/CLion/HRJrenderer/obj/african_head_eye_inner.obj");
    for (int m=1; m<2; m++) { // iterate through all input objects

        GouraudShader shader(model1);
        std::cout<<model1.nfaces()<<std::endl;
        for (int i=0; i<model1.nfaces(); i++) { // for every triangle
            vec4 clip_vert[3]; // triangle coordinates (clip coordinates), written by VS, read by FS
            for (int j=0; j<3; j++)
                clip_vert[j] = shader.vertex(i, j); // call the vertex shader for each triangle vertex
            tiangle(clip_vert, shader, framebuffer, zbuffer); // actual rasterization routine call
        }
    }

    Model model2("/Users/huangruijia/Desktop/CLion/HRJrenderer/obj/african_head_eye_outter.obj");
    for (int m=1; m<2; m++) { // iterate through all input objects

        GouraudShader shader(model2);
        std::cout<<model2.nfaces()<<std::endl;
        for (int i=0; i<model2.nfaces(); i++) { // for every triangle
            vec4 clip_vert[3]; // triangle coordinates (clip coordinates), written by VS, read by FS
            for (int j=0; j<3; j++)
                clip_vert[j] = shader.vertex(i, j); // call the vertex shader for each triangle vertex
            tiangle(clip_vert, shader, framebuffer, zbuffer); // actual rasterization routine call
        }
    }
    framebuffer.write_tga_file("output.tga"); // the vertical flip is moved inside the function
    return 0;
}
