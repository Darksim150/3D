#include "vector.h"
#include "math.h"

Vec2D vec2d(float x,float y)
{
    Vec2D v = {x,y};
    return v;
}

Vec3D vec3d(float x,float y, float z)
{
    Vec3D v = {x,y,z};
    return v;
}

Vec4D vec4d(float x,float y, float z, float w)
{
    Vec4D v = {x,y,z,w};
    return v;
}

Vec3D Normalize(Vec3D vector)
{
	float magnitude = sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
	Vec3D unit;
	unit.x = vector.x/magnitude;
	unit.y = vector.y/magnitude;
	unit.z = vector.z/magnitude;
	return unit;
}

Vec3D vec_scale(Vec3D vec, float scale)
{
	vec.x *= scale;
	vec.y *= scale;
	vec.z *= scale;

	return vec;
}
/*eol@eof*/
