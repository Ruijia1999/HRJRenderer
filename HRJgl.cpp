//
// Created by 黄睿佳 on 2020/12/17.
//

#include "HRJgl.h"
extern mat<4,4> projection;
extern mat<4,4> modelView;
extern mat<4,4> viewport;

vec3 barycentric(vec2 *pts, vec2 P) {
    vec3 u = cross (vec3(pts[2][0]-pts[0][0],pts[1][0]-pts[0][0],pts[0][0]-P[0]),vec3(pts[2][1]-pts[0][1],pts[1][1]-pts[0][1],pts[0][1]-P[1]));
    /* `pts` and `P` has integer value as coordinates
       so `abs(u[2])` < 1 means `u[2]` is 0, that means
       triangle is degenerate, in this case return something with negative coordinates */
    if (std::abs(u[2])<0) return vec3(-1,1,1);
    // std::cout<<1.f-(u.x+u.y)/u.z<<" "<< u.y/u.z<<" "<<u.x/u.z<<" ";
    return vec3(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
}
mat<4,4> getLookat(vec3 eye, vec3 center, vec3 up) {
    vec3 z = (eye-center).normalize();
    vec3 x = cross(up,z).normalize();
    vec3 y = cross(z,x).normalize();
    mat<4,4> Minv;
    Minv[0]={1,0,0,0};
    Minv[1]={0,1,0,0};
    Minv[2]={0,0,1,0};
    Minv[3]={0,0,0,1};
    mat<4,4> Tr = Minv;

    for (int i=0; i<3; i++) {
        Minv[0][i] = x[i];
        Minv[1][i] = y[i];
        Minv[2][i] = z[i];
        Tr[i][3] = -center[i];
    }

    mat<4,4> res = Minv*Tr;

    return res;
}
mat<4,4> getModelView(const double &width, const double &height){
    mat<4,4> modelView;
    modelView[0]=vec4{3*width/8,0,0,width/8+3*width/8};
    modelView[1]=vec4{0,3*height/8,0,height/8+3*height/8};
    modelView[2]=vec4{0,0,1.0/2,1.0/2};
    modelView[3]=vec4{0,0,0,1};
    return modelView;
}
mat<4,4> getProjection(double coeef){
    //  mat<4,4> m4={{{1/(aspect*tan),0,0,0},{0,1/tan,0,0},{0,0,(-near-far)/(near),0},{0,0,1,(2*near*far)/(near-far)}}};
    mat<4,4>  m4= {{{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,coeef,1}}};
    return m4;

}

void tiangle(vec4 clip_verts[3], HRJShader &shader, TGAImage &image, float *zbuffer){

    vec4 pts[3]  = { viewport*clip_verts[0],    viewport*clip_verts[1],    viewport*clip_verts[2]    };  // triangle screen coordinates before persp. division

   // vec4 pts[3]  = { clip_verts[0],    viewport*clip_verts[1],    viewport*clip_verts[2]    };  // triangle screen coordinates before persp. division
    //vec2 pts2[3] = { proj<2>(pts[0]), proj<2>(pts[1]/pts[1][3]), proj<2>(pts[2]/pts[2][3]) };  // triangle screen coordinates after  perps. division

    vec2 pts2[3] = { proj<2>(pts[0]/pts[0][3]), proj<2>(pts[1]/pts[1][3]), proj<2>(pts[2]/pts[2][3]) };  // triangle screen coordinates after  perps. division



    vec2 bboxmin( std::numeric_limits<double>::max(),  std::numeric_limits<double>::max());
    vec2 bboxmax(-std::numeric_limits<double>::max(), -std::numeric_limits<double>::max());
    vec2 clamp(image.get_width()-1, image.get_height()-1);
    for (int i=0; i<3; i++) {
        //std::cout << pts[i][0] << "," << pts[i][1] << ","<< pts[i][2] << "  ";
        for (int j = 0; j < 2; j++) {
            bboxmin[j] = std::max(0., std::min(bboxmin[j], pts2[i][j]));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts2[i][j]));
        }
    }

    for (int x=(int)bboxmin.x; x<=(int)bboxmax.x; x++) {
        for (int y=(int)bboxmin.y; y<=(int)bboxmax.y; y++) {
            vec3 bc_screen  = barycentric(pts2, vec2(x, y));
            //std::cout << bc_screen[0] << "," << bc_screen[1] << ","<< bc_screen[2] << "  ";
            vec3 bc_clip    = vec3(bc_screen.x/pts[0][3], bc_screen.y/pts[1][3], bc_screen.z/pts[2][3]);
            bc_clip = bc_clip/(bc_clip.x+bc_clip.y+bc_clip.z); // check https://github.com/ssloy/tinyrenderer/wiki/Technical-difficulties-linear-interpolation-with-perspective-deformations
            double frag_depth = vec3(clip_verts[0][2], clip_verts[1][2], clip_verts[2][2])*bc_screen;
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0 || zbuffer[x+y*image.get_width()]>frag_depth) continue;
            TGAColor color ;
            bool discard = shader.fragment(bc_clip, color);
            if (discard) continue;
            zbuffer[x+y*image.get_width()] = frag_depth;
            image.set(x, y, color);
        }
    }
}
