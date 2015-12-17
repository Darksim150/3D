#include "collisions.h"
#include "math.h"
#include "vector.h"


#define EPSILON .00000001
#define CROSS(dest, v1, v2) (dest[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]), dest[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]), dest[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]))
#define DOT(v1, v2) ((v1[0] * v2[0]) + (v1[1] * v2[1]) + (v1[2] * v2[2]))
#define SUB(dest, v1, v2) (dest[0] = v1[0] - v2[0], dest[1] = v1[1] - v2[1], dest[2] = v1[2] - v2[2])


int sphere_sphere_intersection(
    Vec3D c1,float r1,   /*sphere 1*/
    Vec3D c2,float r2    /*sphere 2*/
)
{
    float distance;
    distance = 
    (c1.x - c2.x)*(c1.x - c2.x) +
    (c1.y - c2.y)*(c1.y - c2.y) +
    (c1.z - c2.z)*(c1.z - c2.z);
    if (distance <= ((r1 + r2) * (r1 + r2)))
    {
        return 1;
    }
    return 0;
}

int cube_cube_intersection(
    Cube a,
    Cube b)
{
    if ((a.x > b.x + b.w) || (b.x > a.x + a.w) ||
        (a.y > b.y + b.h) || (b.y > a.y + a.h) ||
        (a.z > b.z + b.d) || (b.z > a.z + a.d))
    {
        return 0;
    }
    return 1;
}

int point_cube_intersection(
    Vec3D point,            /*point*/
    Vec3D box, Vec3D size)  /*cube*/
{
    if ((point.x < box.x) ||
        (point.y < box.y) ||
        (point.z < box.z) ||
        (point.x > box.x + size.x) ||
        (point.y > box.y + size.y) ||
        (point.z > box.z + size.z))
    {
        return 0;
    }
    return 1;
}

int sphere_cube_intersection(
    Vec3D center,float r,   /*sphere*/
    Vec3D box, Vec3D size)  /*axis aligned 3d cube*/
{
    float distance;
    Vec3D offset;
    
    offset.x = -(box.x + (size.x * 0.5));
    offset.y = -(box.y + (size.y * 0.5));
    offset.z = -(box.z + (size.z * 0.5));
    
    vec3d_add(box,box,offset);
    vec3d_add(center,center,offset);
    center.x = fabs(center.x);
    center.y = fabs(center.y);
    center.z = fabs(center.z);
    box.x = fabs(box.x);
    box.y = fabs(box.y);
    box.z = fabs(box.z);
    
    distance = 
        (box.x - center.x)*(box.x - center.x) +
        (box.y - center.y)*(box.y - center.y) +
        (box.z - center.z)*(box.z - center.z);
    
    if (distance <= (r * r))
    {
        /*success, intersection with corner*/
        return 1;
    }
    
    if ((center.x <= box.x + r) &&
        (center.y <= box.y) &&
        (center.z <= box.z))
    {
        return 1;
    }
    if ((center.x <= box.x) &&
        (center.y <= box.y + r) &&
        (center.z <= box.z))
    {
        return 1;
    }
    if ((center.x <= box.x) &&
        (center.y <= box.y) &&
        (center.z <= box.z + r))
    {
        return 1;
    }
    return 0;
}

int RayTriangleCollision(Vec3D origin, Vec3D dir, Vec3D vert0, Vec3D vert1, Vec3D vert2)
{
	Vec3 edge1, edge2, tvec, pvec, qvec;
	float det, inv_det;
	float t, u, v;

	float *o;

	float *d;

	float *v0,*v1,*v2;
		
	v0 = (float *)(&vert0);
	v1 = (float *)(&vert1);
	v2 = (float *)(&vert2);

	o = (float *)(&origin);

	d = (float *)(&dir);

	/* find vectors for two edges sharing vert0*/
	SUB(edge1, v1, v0);
	SUB(edge2, v2, v0);

	/* begin calculating determinant - also used to calculated U parameter */
	CROSS(pvec, d, edge2);

	/* if determinant is near zero, ray lies in plane of triangle*/
	det = DOT(edge1, pvec);
#ifdef TEST_CULL /* Define TEST_CULL if culling */
	if (det < EPSILON)
		return 0;

	/* calculate distance from vert0 to ray origin */
	SUB(tvec, origin, vert0);

	/* calculate U parameter and test bounds*/
	u = DOT(tvec, pvec);

	if (*u < 0.0 || *u > det)
		return 0;

	CROSS(qvec, tvec, edge1);

	/* calculate V parameter and test bounds*/
	v = DOT(dir, qvec);

	if (*v < 0.0 || *u + *v > det)
		return 0;

	/* calculate t, scale parameters, ray intersects triangle*/
	t = DOT(edge2, qvec);
	inv_det = 1.0 / det;
	t *= inv_det;
	u *= inv_det;
	v *= inv_det;
#else /* Non-culling branch*/
	if (det > -EPSILON && det < EPSILON)
		return 0;

	inv_det = 1.0 / det;

	/* calculate distance from vert0 to ray origin */
	SUB(tvec, o, v0);

	/* calculate U parameter and test bounds */
	u = DOT(tvec, pvec) * inv_det;

	if (u < 0.0 || u > 1.0)
		return 0;

	CROSS(qvec, tvec, edge1);

	/* calculate V parameter and test bounds */
	v = DOT(d, qvec) * inv_det;

	if (v < 0.0 || u + v > 1.0)
		return 0;

	/* calculate t, ray intersects triangle */
	t = DOT(edge2, qvec) * inv_det;
#endif
	return 1;
}

/*eol@eof*/
