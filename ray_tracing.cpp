#include<bits/stdc++.h>
typedef long long ll;
using namespace std;
// 3 dimentional vector class
class Vec{
public:
  float x,y,z;
  Vec():x(0),y(0),z(0){}
  Vec(float a):x(a),y(a),z(a){}
  Vec(float a,float b,float c):x(a),y(b),z(c){}
  Vec& normalize(){
    float dis=dist();
    if(dis>0){
      x/=dis;
      y/=dis;
      z/=dis;
    }
    return *this;
  }
  float dist(){return sqrt(x*x+y*y+z*z);}
  Vec operator*(const float &a) const {
    return Vec(x*a,y*a,z*a);
  }
  Vec operator*(const Vec& a) const {
    return Vec(x*a.x,y*a.y,z*a.z);
  }
  Vec operator+(const Vec&a ) const {
    return Vec(x+a.x,y+a.y,z+a.z);
  }
  Vec operator-(const Vec&a ) const {
    return Vec(x-a.x,y-a.y,z-a.z);
  }
  Vec operator-() const {
    return Vec(-x,-y,-z);
  }
  Vec& operator+=(const Vec&a){
    x+=a.x;y+=a.y;z+=a.z;
    return *this;
  }
  Vec& operator*=(const Vec&a){
    x*=a.x;y*=a.y;z*=a.z;
    return *this;
  }
  float dotproduct(const Vec &a) const{
    return a.x*x+y*a.y+z*a.z;
  }
  friend ostream& operator<<(ostream &a,const Vec &b){
    a<<"{"<<b.x<<","<<b.y<<","<<b.z<<"}";
    return a;
  }
};
class sphere{
public:
  Vec center;
  float radius;
  Vec surfaceC,emissionC;
  float transparency,reflection;
  sphere(const Vec& c,const float& r,const Vec &sc,const float& ref=0,const float&tr=0, const Vec& ec=0){
    center=c;
    radius=r;
    surfaceC=sc;
    emissionC=ec;
    transparency=tr;
    reflection =ref;
  }
  bool ifintersect(const Vec&rayo,const Vec& raydir,float& t0,float& t1) const {
    // Vec temp=rayo-center;
    // float b=2*raydir.dotproduct(temp);
    // float c=temp.dotproduct(temp)-radius*radius;
    // float D=b*b-4*c;
    // if(D<0){
    //   return false;
    // }
    // else{ // the lengthy calculation produce imperfections
    //   t0=(-b-sqrt(D))/2;
    //   t1=(-b+sqrt(D))/2;
    //   if(t0<0 and t1<0){
    //     return false;
    //   }
    //   return true;
    // }
    // equn of ray= a+tb
    // equation of circle =|p+c|=r*r;
    // change the euation to (t^2)/2-(c-a).b+(c-a).(c-a)/2+ r*r/2 and solve
    Vec l=center-rayo;
    float tca=l.dotproduct(raydir);
    if(tca<0) return false;
    float d2=l.dotproduct(l)-tca*tca;
    if(d2>radius*radius) return false;
    float thc=sqrt(radius*radius-d2);
    t0=tca-thc;
    t1=tca+thc;
    return true;
  }
};
float fresnal(const float& a,const float&b ,const float&m){
  return b*m+a*(1-m);
}
#define recursion_limit 5
Vec trace(const vector<sphere>& spheres,const Vec& rayo,const Vec& raydir,const int& depth){
  float tmin=1e8;
  const sphere* sp=nullptr;
  for(auto &i:spheres){
    float t0,t1;
    if(i.ifintersect(rayo,raydir,t0,t1)){
      if(t0<0){ t0=t1 ;}
      if(t0<tmin){
        tmin=t0;
        sp=&i;
      }
    }
  }
  if(!sp) return Vec(2);
  Vec surfaceC=0;
  Vec intr=rayo +raydir*tmin;//intr = intersection point on the sphere
  Vec norintr=intr-sp->center;//norintr=normal to the surface of the sphere
  norintr.normalize();
  float bias=1e-4; // a bias is added to the point of instersection of the ray to avoid shadow-acne (a rendering flaw)
  bool inside=false;
  if(raydir.dotproduct(norintr)>0){// if the direction of normal is not opposite to the ray it means we're inside the sphere
    inside=true;
    norintr=-norintr;
  }
  if((sp->transparency or sp->reflection) and depth<recursion_limit){
    float facingratio =-raydir.dotproduct(norintr);
    float fresnelef=fresnal(pow((1-facingratio),3),1,0.1);
    Vec refldir=raydir-norintr*2*(raydir.dotproduct(norintr));
    refldir.normalize();
    Vec reflection=trace(spheres,intr+norintr*bias,refldir,depth+1);
    Vec refraction=0;
    if(sp->transparency){
      float ri=1.1;
      float eta=(inside)?ri:1/ri;
      float cosi=-norintr.dotproduct(raydir);
      float z=1-eta*eta*(1-cosi*cosi);
      Vec refrdir = raydir*eta+norintr*(eta*cosi-sqrt(z));
      refrdir.normalize();
      refraction = trace(spheres,intr- norintr*bias, refrdir, depth + 1);
    }
    surfaceC=(reflection*fresnelef +refraction*(1 - fresnelef)*sp->transparency)*sp->surfaceC;
  }
  else{
    for(int h=0;h<spheres.size();h++){
      Vec transmission=1;
      Vec lightdir=spheres[h].center-intr;
      lightdir.normalize();
      for(int j=0;j<spheres.size();j++){
        if(h==j){
          continue;
        }
        float t0,t1;
        if(spheres[j].ifintersect(intr+norintr*bias,lightdir,t0,t1)){
          transmission=0;
          break;
        }
      }
      // if other spheres emit light as well i.e there is more than one light source
      surfaceC+=sp->surfaceC*transmission*(max(float(0),norintr.dotproduct(lightdir)))*spheres[h].emissionC;
    }
  }
  return surfaceC+sp->emissionC;
}
#define PI 3.141592653589793
void img_render(const std::vector<sphere> &spheres)
{
    int width=640,height=480;
    Vec *image=new Vec[width*height], *pixel=image;
    float invWidth=1/float(width),invHeight=1/float(height);
    float fov=60,aspectratio=width/float(height);
    float angle=tan(PI*0.5*fov/180.);
    for (int y=0;y<height;y++) {
        for (int x=0;x<width;x++,pixel++) {
            float xx = (2*((x+0.5)*invWidth) - 1)*angle*aspectratio;
            float yy = (1-2*((y+0.5)*invHeight))*angle;
            Vec raydir(xx,yy,-1);
            raydir.normalize();
            *pixel = trace(spheres,Vec(0), raydir, 0);
        }
    }
    std::ofstream ofs("./render_result.ppm", std::ios::out | std::ios::binary);
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (int i = 0; i < width * height; ++i) {
        ofs << (char)(std::min(float(1), image[i].x) * 255) <<
               (char)(std::min(float(1), image[i].y) * 255) <<
               (char)(std::min(float(1), image[i].z) * 255);
    }
    ofs.close();
    delete [] image;
}
int main(){
  std::vector<sphere> spheres;
  // input data of spheres is hard coded but it can be taken input at run time
  //order-> position, radius, surface color, reflectivity, transparency, emission color
  spheres.push_back(sphere(Vec( 0.0, -10004, -20), 10000, Vec(0.2, 0.2, 0.2), 0, 0.0)); // this sphere forms the base underneath the floating spheres
  spheres.push_back(sphere(Vec( 0.0,      5, -20),     4, Vec(1.00, 0.32, 0.36), 1, 0.5));
  spheres.push_back(sphere(Vec( -2,      4, -27),     4, Vec(0.0, 0.78, 0.0), 1, 0.5));
  spheres.push_back(sphere(Vec( 5.0,     -1, -15),     2, Vec(0.90, 0.76, 0.46), 1, 0.0));
  spheres.push_back(sphere(Vec( 5.0,      0, -25),     3, Vec(0.65, 0.77, 0.97), 1, 0.0));
  spheres.push_back(sphere(Vec(-5.5,      0, -15),     3, Vec(1, 0.71, 0.756), 1, 0.5));
  //spherical light source
  spheres.push_back(sphere(Vec( 0.0,     20, -30),     3, Vec(0), 0, 0.0, Vec(3.529,3.807,3.921)));
  cout<<"the image of spheres has been rendered and stored in the same directory as the program file"<<endl;
  img_render(spheres);
}
