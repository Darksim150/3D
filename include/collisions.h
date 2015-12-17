#ifndef __COLLISIONS_H__
#define __COLLISIONS_H__

#include "vector.h"

typedef struct
{
    float x,y,z;
    float w,h,d;
}Cube;

#define cube_set(cube,a,b,c,l,m,n) (cube.x=a,cube.y=b,cube.z=c,\
                                    cube.w=l,cube.h=m,cube.d=n)

int cube_cube_intersection(
    Cube a,
    Cube b);

typedef float Vec;
typedef Vec Vec3[3];

int RayTriangleCollision(Vec3D origin, Vec3D dir, Vec3D vert0, Vec3D vert1, Vec3D vert2);

#endif
