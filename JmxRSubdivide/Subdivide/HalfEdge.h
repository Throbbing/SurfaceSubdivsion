#pragma once
#include"../Config/JmxRConfig.h"
#include"../Math/Vector.h"
#include"../Editor/IMGUIDx11.h"
#include"../JmxREX/JmxREX.h"
#include<memory>
namespace jmxRCore
{
	using VertexIndex = s32;
	using EdgeIndex = s32;
	using FaceIndex = s32;
	struct Face
	{
		VertexIndex v0Index;
		VertexIndex v1Index;
		VertexIndex v2Index;
	};

	struct HEVertex
	{
		Vec3		vertex;
		EdgeIndex	edgeIndexOrign = -1;	//以该顶点为起始顶点的边的索引
	};


	struct HEEdge
	{
		VertexIndex origin;				//起始顶点 索引
		EdgeIndex	pair;				//同顶点反向边 索引
		FaceIndex	faceIndex;			//面索引
		EdgeIndex	next;				//同面下一条边索引
		EdgeIndex	prev;				//同面上一条边索引
	};

	struct HEFace
	{
		EdgeIndex edgeIndex;
	};


	
	struct HEMesh
	{
		
		std::vector<HEVertex>	vertices;
		std::vector<HEEdge>		edges;
		std::vector<HEFace>		faces;

		//判断一个顶点是否在边界边上
		EdgeIndex isOnBoundary(VertexIndex vIndex);
		//返回边界边所在面的顶点
		void getBoundaryVertices(VertexIndex vIndex,
			EdgeIndex eIndex,
			VertexIndex* v0,
			VertexIndex* v1,
			VertexIndex* v2);
		//获取一个内部顶点所有相邻顶点
		std::vector<VertexIndex> getInteriorNeighVertices(VertexIndex);

		//获取一条边的两个顶点
		void getEndpoint(EdgeIndex eIndex,
			VertexIndex* v0,
			VertexIndex* v1);

		//判断一条边是否是边界边
		bool edgeOnBoundary(EdgeIndex eIndex) 
		{
			return edges[eIndex].pair == -1;
		}

		//获取某条边所在面的三个顶点中，不属于边端点的那个顶点(按照绕序)
		VertexIndex getExtraVertex(EdgeIndex eIndex);

		//获取共享某条内部边的两个三角形的额外两个顶点
		void getExtraVertex(EdgeIndex eIndex,VertexIndex* v0,VertexIndex* v1);

		//找到两个顶点的边
		EdgeIndex getEdge(VertexIndex v0, VertexIndex v1)
		{
			for (s32 e = 0; e < edges.size(); ++e)
			{
				VertexIndex t0, t1;
				getEndpoint(e, &t0, &t1);
				if (t0 == v0 &&t1 == v1)
					return e;
			}
			assert(false);
			return -1;
		}
	};

	struct RenderMesh
	{
		std::vector<Vec3>		vertices;
		std::vector<u32>		indices;

		ID3D11Buffer*			vb;
		ID3D11Buffer*			ib;
	};

	class WRL
	{
	public:
		static bool loadWRL(const std::string& name,
			std::vector<Vec3>& vertices,
			std::vector<Face>& faces);

		static std::shared_ptr<HEMesh> wrl2HE(const std::vector<Vec3>& vertices,
			const std::vector<Face>& faces);
	};


	class RoamCamera
	{
	public:
		struct Frustum
		{
			float NearZ;
			float FarZ;
			float Aspect;
			float FovY;
			float NearWindowHeight;
			float NearWindowWidth;
			float FarWindowHeight;
			float FarWindowWidth;
		};
		RoamCamera(const Vec3& look,
			const Vec3& up,
			const Vec3& pos, f32 nearZ, f32 farZ, s32 width, s32 height);
		~RoamCamera() {}




		Frustum		getFrustum() const { return mFrustum; }

		f32			getNearZ() const { return mFrustum.NearZ; }
		f32			getFarZ() const { return mFrustum.FarZ; }
		f32			getFovY() const { return mFrustum.FovY; }
		f32			getAspect() const { return mFrustum.Aspect; }

		f32			getNearWindowHeight() const { return mFrustum.NearWindowHeight; }
		f32			getNearWindowWidth()  const { return mFrustum.NearWindowWidth; }
		f32			getFarWindowHeight() const { return mFrustum.FarWindowHeight; }
		f32			getFarWindowWidth()	 const { return mFrustum.FarWindowWidth; }
		XMMATRIX	getView() const { return XMLoadFloat4x4(&mView); }
		XMMATRIX	getProj() const { return XMLoadFloat4x4(&mProj); }
		XMMATRIX	getViewProj() const { return getView()*getProj(); }

		void		setPos(float x, float y, float z) { mPos = Vec3(x, y, z); }
		void		setPos(const Vec3& pos) { mPos = pos; }

		void		setTarget(const Vec3& pos, const Vec3& target, const Vec3& up);

		//666666666666666666666666666666666666666666666666666

		void		setLens(float fovY, float aspect, float nearZ, float farZ);


		void		updateMatrix();

		Vec3		getLook() { return mLook; }
		Vec3		getUp() { return mUp; }
		Vec3		getPos() { return mPos; }

		//look 方向平移
		void		walk(float dt);
		//right方向平移
		void		strafe(float dt);

		//绕right轴
		void		pitch(float angle);

		//绕up轴
		void		yaw(float angle);

		//绕y轴
		void		rotateY(float angle);

	private:

		Vec3		mPos;
		Vec3		mLook;
		Vec3		mUp;
		Vec3		mRight;

		Frustum		mFrustum;

		XMFLOAT4X4	mView;
		XMFLOAT4X4	mProj;
	};


	class SubdivideLayout :public jmxRGUILayout
	{
	public:
		SubdivideLayout();
		virtual ~SubdivideLayout();
		virtual void run();

	private:
		
		void loadMesh();
		void drawMesh();


		void loop(std::shared_ptr<HEMesh>& heMesh,
			std::shared_ptr<RenderMesh> renderMesh);

		void butterfly(std::shared_ptr<HEMesh>& heMesh,
			std::shared_ptr<RenderMesh> renderMesh);
		
		Vec3 butterflyHelp(VertexIndex v0, const std::vector<VertexIndex>& neighV0,
			VertexIndex v1,const std::vector<VertexIndex>& neighV1,
			EdgeIndex e,
			const std::vector<HEVertex>& oldVertices,
			std::shared_ptr<HEMesh> heMesh);
		


		
		ID3D11VertexShader*			mVS;
		ID3D11PixelShader*			mPS;
		ID3D11InputLayout*			mLayout;
		ID3D11Buffer*				mCBTrans;
		ID3D11RasterizerState*		mRS;
		std::shared_ptr<RoamCamera> mCamera;

		std::vector<std::shared_ptr<RenderMesh>>	mRenderMeshs;
		std::vector<std::shared_ptr<HEMesh>>		mHEMeshs;
		std::vector<std::string>					mNames;

		std::vector<std::shared_ptr<RenderMesh>>	mRenderMeshsOri;
		std::vector<std::shared_ptr<HEMesh>>		mHEMeshsOri;


		s32							mDrawMeshIndex = -1;
		s32							mSubdivsionScheme = 0;
		

		
	};
}