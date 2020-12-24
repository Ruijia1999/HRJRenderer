
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

const vec3 light_dir(1,1,1); // light source
//const vec3       eye(0,0,3); // camera position
//const vec3    center(0,0,0); // camera direction
const vec3        up(0,1,0); // camera up vector

extern mat<4,4> projection = getProjection(-1.f/(vec3(1,1,3)).norm());
extern mat<4,4> modelView=getLookat(vec3(1,1,3),vec3(0,0,0),vec3(0,1,0));
extern mat<4,4> viewport=getModelView(width,height);

//教程:https://zhuanlan.zhihu.com/p/65495373


struct GouraudShader: HRJShader{
    vec3 varying_normal[3];
    const Model &model;
    vec3 light;
    GouraudShader(const Model &m) : model(m) {
        light = light_dir; // transform the light vector to the normalized device coordinates
        light.normalize();
    }
    vec4 vertex(int iface, int nthvert) {
        varying_normal[nthvert] = model.normal(iface, nthvert) ; // get diffuse lighting intensity
        vec4 gl_Vertex = embed<4>(model.vert(iface, nthvert)); // read the vertex from .obj file
        return projection*modelView*gl_Vertex; // transform it to screen coordinates
    }

    bool fragment(vec3 bar, TGAColor &color) {
        vec3 n = varying_normal[0]*bar.x+varying_normal[1]*bar.y+varying_normal[2]*bar.z;
//        float intensity = varying_intensity*bar;
   //     n.normalize();
        float intensity = n*light;
        // interpolate intensity for the current pixel
        //std::cout<<intensity<<" ";
       if(intensity>0)
        color = TGAColor(255, 255, 255)*intensity; // well duh
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
    framebuffer.write_tga_file("output.tga"); // the vertical flip is moved inside the function
    return 0;
}
