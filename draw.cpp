////
//// Created by 黄睿佳 on 2020/12/17.
////
//
//#include "draw.h"
//
//
//const int height=1024;
//const int width=1024;
//bool LineDrawing(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color){
//    bool steep = false;
//    if (std::abs(x0-x1)<std::abs(y0-y1)) {
//        std::swap(x0, y0);
//        std::swap(x1, y1);
//        steep = true;
//    }
//    if (x0>x1) {
//        std::swap(x0, x1);
//        std::swap(y0, y1);
//    }
//    int dx = x1-x0;
//    int dy = y1-y0;
//    float derror = std::abs(dy/float(dx));
//    float error = 0;
//    int y = y0;
//    for (int x=x0; x<=x1; x++) {
//        if (steep) {
//            image.set(y, x, color);
//        } else {
//            image.set(x, y, color);
//        }
//        error += derror;
//        if (error>.5) {
//            y += (y1>y0?1:-1);
//            error -= 1.;
//        }
//    }
//}
//
//
////扫描线法，有裂缝。
//void triangle(vec2 t0, vec2 t1, vec2 t2, TGAImage &image, TGAColor color) {
//    if (t0.y==t1.y && t0.y==t2.y) return; // I dont care about degenerate triangles
//    // sort the vertices, t0, t1, t2 lower−to−upper (bubblesort yay!)
//    if (t0.y>t1.y) std::swap(t0, t1);
//    if (t0.y>t2.y) std::swap(t0, t2);
//    if (t1.y>t2.y) std::swap(t1, t2);
//    int total_height = t2.y-t0.y;
//    for (int i=0; i<total_height; i++) {
//        bool second_half = i>t1.y-t0.y || t1.y==t0.y;
//        int segment_height = second_half ? t2.y-t1.y : t1.y-t0.y;
//        float alpha = (float)i/total_height;
//        float beta  = (float)(i-(second_half ? t1.y-t0.y : 0))/segment_height; // be careful: with above conditions no division by zero here
//        vec2 A = t0 + (t2-t0)*alpha;
//        vec2 B = second_half ? t1 + (t2-t1)*beta : t0 + (t1-t0)*beta;
//        if (A.x>B.x) std::swap(A, B);
//        for (int j=A.x; j<=B.x; j++) {
//            image.set(j, t0.y+i, color); // attention, due to int casts t0.y+i != A.y
//        }
//    }
//}
////重心法，有裂缝。
//void draw_triangle(vec3 *pts, TGAImage &image, TGAColor color, float *zbuffer ) {
//    vec2 bboxmin(image.get_width()-1,  image.get_height()-1);
//    vec2 bboxmax(0, 0);
//    vec2 clamp(image.get_width()-1, image.get_height()-1);
//    vec2 vt2D[3];
//    vt2D[0]=vec2{pts[0].x,pts[0].y};
//    vt2D[1]=vec2{pts[1].x,pts[1].y};
//    vt2D[2]=vec2{pts[2].x,pts[2].y};
//    for (int i=0; i<3; i++) {
//        //std::cout<<"("<<pts[i].x<<", "<<pts[i].y<<")"<<std::endl;
//        for (int j=0; j<2; j++) {
//
//            bboxmin[j] = std::max(0.0,        std::min(bboxmin[j], pts[i][j]));
//            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
//        }
//    }
//    vec3 P;
//    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
//        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) {
//            vec3 bc_screen  = barycentric(vt2D, vec2(P.x,P.y));
//            if(!isInTriangle(vt2D, vec2(P.x,P.y)))
//                continue;
////            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) {
////               // std::cout<<(P.y-vt2D[0].y)/(P.x-vt2D[0].x)<<" "<<(P.y-vt2D[1].y)/(P.x-vt2D[1].x)<<" "<<(P.y-vt2D[2].y)/(P.x-vt2D[2].x)<<std::endl;
//////if( (P.y-vt2D[0].y)/(P.x-vt2D[0].x)!=INFINITY&&(P.y-vt2D[1].y)/(P.x-vt2D[1].x)!=INFINITY&&(P.y-vt2D[2].y)/(P.x-vt2D[2].x)!=INFINITY)
////
////                  // std::cout<<"("<<P.x<<", "<<P.y<<"), ";
////
////                continue;
////            }
//            P.z=0;
//            for (int i=0; i<3; i++) P.z += (pts[i][2]+200)*bc_screen[i];
//            if(zbuffer[(int)P.x+(int)P.y*width]<P.z){
//                zbuffer[(int)P.x+(int)P.y*width]=P.z;
//                image.set(P.x, P.y, color);
//            }
//
//
//        }
//    }
//
//}
////重心法，有裂缝。
//void draw_triangle_color(vec3 *pts, TGAImage &image, TGAColor *colors, float *zbuffer ) {
//    vec2 bboxmin(image.get_width()-1,  image.get_height()-1);
//    vec2 bboxmax(0, 0);
//    vec2 clamp(image.get_width()-1, image.get_height()-1);
//    vec2 vt2D[3];
//    vt2D[0]=vec2{pts[0].x,pts[0].y};
//    vt2D[1]=vec2{pts[1].x,pts[1].y};
//    vt2D[2]=vec2{pts[2].x,pts[2].y};
//    TGAColor vcolor;
//    for (int i=0; i<3; i++) {
//        //std::cout<<"("<<pts[i].x<<", "<<pts[i].y<<")"<<std::endl;
//        for (int j=0; j<2; j++) {
//
//            bboxmin[j] = std::max(0.0,        std::min(bboxmin[j], pts[i][j]));
//            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
//        }
//    }
//    vec3 P;
//    std::cout<<barycentric(vt2D,vec2(15,3));
////    vec3 v0(1,1,0);
////    vec3 v1(8,15,0);
////    vec3 v2(15,3,0);
//    std::cout<<std::endl;std::cout<<std::endl;
//    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
//        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) {
//            vec3 bc_screen  = barycentric(vt2D, vec2(P.x,P.y));
//            if(!isInTriangle(vt2D, vec2(P.x,P.y))){
//
//                continue;
//            }
//
////            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) {
////               // std::cout<<(P.y-vt2D[0].y)/(P.x-vt2D[0].x)<<" "<<(P.y-vt2D[1].y)/(P.x-vt2D[1].x)<<" "<<(P.y-vt2D[2].y)/(P.x-vt2D[2].x)<<std::endl;
//////if( (P.y-vt2D[0].y)/(P.x-vt2D[0].x)!=INFINITY&&(P.y-vt2D[1].y)/(P.x-vt2D[1].x)!=INFINITY&&(P.y-vt2D[2].y)/(P.x-vt2D[2].x)!=INFINITY)
////
////                  // std::cout<<"("<<P.x<<", "<<P.y<<"), ";
////
////                continue;
////            }
//            P.z=0;
//            float r=0, g=0, b=0;
//            float k=0;
//
//            for (int i=0; i<3; i++) {
//
//                P.z += (pts[i][2]+200)*bc_screen[i];
//                r+=colors[i].bgra[0]*bc_screen[i];
//                g+=colors[i].bgra[1]*bc_screen[i];
//                b+=colors[i].bgra[2]*bc_screen[i];
//            }
////            vec3 v0(1,1,0);
////            vec3 v1(8,15,0);
////            vec3 v2(15,3,0);
//            vcolor=TGAColor(r,g,b);
//
//
//            if(zbuffer[(int)P.x+(int)P.y*width]<P.z){
//                zbuffer[(int)P.x+(int)P.y*width]=P.z;
//                image.set(P.x, P.y, vcolor);
//            }
//
//
//        }
//    }
//
//}
////贴图
//void draw_triangle(vec3 *pts, TGAImage &image, Model &model, float *zbuffer ) {
//    vec2 bboxmin(image.get_width()-1,  image.get_height()-1);
//    vec2 bboxmax(0, 0);
//    vec2 clamp(image.get_width()-1, image.get_height()-1);
//    vec2 vt2D[3];
//    vt2D[0]=vec2{pts[0].x,pts[0].y};
//    vt2D[1]=vec2{pts[1].x,pts[1].y};
//    vt2D[2]=vec2{pts[2].x,pts[2].y};
//    for (int i=0; i<3; i++) {
//        for (int j=0; j<2; j++) {
//            bboxmin[j] = std::max(0.0,        std::min(bboxmin[j], pts[i][j]));
//            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
//        }
//    }
//    vec3 P;
//    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
//        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) {
//            vec3 bc_screen  = barycentric(vt2D, vec2(P.x,P.y));
//            if (bc_screen.x<-0.0 || bc_screen.y<-0.0 || bc_screen.z<-0.0) {
//                std::cout<<(P.y-vt2D[0].y)/(P.x-vt2D[0].x)<<" "<<(P.y-vt2D[1].y)/(P.x-vt2D[1].x)<<" "<<(P.y-vt2D[2].y)/(P.x-vt2D[2].x)<<", ";
//                continue;
//            }
//            P.z=0;
//            for (int i=0; i<3; i++) P.z += (pts[i][2]+200)*bc_screen[i];
//            if(zbuffer[(int)P.x+(int)P.y*width]<P.z){
//                zbuffer[(int)P.x+(int)P.y*width]=P.z;
//                image.set(P.x, P.y,model.diffuse(vec2(P.x,P.y)) );
//            }
//
//
//        }
//    }
//}
//
//
//
//void drawModelLine(TGAImage &image){
//    Model model("/Users/huangruijia/Desktop/CLion/HRJrenderer/obj/african_head.obj");
//    std::vector<int> face = model.facet();
//    for (int i=0; i<model.nfaces(); i++) {
//        for (int j=0; j<3; j++) {
//
//            vec4 v1={(model.vert(face[i*3+j]).x),model.vert(face[i*3+j]).y,model.vert(face[i*3+j]).z,1};
//
//            vec4 v2={model.vert(face[i*3+(j+1)%3]).x,model.vert(face[i*3+(j+1)%3]).y,model.vert(face[i*3+(j+1)%3]).z,1};
//
//
//            v1=projection*modelView*v1;
//            v2=projection*modelView*v2;
//            v1=viewport*(v1/v1[3]);
//            v2=viewport*(v2/v2[3]);
//
//
////            vec4 r1=lookAt*projection*v1;
////            vec4 r2=lookAt*projection*v2;
////            vec4 r1=v1;
////            vec4 r2=v2;
//            //std::cout<<"hhh:"<<r1[0]<<" "<<v1[0]<<" "<<r1[0]/r1[3]<<" ";
//
//            LineDrawing(v1[0],v1[1], v2[0], v2[1], image, red);
//        }
//
//    }
//}
//
//
//void drawModelColor(TGAImage &image, TGAImage &texture){
//    Model model("/Users/huangruijia/Desktop/CLion/HRJrenderer/obj/african_head.obj");
//
//    std::vector<int> face = model.facet();
//    float zbuffer[width*height];
//    for(int i =0; i < width*height;i++){
//        zbuffer[i]=std::numeric_limits<float>::min();
//    }
//    for (int i=0; i<model.nfaces(); i++) {
//        // std::cout<<" h";
//        vec3 world_coords[3];
//        vec3 screen_coords[3];
//
//        for (int j=0; j<3; j++) {
//
//            world_coords[j] =  model.vert(face[i*3+j]);
////
////            vec4 orig = vec4{world_coords[j][0],world_coords[j][1],world_coords[j][2],1};
////            vec4 re = projection*orig;
////            screen_coords[j] = vec3 ((world_coords[j].x/re[3]+1)*width/2,(world_coords[j].y/re[3]+1)*height/2,world_coords[j].z/re[3]+1);
//            screen_coords[j] = vec3 ((int)((world_coords[j].x+1)*width/2),(int)((world_coords[j].y+1)*height/2),world_coords[j].z+1);
//        }
//        vec3 light_dir = {0,0,-1};
//        vec3 n = (world_coords[2]-world_coords[0])^(world_coords[1]-world_coords[0]);
//        n.normalize();
//        float intensity = n*light_dir;
//
//        if (intensity>0) {
//            //   std::cout<<intensity<<" h";
//            draw_triangle(screen_coords, image, TGAColor(intensity*255, intensity*255, intensity*255, 255), zbuffer);
//        }
//        //  draw_triangle(screen_coords, image,model, zbuffer);
//
//    }
//}
//
