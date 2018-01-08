/**
*　　　　　　　　┏┓　　　┏┓+ +
*　　　　　　　┏┛┻━━━┛┻┓ + +
*　　　　　　　┃　　　　　　　┃ 　
*　　　　　　　┃　　　━　　　┃ ++ + + +
*　　　　　　 ████━████ ┃+
*　　　　　　　┃　　　　　　　┃ +
*　　　　　　　┃　　　┻　　　┃
*　　　　　　　┃　　　　　　　┃ + +
*　　　　　　　┗━┓　　　┏━┛
*　　　　　　　　　┃　　　┃　　　　　　　　　　　
*　　　　　　　　　┃　　　┃ + + + +
*　　　　　　　　　┃　　　┃　　　　Code is far away from bug with the animal protecting　　　　　　　
*　　　　　　　　　┃　　　┃ + 　　　　神兽保佑,代码无bug　　
*　　　　　　　　　┃　　　┃
*　　　　　　　　　┃　　　┃　　+　　　　　　　　　
*　　　　　　　　　┃　 　　┗━━━┓ + +
*　　　　　　　　　┃ 　　　　　　　┣┓
*　　　　　　　　　┃ 　　　　　　　┏┛
*　　　　　　　　　┗┓┓┏━┳┓┏┛ + + + +
*　　　　　　　　　　┃┫┫　┃┫┫
*　　　　　　　　　　┗┻┛　
*/
#pragma  once
#include"Vector.h"
#include"Math.h"
namespace jmxRCore
{
	class DifferentialGeometry;
	//独立坐标系
	class Frame
	{
	public:
		Frame(const Vec3& xx, const Vec3& yy, const Vec3& zz) :
			X(xx), Y(yy), Z(zz){}
		Frame(const DifferentialGeometry& dg);
		Frame(){}
		~Frame(){}
		
		
		Vec3 toLocal(const Vec3& v) const
		{
			return Vec3(dot(v, X), dot(v, Y), dot(v, Z));
		}
//		Point3 toLocal(const Point3& p) const 
//		{
//			return Point3(dot(p, X), dot(p, Y), dot(p, Z));
//		}
		Normal toLocal(const Normal& n) const 
		{
			return Normal(dot(n, X), dot(n, Y), dot(n, Z));
		}
		Vec3 toWorld(const Vec3& v) const 
		{
			return v.x*X + v.y*Y + v.z*Z;
		}
//		Point3 toWorld(const Point3& p) const 
//		{
//			return Point3(p.x*X + p.y*Y + p.z*Z);
//		}
		Normal toWorld(const Normal& n)
		{
			return Normal(n.x*X + n.y*Y + n.z*Z);
		}

		static f32 cosTheta(const Vec3& v)
		{
			return v.z;
		}

		static f32 absCosTheta(const Vec3& v)
		{
			return std::fabs(v.z);
		}

		static f32 sinTheta2(const Vec3& v)
		{
			f32 cosa=v.z;
			return max(0.f, 1.f - cosa*cosa);
		}

		static f32 sinTheta(const Vec3& v)
		{
			return std::sqrtf(sinTheta2(v));
		}

		static f32 cosPhi(const Vec3& v)
		{
			f32 sinT = sinTheta(v);
			if (sinT == 0.f)return 1.f;
			return clamp(v.x / sinT, -1.f, 1.f);
		}

		static f32 sinPhi(const Vec3& v)
		{
			f32 sinT = sinTheta(v);
			if (sinT == 0.f) return 1.f;
			return clamp(v.y / sinT, -1.f, 1.f);
		}

		static f32 sphericalTheta(const Vec3& v)
		{
			return std::acosf(clamp(v.z, -1.f, 1.f));
		}

		static f32 sphericalPhi(const Vec3& v)
		{
			f32 phi = std::atan2f(v.y, v.x);
			return phi < 0.f ? phi + 2 * PI : phi;
		}

		static void constructFrame(_Para_In const Vec3& v1,
			_Para_Out Vec3* v2,
			_Para_Out Vec3* v3)
		{
			if (std::fabsf(v1.x) > std::fabsf(v1.y)) {
				f32 invLen = 1.f / std::sqrtf(v1.x*v1.x + v1.z*v1.z);
				*v2 = Vec3(-v1.z * invLen, 0.f, v1.x * invLen);
			}
			else {
				f32 invLen = 1.f / std::sqrtf(v1.y*v1.y + v1.z*v1.z);
				*v2 = Vec3(0.f, v1.z * invLen, -v1.y * invLen);
			}
			*v3 = cross(v1, *v2);
		}

		Vec3 X, Y, Z;
		
	};
}