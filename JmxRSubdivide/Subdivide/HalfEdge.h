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
		EdgeIndex	edgeIndexOrign = -1;	//�Ըö���Ϊ��ʼ����ıߵ�����
	};


	struct HEEdge
	{
		VertexIndex origin;				//��ʼ���� ����
		EdgeIndex	pair;				//ͬ���㷴��� ����
		FaceIndex	faceIndex;			//������
		EdgeIndex	next;				//ͬ����һ��������
		EdgeIndex	prev;				//ͬ����һ��������
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

		//�ж�һ�������Ƿ��ڱ߽����
		EdgeIndex isOnBoundary(VertexIndex vIndex);
		//���ر߽��������Ķ���
		void getBoundaryVertices(VertexIndex vIndex,
			EdgeIndex eIndex,
			VertexIndex* v0,
			VertexIndex* v1,
			VertexIndex* v2);
		//��ȡһ���ڲ������������ڶ���
		std::vector<VertexIndex> getInteriorNeighVertices(VertexIndex);

		//��ȡһ���ߵ���������
		void getEndpoint(EdgeIndex eIndex,
			VertexIndex* v0,
			VertexIndex* v1);

		//�ж�һ�����Ƿ��Ǳ߽��
		bool edgeOnBoundary(EdgeIndex eIndex) 
		{
			return edges[eIndex].pair == -1;
		}

		//��ȡĳ��������������������У������ڱ߶˵���Ǹ�����(��������)
		VertexIndex getExtraVertex(EdgeIndex eIndex);

		//��ȡ����ĳ���ڲ��ߵ����������εĶ�����������
		void getExtraVertex(EdgeIndex eIndex,VertexIndex* v0,VertexIndex* v1);

		//�ҵ���������ı�
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

		//look ����ƽ��
		void		walk(float dt);
		//right����ƽ��
		void		strafe(float dt);

		//��right��
		void		pitch(float angle);

		//��up��
		void		yaw(float angle);

		//��y��
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