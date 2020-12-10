
#include "tgaimage.h"
#include "model.h"
#include <vector>
#include "geometry.h"
#include <algorithm>
#include <iostream>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(0, 255,   0,   255);
const int width = 1024;
const int height = 1024;
//教程:https://zhuanlan.zhihu.com/p/65495373
vec3 barycentric(vec2 *pts, vec2 P) {
    vec3 u = cross (vec3(pts[2][0]-pts[0][0],pts[1][0]-pts[0][0],pts[0][0]-P[0]),vec3(pts[2][1]-pts[0][1],pts[1][1]-pts[0][1],pts[0][1]-P[1]));
    /* `pts` and `P` has integer value as coordinates
       so `abs(u[2])` < 1 means `u[2]` is 0, that means
       triangle is degenerate, in this case return something with negative coordinates */
    if (std::abs(u[2])<1) return vec3(-1,1,1);
    return vec3(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
}


bool LineDrawing(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color){
    bool steep = false;
    if (std::abs(x0-x1)<std::abs(y0-y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0>x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1-x0;
    int dy = y1-y0;
    float derror = std::abs(dy/float(dx));
    float error = 0;
    int y = y0;
    for (int x=x0; x<=x1; x++) {
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
        error += derror;
        if (error>.5) {
            y += (y1>y0?1:-1);
            error -= 1.;
        }
    }
}


//扫描线法，有裂缝。
void triangle(vec2 t0, vec2 t1, vec2 t2, TGAImage &image, TGAColor color) {
    if (t0.y==t1.y && t0.y==t2.y) return; // I dont care about degenerate triangles
    // sort the vertices, t0, t1, t2 lower−to−upper (bubblesort yay!)
    if (t0.y>t1.y) std::swap(t0, t1);
    if (t0.y>t2.y) std::swap(t0, t2);
    if (t1.y>t2.y) std::swap(t1, t2);
    int total_height = t2.y-t0.y;
    for (int i=0; i<total_height; i++) {
        bool second_half = i>t1.y-t0.y || t1.y==t0.y;
        int segment_height = second_half ? t2.y-t1.y : t1.y-t0.y;
        float alpha = (float)i/total_height;
        float beta  = (float)(i-(second_half ? t1.y-t0.y : 0))/segment_height; // be careful: with above conditions no division by zero here
        vec2 A = t0 + (t2-t0)*alpha;
        vec2 B = second_half ? t1 + (t2-t1)*beta : t0 + (t1-t0)*beta;
        if (A.x>B.x) std::swap(A, B);
        for (int j=A.x; j<=B.x; j++) {
            image.set(j, t0.y+i, color); // attention, due to int casts t0.y+i != A.y
        }
    }
}
//扫描线法，有裂缝。
void draw_triangle(vec3 *pts, TGAImage &image, TGAColor color, float *zbuffer ) {
    vec2 bboxmin(image.get_width()-1,  image.get_height()-1);
    vec2 bboxmax(0, 0);
    vec2 clamp(image.get_width()-1, image.get_height()-1);
    vec2 vt2D[3];
    vt2D[0]=vec2{pts[0].x,pts[0].y};
    vt2D[1]=vec2{pts[1].x,pts[1].y};
    vt2D[2]=vec2{pts[2].x,pts[2].y};
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            bboxmin[j] = std::max(0.0,        std::min(bboxmin[j], pts[i][j]));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
        }
    }
    vec3 P;
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) {
            vec3 bc_screen  = barycentric(vt2D, vec2(P.x,P.y));
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue;
            P.z=0;
            for (int i=0; i<3; i++) P.z += (pts[i][2]+200)*bc_screen[i];
            if(zbuffer[(int)P.x+(int)P.y*width]<P.z){
                zbuffer[(int)P.x+(int)P.y*width]=P.z;
                image.set(P.x, P.y, color);
            }


        }
    }
}
mat<4,4> getProjection(double tan, double aspect, double near, double far){
    mat<4,4> m4={{{1/(aspect*tan),0,0,0},{0,1/tan,0,0},{0,0,(-near-far)/(near),0},{0,0,1,(2*near*far)/(near-far)}}};
    m4= {{{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,-0.5,1}}};
    for(int i = 0; i < 4; i++){
        for(int j=0; j < 4; j++){
            std::cout<<m4[i][j]<<" ";
        }
        std::cout<<std::endl;
    }
    return m4;

}


const mat<4,4> projection = getProjection(0.9,1,-100,-500);

void drawModelLine(TGAImage &image){
    Model model("/Users/huangruijia/Desktop/CLion/HRJrenderer/obj/african_head.obj");
    std::vector<int> face = model.facet();
    for (int i=0; i<model.nfaces(); i++) {
        for (int j=0; j<3; j++) {
            vec4 v1={(model.vert(face[i*3+j]).x),model.vert(face[i*3+j]).y,model.vert(face[i*3+j]).z,1};

            vec4 v2={model.vert(face[i*3+(j+1)%3]).x,model.vert(face[i*3+(j+1)%3]).y,model.vert(face[i*3+(j+1)%3]).z,1};
//
            vec4 r1=projection*v1;
            vec4 r2=projection*v2;
            std::cout<<"hhh:"<<r1[0]<<" "<<v1[0]<<" "<<r1[0]/r1[3]<<" ";
            v1[0]=(v1[0]/r1[3]+1.)*width/2;
            v1[1]=(v1[1]/r1[3]+1.)*height/2;
            v2[0]=(v2[0]/r2[3]+1.)*width/2;
            v2[1]=(v2[1]/r2[3]+1.)*height/2;
            LineDrawing(v1[0],v1[1], v2[0], v2[1], image, white);
        }

    }
}


void drawModelColor(TGAImage &image){

    Model model("/Users/huangruijia/Desktop/CLion/HRJrenderer/obj/african_head.obj");
    std::vector<int> face = model.facet();
    float zbuffer[width*height];
    for(int i =0; i < width*height;i++){
        zbuffer[i]=std::numeric_limits<float>::min();
    }
    for (int i=0; i<model.nfaces(); i++) {
       // std::cout<<" h";
        vec3 world_coords[3];
        vec3 screen_coords[3];

        for (int j=0; j<3; j++) {

            world_coords[j] =  model.vert(face[i*3+j]);
//
//            vec4 orig = vec4{world_coords[j][0],world_coords[j][1],world_coords[j][2],1};
//            vec4 re = projection*orig;
//            screen_coords[j] = vec3 ((world_coords[j].x/re[3]+1)*width/2,(world_coords[j].y/re[3]+1)*height/2,world_coords[j].z/re[3]+1);
            screen_coords[j] = vec3 ((world_coords[j].x+1)*width/2,(world_coords[j].y+1)*height/2,world_coords[j].z+1);
        }
        vec3 light_dir = {0,0,-1};
        vec3 n = (world_coords[2]-world_coords[0])^(world_coords[1]-world_coords[0]);
        n.normalize();
        float intensity = n*light_dir;

        if (intensity>0) {
         //   std::cout<<intensity<<" h";
          draw_triangle(screen_coords, image, TGAColor(intensity*255, intensity*255, intensity*255, 255), zbuffer);
        }
    }
}

//请深入探究三角形光栅化方法
int main(int argc, char** argv) {
    TGAImage image(width, height, TGAImage::RGB);
    vec2 t0 = {5,20};
    vec2 t1 = {15,30};
    vec2 t2 = {28,6};
    //triangle(t0, t1, t2, image, red);
    drawModelColor(image);

    image.write_tga_file("output.tga");

    return 0;
}

