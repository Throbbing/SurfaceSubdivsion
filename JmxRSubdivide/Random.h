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
#pragma once
#include"Config/JmxRConfig.h"
#include<ctime>
#include<random>
/*
	随机数生成器
*/
//#define pbrtRng
namespace jmxRCore
{
	/*
	
	class Random
	{
	private:
		std::default_random_engine e;
		std::uniform_Para_Int_distribution<unsigned int> u;
		std::uniform_real_distribution<float>   r;
		std::bernoulli_distribution			   b;

		u32 min;
		u32 max;
		Random(const Random&) = delete;
		Random operator=(const Random&) = delete;


	public:
		Random(u32 min = 0, u32 max = 0xffff, u32 seed = 0) :r(0.0f, 1.0f), u(min, max), b(0.5)
		{
			if (seed)
				e.seed(seed);
			else
				e.seed((u32)time(0));

			this->min = min;
			this->max = max;



		}
		u32 getRandU() const
		{

			return u(e);
		}
		u32 getRandUX() 
		{
			return e();
		}
		f32 getRandF() const 
		{

			return r(e);
		}
		bool getRandB() const 
		{
			return b(e);
		}
		void setUdistri(u32 Min, u32 Max)
		{
			min = Min;
			max = Max;

			std::uniform_Para_Int_distribution<unsigned int> temp(min, max);
			u = temp;
		}
		void setFdistri(f32 min, f32 max)
		{
			std::uniform_real_distribution<f32> temp(min,max);
			r = temp;
		}
		void setBdistri(float c)
		{
			if (c<0.0f || c>1.0f)
				return;
			std::bernoulli_distribution temp(c);
			b = temp;
		}
		void setSeed(unsigned int seed)
		{
			e.seed(seed);
		}




	};
	*/

#if defined(pbrtRng)
	


	class Random {
	public:
		Random(u32 s = 5489UL) {
			mti = N + 1; 
			seed(s);
		}

		void seed(u32 seed) const;
		float randomFloat() const;
		u32 randomUInt() const;

	private:
		static const int N = 624;
		mutable unsigned long mt[N]; 
		mutable int mti;
	};


	
#else
	
	class Random
	{
	public:
		Random();
		Random(const Random&) = delete;
		Random& operator=(const Random&) = delete;
		void seed(unsigned int seed) { e.seed(seed); }
		float randomFloat();
		unsigned int randomUInt();
	private:
		std::default_random_engine e;
		std::uniform_int_distribution<unsigned int> ui;
 		std::uniform_real_distribution<float> ur;
	};
	
#endif
}