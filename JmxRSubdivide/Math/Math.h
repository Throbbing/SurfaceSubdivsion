#pragma once

#include<cmath>
#include<assert.h>
#include"../Config/JmxRConfig.h"



#ifndef max

#define max(a,b) (a>b?a:b)
#endif // !max

#ifndef min

#define min(a,b) (a<b?a:b)

#endif

#ifndef EPSILON
#define EPSILON			1e-3f
#endif

#define PI 3.141592657f
#define PI_2 6.2831852f


namespace jmxRCore
{


	FORCEINLINE f32 angle2Radian(f32 angle)
	{
		return angle / 180.f *PI;
	}

	FORCEINLINE f32 radian2Angle(f32 radian)
	{
		return radian / PI *180.f;
	}

	FORCEINLINE f32 clamp(f32 v, f32 low, f32 high)
	{
		if (v < low) return low;
		else if (v > high) return high;
		else return v;
	}

	FORCEINLINE s32 clamp(s32 v, s32 low, s32 high)
	{
		if (v < low) return low;
		else if (v > high) return high;
		else return v;
	}

	FORCEINLINE f32 lerp(f32 t, f32 a, f32 b)
	{
		return (1.f - t)*a + (t)*b;
	}


	FORCEINLINE f32 frac(f32 n)
	{
		return n - (int)n;
	}


	FORCEINLINE f32 dirac(f32 v)
	{
		return std::fabs(v) < 0.00000001f ? 1.f : 0.f;
	}


	FORCEINLINE bool isPowerOf2(u32 v)
	{
		return v && !(v & (v - 1));
	}

	FORCEINLINE u32	ceilPowerOf2(u32 v)
	{
		u32 r = 1;
		while (r < v) r <<= 1;
		return r;
	}

	FORCEINLINE u32 ceilSquare(u32 v)
	{
		u32 r = 1;
		while(r*r < v)
			++r;

		return r*r;
	}

	FORCEINLINE f32 square(f32 v)
	{
		return v*v;
	}

	//二维矩阵求逆
	inline void Mat2Inv(f32 m[4], f32 inv[4])
	{
		f32 det = m[3] * m[0] - m[1] * m[2];
		if (fabsf(det) < EPSILON)
		{
			inv[0] = 0; inv[1] = 0; inv[2] = 0; inv[3] = 0;
		}
		f32 dd = 1.f / det;
		inv[0] = +dd*m[3];
		inv[1] = -dd*m[1];
		inv[2] = -dd*m[2];
		inv[3] = +dd*m[0];
	}

	//求解一元二次方程
	inline bool equation2(f32 a, f32 b, f32 c, f32* t0 = nullptr, f32* t1 = nullptr)
	{
		f32 dd = b*b - 4 * a*c;
		if (dd < 0)
			return false;
		f32 sqrtd = sqrtf(dd);

		if (fabs(a) < EPSILON)
			return false;

		if (t0)
			*t0 = (-b + sqrtd) / (2 * a);
		if (t1)
			*t1 = (-b - sqrtd) / (2 * a);

		return true;
	}

	inline void quaternian2Euler(f32 x, f32 y, f32 z, f32 w,
		_Para_Out f32* roll, _Para_Out f32* pitch, _Para_Out f32* yaw)
	{
		*roll = std::atan2f(2 * (w*z + x*y), 1.f - 2 * (z*z + x*x));
		*pitch = std::asinf(2 * (w*x - y*z));
		*yaw = std::atan2(2 * (w*y + z*x), 1.f - 2 * (x*x + y*y));
	}


}
