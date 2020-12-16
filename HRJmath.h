////
//// Created by 黄睿佳 on 2020/12/16.
////
//
//#ifndef HRJRENDERER_HRJMATH_H
//#define HRJRENDERER_HRJMATH_H
//
//
//#include <cmath>
//#include <cassert>
//#include <iostream>
//
//template<int n, typename T> struct vec{
//    T data[n];
//
//    vec()=default;
//    vec(const vec<n,T> &in):data(in){}
//
//    T& operator [](const int i){ assert(i>=0&&i<n); return data[i];}
//    T operator [](const int i) const { assert(i>=0&&i<n); return data[i];}
//
//    T norm2(){return (*this)*(*this);}
//    T norm(){return std::sqrt((*this)*(*this));}
//
//};
//
//template<typename T> struct vec<2, T>{
//    T x,y;
//    vec()=default;
//    vec(T inx, T iny):x(inx), y(iny){}
//    T& operator [](const int i){ assert(i>=0&&i<2); return i==0?x:y;}
//    T operator [](const int i) const { assert(i>=0&&i<2); return i==0?x:y;}
//
//    T norm2(){return (*this)*(*this);}
//    T norm(){return std::sqrt((*this)*(*this));}
//    vec & normalize() { *this = (*this)/norm(); return *this; }
//};
//
//template<typename T> struct vec<3, T>{
//    T x,y,z;
//    vec()=default;
//    vec(T inx, T iny, T inz):x(inx), y(iny),z(inz){}
//    T& operator [](const int i){ assert(i>=0&&i<3); return i==0 ? x : (1==i ? y : z);}
//    T operator [](const int i) const { assert(i>=0&&i<2); return i==0 ? x : (1==i ? y : z);}
//
//    T norm2(){return (*this)*(*this);}
//    T norm(){return std::sqrt((*this)*(*this));}
//    vec & normalize() { *this = (*this)/norm(); return *this; }
//};
//
//////////////////////////////////////////////////////////////////////////////////////////
//
//template<int n, typename T> vec<n, T> operator+(const vec<n, T>& lhs, const vec<n, T>& rhs) {
//    vec<n, T> ret = lhs;
//    for (int i=n; i--; ret[i]+=rhs[i]);
//    return ret;
//}
//
//template<int n, typename T> vec<n, T> operator-(const vec<n, T>& lhs, const vec<n, T>& rhs) {
//    vec<n, T> ret = lhs;
//    for (int i=n; i--; ret[i]-=rhs[i]);
//    return ret;
//}
//
//template<int n, typename T> double operator*(const vec<n, T>& lhs, const vec<n, T>& rhs) {
//    T ret = 0;
//    for (int i=n; i--; ret+=lhs[i]*rhs[i]);
//    return ret;
//}
//
//template<int n, typename T> vec<n, T> operator*(const double& rhs, const vec<n, T> &lhs) {
//    vec<n, T> ret = lhs;
//    for (int i=n; i--; ret[i]*=rhs);
//    return ret;
//}
//
//template<int n, typename T> vec<n, T> operator*(const vec<n, T>& lhs, const double& rhs) {
//    vec<n, T> ret = lhs;
//    for (int i=n; i--; ret[i]*=rhs);
//    return ret;
//}
//
//template<int n, typename T> vec<n, T> operator/(const vec<n, T>& lhs, const double& rhs) {
//    vec<n, T> ret = lhs;
//    for (int i=n; i--; ret[i]/=rhs);
//    return ret;
//}
//
//
//template<int n, typename T> std::ostream& operator<<(std::ostream& out, const vec<n, T>& v) {
//    for (int i=0; i<n; i++) out << v[i] << " ";
//    return out;
//}
//
//template<typename T> vec<3,T> operator^(const vec<3, T>& lhs, const vec<3, T>& rhs){
//    vec<3,T> ret = vec<3,T>(lhs[1]*rhs[2]-lhs[2]*rhs[1],lhs[2]*rhs[0]-lhs[0]*rhs[2],lhs[0]*rhs[1]-lhs[1]*rhs[0]);
//    return ret;
//}
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//typedef vec<2, int> vec2i;
//typedef vec<3, int> vec3i;
//typedef vec<2, float> vec2f;
//typedef vec<3, float> vec3f;
//#endif //HRJRENDERER_HRJMATH_H
