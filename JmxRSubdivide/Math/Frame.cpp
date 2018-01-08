#include"Frame.h"
#include"../Geometry/Geometry.h"


jmxRCore::Frame::Frame(const DifferentialGeometry& dg)
{
	Z = normalize(Vec3(dg.hitNormal));
	X = normalize(Vec3(dg.dpdu));
	Y = normalize(cross(Z, X));
}
