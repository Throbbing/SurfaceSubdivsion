#include"HalfEdge.h"

extern s32  gMouseX;
extern s32  gMouseY;
extern s32  gLastMouseX;
extern s32  gLastMouseY;
extern bool gMouseDown;
extern bool gMouseMove;

std::string extractFileName(const std::string & filePath)
{
	u32 head, tail;
	for (s32 i = filePath.size() - 1; i >= 0; --i)
	{
		if (filePath[i] == '.')
			tail = i - 1;
		if (filePath[i] == '/' || filePath[i] == '\\')
		{
			head = i + 1;
			break;
		}
	}
	return filePath.substr(head, tail - head + 1);

}

bool jmxRCore::WRL::loadWRL(const std::string & name, 
	std::vector<Vec3>& vertices,
	std::vector<Face>& faces)
{
	char buffer[256];
	std::ifstream input(name);

	if (!input.is_open())
	{
		//cout << "Error reading file";
		return false;
		//exit(1);
	}

	while (!input.eof())
	{
		input.getline(buffer, 100);

		if (strstr(buffer, "point") != NULL) {
			break;
		}
	}

	while (!input.eof())
	{
		input.getline(buffer, 100);
		float vx, vy, vz;
		if (strstr(buffer, "]") == NULL) {
			sscanf_s(buffer, "%f %f %f", &vx, &vy, &vz);
			//cout << vx << "," << vy << "," << vz << endl;
			Vec3 vertex = Vec3(vx, vy, vz);
			vertices.push_back(vertex);
		}
		else {
			break;
		}
	}

	while (!input.eof())
	{
		input.getline(buffer, 100);
		int v0, v1, v2;
		if (strstr(buffer, "-1") != NULL) {
			sscanf_s(buffer, "%d %d %d -1", &v0, &v1, &v2);
			Face face;
			face.v0Index = v0;
			face.v1Index = v1;
			face.v2Index = v2;
			faces.push_back(face);
		}
	}

	input.close();


	return true;
}

std::shared_ptr<jmxRCore::HEMesh> jmxRCore::WRL::wrl2HE(
	const std::vector<Vec3>& vertices, 
	const std::vector<Face>& faces)
{


	auto heMesh = std::make_shared<HEMesh>();

	
	//处理边
	for (int i = 0; i < faces.size(); ++i) {
		HEEdge edge0, edge1, edge2;

		auto&  face = faces[i];

		//i*3 face
		edge0.origin = face.v0Index;
		edge0.pair = -1;
		edge0.faceIndex = i;
		edge0.next = 3 * i + 1;
		edge0.prev = 3 * i + 2;

		//i*3+1
		edge1.origin = face.v1Index;
		edge1.pair = -1;
		edge1.faceIndex = i;
		edge1.next = 3 * i + 2;
		edge1.prev = 3 * i;

		//i*3+2
		edge2.origin = face.v2Index;
		edge2.pair = -1;
		edge2.faceIndex = i;
		edge2.next = 3 * i;
		edge2.prev = 3 * i + 1;

		heMesh->edges.push_back(edge0);
		heMesh->edges.push_back(edge1);
		heMesh->edges.push_back(edge2);
	}

	//寻找同顶点反向边
	for (int i = 0; i < heMesh->edges.size(); ++i)
	{
		auto& edge = heMesh->edges[i];
//		if (edge.pair != -1)
//			continue;

		for (int j = 0; j < heMesh->edges.size(); ++j)
		{
//			if (j == i)
//				continue;

			auto& edge2 = heMesh->edges[j];

			if (edge.origin == heMesh->edges[edge2.next].origin &&
				heMesh->edges[edge.next].origin == edge2.origin)
			{
				edge.pair = j;
//				edge2.pair = i;
				break;
			}
		}
	}


	//顶点处理
	heMesh->vertices.resize(vertices.size());
	for (int e = 0; e < heMesh->edges.size(); ++e)
	{
		auto& edge = heMesh->edges[e];
		heMesh->vertices[edge.origin].vertex = vertices[edge.origin];
		heMesh->vertices[edge.origin].edgeIndexOrign = e;

	}


	//面处理
	//因为边是按照面的顺序存放在数组中
	//以面的第一条边代替面 i*3
	heMesh->faces.resize(faces.size());
	for (int i = 0; i<faces.size(); ++i)
	{
		heMesh->faces[i].edgeIndex = i * 3;
	}

	//边，同顶反向边，顶点，面都已经给处理完毕
	return heMesh;
}

//判断顶点是否在边界边上
//若是，返回该顶点所在的边界边
jmxRCore:: EdgeIndex jmxRCore::HEMesh::isOnBoundary(VertexIndex vIndex)
{
	for (int e = 0; e < edges.size(); ++e)
	{
		if (edges[e].origin == vIndex&&edges[e].pair == -1)
			return e;
	}

	return -1;
}

//返回边界边所在面的三个顶点
void jmxRCore::HEMesh::getBoundaryVertices(VertexIndex vIndex, EdgeIndex eIndex, VertexIndex * v0, VertexIndex * v1, VertexIndex * v2)
{
	*v0 = vIndex;
	auto nextEdge = edges[edges[eIndex].next];
	*v1 = nextEdge.origin;
	*v2 = edges[nextEdge.next].origin;


}

//返回内部顶点的所有相邻顶点
std::vector < jmxRCore::VertexIndex > jmxRCore::HEMesh::getInteriorNeighVertices(VertexIndex vIndex)
{
	std::vector <VertexIndex> neighVertices;
	
	for (auto& edge : edges)
	{
		if (edge.origin == vIndex)
		{
			auto& nextEdge = edges[edge.next];
			neighVertices.push_back(nextEdge.origin);
		}
	}
	return neighVertices;
}

void jmxRCore::HEMesh::getEndpoint(EdgeIndex eIndex,
	VertexIndex* v0,
	VertexIndex* v1)
{
	*v0 = edges[eIndex].origin;
	*v1 = edges[edges[eIndex].next].origin;
}

jmxRCore::VertexIndex jmxRCore::HEMesh::getExtraVertex(EdgeIndex eIndex)
{
	VertexIndex v0, v1;
	
	auto nextEdge = edges[eIndex].next;
	assert(edges[edges[eIndex].prev].origin == edges[edges[nextEdge].next].origin);

	return edges[edges[eIndex].prev].origin;
	


}

void jmxRCore::HEMesh::getExtraVertex(EdgeIndex eIndex, VertexIndex* v0, VertexIndex* v1)
{
	*v0 = getExtraVertex(eIndex);
	*v1 = getExtraVertex(edges[eIndex].pair);
}

jmxRCore::SubdivideLayout::SubdivideLayout()
{
	D3D11_INPUT_ELEMENT_DESC input2[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	mVS = (ID3D11VertexShader*)createShaderAndLayout(L"HLSL/SubdivideVS.hlsl",
		nullptr, nullptr,
		"main", "vs_5_0",
		EVs, jmxREX::hw.d3dDevice,
		input2, 1, &mLayout);

	mPS = (ID3D11PixelShader*)createShaderAndLayout(L"HLSL/SubdividePS.hlsl",
		nullptr, nullptr,
		"main", "ps_5_0",
		EPs, jmxREX::hw.d3dDevice);


	mCBTrans = createConstantBuffer(PAD16(sizeof(XMFLOAT4X4)),
		jmxREX::hw.d3dDevice);

	mCamera = std::shared_ptr<RoamCamera>(new RoamCamera(
		Vec3(0, 0, 1),
		Vec3(0, 1, 0),
		Vec3(0, 0, 0),
		0.1f,
		2000.f,
		ImGui::GetIO().DisplaySize.x,
		ImGui::GetIO().DisplaySize.y));

	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_NONE;
	rd.DepthClipEnable = true;
	HR(jmxREX::hw.d3dDevice->CreateRasterizerState(&rd, &mRS));

	
}

jmxRCore::SubdivideLayout::~SubdivideLayout()
{
	ReleaseCom(mVS);
	ReleaseCom(mPS);
	ReleaseCom(mLayout);
	ReleaseCom(mCBTrans);
}

void jmxRCore::SubdivideLayout::run()
{

	static char* schemeName[] = { "Loop","Butterfly" };
	static f32 speed = 20.f;
	f32 dt = jmxREX::timer.deltaTime();
	if (GetAsyncKeyState('W') & 0x8000)
		mCamera->walk(speed*dt);

	if (GetAsyncKeyState('S') & 0x8000)
		mCamera->walk(-speed*dt);

	if (GetAsyncKeyState('A') & 0x8000)
		mCamera->strafe(-speed*dt);

	if (GetAsyncKeyState('D') & 0x8000)
		mCamera->strafe(speed*dt);


	if (gMouseMove)
	{

		f32 dx = XMConvertToRadians(0.25f*static_cast<float>(gMouseX - gLastMouseX));
		f32 dy = XMConvertToRadians(0.25f*static_cast<float>(gMouseY - gLastMouseY));
		mCamera->pitch(dy);
		mCamera->rotateY(dx);


		gLastMouseX = gMouseX;
		gLastMouseY = gMouseY;

		gMouseMove = false;

	}

	mCamera->updateMatrix();


	if (!ImGui::Begin("Op",nullptr,ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::End();
		return;
	}

	if (ImGui::Button("Load WRL Mesh"))
	{
		loadMesh();
	}

	

	if (!mRenderMeshs.empty())
		mDrawMeshIndex = 0;

	
	ImGui::Combo("Scheme", &mSubdivsionScheme, schemeName, 2);

	if (ImGui::Button("Subdivide"))
	{
		if (mSubdivsionScheme == 0)
			loop(mHEMeshs[mDrawMeshIndex],
				mRenderMeshs[mDrawMeshIndex]);
		else
			butterfly(mHEMeshs[mDrawMeshIndex],
				mRenderMeshs[mDrawMeshIndex]);
	}

	drawMesh();




	ImGui::End();
}

void jmxRCore::SubdivideLayout::loadMesh()
{
	OPENFILENAME ofn = { 0 };
	TCHAR strFilename[MAX_PATH] = { 0 };
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = TEXT("Text Files\0*.wrl\0\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = strFilename;
	ofn.nMaxFile = sizeof(strFilename);
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = TEXT("选择WRL文件");
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
	if (GetOpenFileName(&ofn))
	{
		auto filePath = WstringToString(strFilename);
		std::ifstream fin(filePath);

		std::vector<Vec3> vertices;
		std::vector<Face> faces;
		
		WRL::loadWRL(filePath,
			vertices,
			faces);


		auto renderMesh = std::make_shared<RenderMesh>();
		auto heMesh = WRL::wrl2HE(vertices, faces);
		for (auto& f : faces)
		{
			renderMesh->indices.push_back(f.v0Index);
			renderMesh->indices.push_back(f.v1Index);
			renderMesh->indices.push_back(f.v2Index);
		}
		renderMesh->vertices = vertices;
		renderMesh->vb = createBuffer(D3D11_USAGE_DEFAULT,
			D3D11_BIND_VERTEX_BUFFER,
			sizeof(Vec3)*renderMesh->vertices.size(),
			0, 0, 0, &renderMesh->vertices[0],
			jmxREX::hw.d3dDevice);

		renderMesh->ib = createBuffer(D3D11_USAGE_DEFAULT,
			D3D11_BIND_INDEX_BUFFER,
			sizeof(u32)*renderMesh->indices.size(),
			0, 0, 0, &renderMesh->indices[0],
			jmxREX::hw.d3dDevice);

		mRenderMeshs.push_back(renderMesh);
		mHEMeshs.push_back(heMesh);
		mNames.push_back(extractFileName(filePath));
		
	}
}

void jmxRCore::SubdivideLayout::drawMesh()
{
	if (mDrawMeshIndex < 0)
		return;


	auto dc = jmxREX::hw.d3dImmediateContext;
	auto rtv = jmxREX::hw.renderTargetView;
	auto dsv = jmxREX::hw.depthStencilView;



	{
		f32 silver[] = { 0.5f,0.5f,0.5f,0.f };
		dc->ClearRenderTargetView(rtv, silver);
		dc->ClearDepthStencilView(dsv,
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
			1.f, 0);

		dc->OMSetRenderTargets(1, &rtv, dsv);
	}

	XMMATRIX world = XMMatrixRotationY(PI/2);
	XMMATRIX vp = mCamera->getViewProj();
	D3D11_MAPPED_SUBRESOURCE ms;
	dc->Map(mCBTrans, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	XMFLOAT4X4* msVP = (XMFLOAT4X4*)ms.pData;
	XMStoreFloat4x4(msVP, XMMatrixTranspose(world*vp));
	dc->Unmap(mCBTrans, 0);


	dc->OMSetRenderTargets(1, &rtv, dsv);

	dc->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dc->IASetInputLayout(mLayout);
	u32 stride = sizeof(XMFLOAT3);
	u32 offset = 0;

	dc->VSSetShader(mVS, nullptr, 0);
	dc->PSSetShader(mPS, nullptr, 0);
	dc->RSSetState(mRS);

	dc->VSSetConstantBuffers(0, 1, &mCBTrans);

	auto vb = mRenderMeshs[mDrawMeshIndex]->vb;
	auto ib = mRenderMeshs[mDrawMeshIndex]->ib;

	dc->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
	dc->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
	dc->DrawIndexed(mRenderMeshs[mDrawMeshIndex]->indices.size(), 0, 0);



	dc->RSSetState(nullptr);
}

//Loop划分
void jmxRCore::SubdivideLayout::loop(std::shared_ptr<HEMesh>& heMesh,
	std::shared_ptr<RenderMesh> renderMesh)
{
	//保存原顶点数据
	std::vector<HEVertex> oldVertices = heMesh->vertices;
	
	//用可渲染格式保存划分后的顶点和面片
	std::vector<Vec3>	newVertices;
	for (auto& v : oldVertices)
		newVertices.push_back(v.vertex);

	std::vector<Face>	newFaces;

	//划分算法是对边的划分
	//由于HE是半边结构，一条边有2条半边对应，所以只需要计算一半数目的边
	std::vector<VertexIndex>   newOddVertexIndices(heMesh->edges.size(), -1);

	//对原始顶点进行移动
#if 1
	for (s32 vIndex = 0; vIndex < heMesh->vertices.size(); ++vIndex)
	{
		
		EdgeIndex boundaryE;
		if ((boundaryE = heMesh->isOnBoundary(vIndex)) != -1)
		{
			//边界点
			VertexIndex v0Index, v1Index, v2Index;
			heMesh->getBoundaryVertices(vIndex, boundaryE,
				&v0Index, &v1Index, &v2Index);

			auto v0 = oldVertices[v0Index].vertex;
			auto v1 = oldVertices[v1Index].vertex;
			auto v2 = oldVertices[v2Index].vertex;

			newVertices[vIndex] = 0.75f*v0 + 0.125f*(v1 + v2);
		}
		else
		{
			//内部点
			auto neighVertices = heMesh->getInteriorNeighVertices(vIndex);
			auto n = neighVertices.size();
			auto t = (3.f / 8 + 0.25f*std::cosf(2 * PI / n));
			auto beta = (5.f / 8.f - t*t) / n;

			auto v0 = oldVertices[vIndex].vertex;

			newVertices[vIndex] = (1.f - n*beta)*v0;
			for (auto& v : neighVertices)
			{
				newVertices[vIndex] += beta*oldVertices[v].vertex;
			}
		}

	}


#endif
#if 1
	//每条边生成一个新的顶点
	for (int e = 0; e < heMesh->edges.size(); ++e)
	{
		if (newOddVertexIndices[e] != -1)
			continue;

		if (heMesh->edgeOnBoundary(e))
//		if(true)
		{
			Vec3 newVec;
			newVec = (oldVertices[heMesh->edges[e].origin].vertex + 
				oldVertices[heMesh->edges[heMesh->edges[e].next].origin].vertex) / 2;

			newOddVertexIndices[e] = newVertices.size();
			newVertices.push_back(newVec);
		}
		else {
			Vec3 newVec;

			VertexIndex v0, v1, v2, v3;
			heMesh->getEndpoint(e, &v0, &v1);
			heMesh->getExtraVertex(e, &v2, &v3);

			newVec = 3.f*(oldVertices[v0].vertex + oldVertices[v1].vertex) / 8.f +
				1.f*(oldVertices[v2].vertex + oldVertices[v3].vertex) / 8.f;

			newOddVertexIndices[e] = newVertices.size();
			newOddVertexIndices[heMesh->edges[e].pair] = newVertices.size();
			newVertices.push_back(newVec);
		}
	}

	//生成新面片
	for (int i = 0; i < heMesh->faces.size(); i++)
	{
		auto edgeIndex = heMesh->faces[i].edgeIndex;
		VertexIndex newVertex0, newVertex1, newVertex2;
		heMesh->getEndpoint(edgeIndex, &newVertex0, &newVertex1);
		newVertex2 = heMesh->getExtraVertex(edgeIndex);

		auto oddIndex0 = newOddVertexIndices[edgeIndex];
		auto oddIndex1 = newOddVertexIndices[heMesh->edges[edgeIndex].next];
		auto oddIndex2 = newOddVertexIndices[heMesh->edges[edgeIndex].prev];

		Face face0 = { newVertex0, oddIndex0, oddIndex2 };
		Face face1 = { oddIndex0, newVertex1, oddIndex1 };
		Face face2 = { oddIndex0, oddIndex1, oddIndex2 };
		Face face3 = { oddIndex2, oddIndex1, newVertex2 };

		newFaces.push_back(face0);
		newFaces.push_back(face1);
		newFaces.push_back(face2);
		newFaces.push_back(face3);
	}
#else
	
#endif
	//生成新的网格
	heMesh = WRL::wrl2HE(newVertices,
		newFaces);


	ReleaseCom(renderMesh->vb);
#if 1
	ReleaseCom(renderMesh->ib);
	
	std::vector<u32> indices;
	for (auto& face : newFaces)
	{
		indices.push_back(face.v0Index);
		indices.push_back(face.v1Index);
		indices.push_back(face.v2Index);
	}
	renderMesh->indices = indices;
#endif
	renderMesh->vertices = newVertices;
	renderMesh->vb = createBuffer(D3D11_USAGE_DEFAULT,
		D3D11_BIND_VERTEX_BUFFER,
		sizeof(Vec3)*renderMesh->vertices.size(),
		0, 0, 0, &renderMesh->vertices[0],
		jmxREX::hw.d3dDevice);

#if 1
	renderMesh->ib = createBuffer(D3D11_USAGE_DEFAULT,
		D3D11_BIND_INDEX_BUFFER,
		sizeof(u32)*renderMesh->indices.size(),
		0, 0, 0, &renderMesh->indices[0],
		jmxREX::hw.d3dDevice);
#endif
}

//bufferfly划分
void jmxRCore::SubdivideLayout::butterfly(std::shared_ptr<HEMesh>& heMesh, std::shared_ptr<RenderMesh> renderMesh)
{

	//保存原顶点数据
	std::vector<HEVertex> oldVertices = heMesh->vertices;

	//用可渲染格式保存划分后的顶点和面片
	std::vector<Vec3>	newVertices;
	for (auto& v : oldVertices)
		newVertices.push_back(v.vertex);

	std::vector<Face>	newFaces;

	//划分算法是对边的划分
	//由于HE是半边结构，一条边有2条半边对应，所以只需要计算一半数目的边
	std::vector<VertexIndex>   newOddVertexIndices(heMesh->edges.size(),-1);

	//生成新顶点
	for (s32 e = 0; e < heMesh->edges.size(); ++e)
	{
		if (newOddVertexIndices[e] != -1)
			continue;

		//当边在为边界边
		if (heMesh->edgeOnBoundary(e))
		{
			//b-a-x-a-b
			VertexIndex v0Index, v1Index;
			heMesh->getEndpoint(e, &v0Index, &v1Index);
			
			auto neighV0 = heMesh->getInteriorNeighVertices(v0Index);
			auto neighV1 = heMesh->getInteriorNeighVertices(v1Index);

			auto b0 = neighV0[0];
			VertexIndex b1;
			for (s32 i = 0; i < neighV1.size(); ++i)
			{
				if (neighV1[i] != b0)
				{
					b1 = b0;
					break;
				}
			}

			auto a0 = v0Index;
			auto a1 = v1Index;

			Vec3 newVertex = ((oldVertices[a0].vertex + oldVertices[a1].vertex)*9.f / 16) -
				(oldVertices[b0].vertex + oldVertices[b1].vertex) / 16.f;

			newOddVertexIndices[e] = newVertices.size();
			newVertices.push_back(newVertex);
			
			
			
		}
		else
		{
			//内部点
#if 1
			VertexIndex v0Index, v1Index;
			heMesh->getEndpoint(e, &v0Index, &v1Index);
			auto neighV0 = heMesh->getInteriorNeighVertices(v0Index);
			auto neighV1 = heMesh->getInteriorNeighVertices(v1Index);

			s32 sixCount = 0;
			if (neighV0.size() == 6) sixCount++;
			if (neighV1.size() == 6) sixCount++;

			if (sixCount == 2)
			{
				auto a0 = v0Index;
				auto a1 = v1Index;

				VertexIndex b0, b1;
				VertexIndex c[4];
				heMesh->getExtraVertex(e, &b0, &b1);

				EdgeIndex es[4];
				es[0] = heMesh->getEdge(a0, b0);
				es[1] = heMesh->getEdge(a0, b1);
				es[2] = heMesh->getEdge(a1, b0);
				es[3] = heMesh->getEdge(a1, b1);

				Vec3 newVertex = (oldVertices[a0].vertex + oldVertices[a1].vertex)*0.5f +
					(oldVertices[b0].vertex + oldVertices[b1].vertex)*0.125f;
				for (int i = 0; i < 4; ++i)
				{
					VertexIndex t0, t1;
					heMesh->getExtraVertex(es[i], &t0, &t1);
					if (t0 == a0 || t0 == a1)
						c[i] = t1;
					else
						c[i] = t0;

					newVertex -= oldVertices[c[i]].vertex / 16.f;
				}
				newOddVertexIndices[e] = newVertices.size();
				newOddVertexIndices[heMesh->edges[e].pair] = newVertices.size();
				newVertices.push_back(newVertex);
			}
			else if (sixCount == 1)
			{
				Vec3 newVertex;
				if (neighV0.size() != 6)
				{
					newVertex = butterflyHelp(v0Index, neighV0,
						v1Index, neighV1,
						e, oldVertices, heMesh);

					newOddVertexIndices[e] = newVertices.size();
					newOddVertexIndices[heMesh->edges[e].pair] = newVertices.size();
					newVertices.push_back(newVertex);
				}
				else
				{
					newVertex = butterflyHelp(v1Index, neighV1,
						v0Index, neighV1,
						heMesh->edges[e].pair,
						oldVertices, heMesh);

					newOddVertexIndices[e] = newVertices.size();
					newOddVertexIndices[heMesh->edges[e].pair] = newVertices.size();
					newVertices.push_back(newVertex);
				}

			}
			else
			{
				auto newVertex0 = butterflyHelp(v0Index, neighV0,
					v1Index, neighV1,
					e, oldVertices, heMesh);

				auto newVertex1 = butterflyHelp(v1Index, neighV1,
					v0Index, neighV1,
					heMesh->edges[e].pair,
					oldVertices, heMesh);

				auto newVertex = (newVertex0 + newVertex1)*0.5f;

				newOddVertexIndices[e] = newVertices.size();
				newOddVertexIndices[heMesh->edges[e].pair] = newVertices.size();
				newVertices.push_back(newVertex);
			}

#else
			VertexIndex v0Index, v1Index;
			heMesh->getEndpoint(e, &v0Index, &v1Index);
			Vec3 newVertex = 0.5f*(oldVertices[v0Index].vertex + oldVertices[v1Index].vertex);
			
			
			newOddVertexIndices[e] = newVertices.size();
			newOddVertexIndices[heMesh->edges[e].pair] = newVertices.size();
			newVertices.push_back(newVertex);
			
#endif
			
		}




	}


	//生成新面片
	for (int i = 0; i < heMesh->faces.size(); i++)
	{
		auto edgeIndex = heMesh->faces[i].edgeIndex;
		VertexIndex newVertex0, newVertex1, newVertex2;
		heMesh->getEndpoint(edgeIndex, &newVertex0, &newVertex1);
		newVertex2 = heMesh->getExtraVertex(edgeIndex);

		auto oddIndex0 = newOddVertexIndices[edgeIndex];
		auto oddIndex1 = newOddVertexIndices[heMesh->edges[edgeIndex].next];
		auto oddIndex2 = newOddVertexIndices[heMesh->edges[edgeIndex].prev];

		Face face0 = { newVertex0, oddIndex0, oddIndex2 };
		Face face1 = { oddIndex0, newVertex1, oddIndex1 };
		Face face2 = { oddIndex0, oddIndex1, oddIndex2 };
		Face face3 = { oddIndex2, oddIndex1, newVertex2 };

		newFaces.push_back(face0);
		newFaces.push_back(face1);
		newFaces.push_back(face2);
		newFaces.push_back(face3);
	}

	//生成新的网格
	heMesh = WRL::wrl2HE(newVertices,
		newFaces);


	ReleaseCom(renderMesh->vb);
#if 1
	ReleaseCom(renderMesh->ib);

	std::vector<u32> indices;
	for (auto& face : newFaces)
	{
		indices.push_back(face.v0Index);
		indices.push_back(face.v1Index);
		indices.push_back(face.v2Index);
	}
	renderMesh->indices = indices;
#endif
	renderMesh->vertices = newVertices;
	renderMesh->vb = createBuffer(D3D11_USAGE_DEFAULT,
		D3D11_BIND_VERTEX_BUFFER,
		sizeof(Vec3)*renderMesh->vertices.size(),
		0, 0, 0, &renderMesh->vertices[0],
		jmxREX::hw.d3dDevice);

#if 1
	renderMesh->ib = createBuffer(D3D11_USAGE_DEFAULT,
		D3D11_BIND_INDEX_BUFFER,
		sizeof(u32)*renderMesh->indices.size(),
		0, 0, 0, &renderMesh->indices[0],
		jmxREX::hw.d3dDevice);
#endif
	
}

jmxRCore::Vec3 jmxRCore::SubdivideLayout::butterflyHelp(VertexIndex v0, 
	const std::vector<VertexIndex>& neighV0,
	VertexIndex v1, 
	const std::vector<VertexIndex>& neighV1, 
	EdgeIndex e,
	const std::vector<HEVertex>& oldVertices,
	std::shared_ptr<HEMesh> heMesh)
{
	if (neighV0.size() == 3)
	{
		Vec3 newVertex = 0.75f*oldVertices[v0].vertex +
			5.f*oldVertices[v1].vertex / 12.f;

		for (auto v : neighV0)
		{
			if (v == v1)
				continue;

			newVertex -= oldVertices[v].vertex / 12.f;
		}
		return newVertex;
	}
	else if (neighV0.size() == 4)
	{
		Vec3 newVertex = 0.75f*oldVertices[v0].vertex +
			3.f*oldVertices[v1].vertex / 8.f;

		auto e1 = heMesh->getExtraVertex(e);
		auto e2 = heMesh->getExtraVertex(heMesh->edges[heMesh->edges[e].prev].pair);
		newVertex -= oldVertices[e2].vertex / 8.f;

		return newVertex;

	}
	else
	{
		assert(neighV0.size() >= 5);

		auto n = neighV0.size();
		Vec3 newVertex = 0.75f*oldVertices[v0].vertex +
			(0.25f + 1.f + 0.5f * 1.f) / n *oldVertices[v1].vertex;

		auto nextEdge = e;
		for (int j = 1; j<n; ++j) 
		{
			float beta = (0.25f + std::cos((2 * PI * j) / n) + 0.5f * std::cos((4.f * PI * j) / n)) / n;

			newVertex += beta*oldVertices[heMesh->getExtraVertex(nextEdge)].vertex;

			nextEdge = heMesh->edges[heMesh->edges[nextEdge].prev].pair;
		}
		return newVertex;




	}
}






jmxRCore::RoamCamera::RoamCamera(const Vec3 & look,
	const Vec3 & up,
	const Vec3 & pos, f32 nearZ, f32 farZ, s32 width, s32 height) :
	mLook(look), mUp(up), mPos(pos)
{
	mRight = normalize(cross(up, look));
	setLens(2.f*PI / 9.f, (f32)width / (f32)height, nearZ, farZ);
}


void jmxRCore::RoamCamera::setLens(float fovY, float aspect, float nearZ, float farZ)
{
	mFrustum.FovY = fovY;
	mFrustum.Aspect = aspect;
	mFrustum.NearZ = nearZ;
	mFrustum.FarZ = farZ;

	mFrustum.NearWindowHeight = 2 * nearZ*tanf(fovY*0.5);
	mFrustum.FarWindowHeight = 2 * farZ*tanf(fovY*0.5);

	mFrustum.NearWindowWidth = aspect*mFrustum.NearWindowHeight;
	mFrustum.FarWindowWidth = aspect*mFrustum.FarWindowHeight;

	XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
	DirectX::XMStoreFloat4x4(&mProj, P);
}

void jmxRCore::RoamCamera::updateMatrix()
{
	XMVECTOR	pos = XMLoadFloat3((XMFLOAT3*)&mPos);
	XMVECTOR	look = XMLoadFloat3((XMFLOAT3*)&mLook);
	XMVECTOR	up = XMLoadFloat3((XMFLOAT3*)&mUp);
	XMVECTOR	right = XMLoadFloat3((XMFLOAT3*)&mRight);

	look = XMVector3Normalize(look);

	up = XMVector3Normalize(XMVector3Cross(look, right));

	right = XMVector3Normalize(XMVector3Cross(up, look));

	float x = -XMVectorGetX(XMVector3Dot(pos, right));
	float y = -XMVectorGetX(XMVector3Dot(pos, up));
	float z = -XMVectorGetX(XMVector3Dot(pos, look));

	XMStoreFloat3((XMFLOAT3*)&mPos, pos);
	XMStoreFloat3((XMFLOAT3*)&mRight, right);
	XMStoreFloat3((XMFLOAT3*)&mUp, up);
	XMStoreFloat3((XMFLOAT3*)&mLook, look);

	mView(0, 0) = mRight.x;
	mView(1, 0) = mRight.y;
	mView(2, 0) = mRight.z;
	mView(3, 0) = x;

	mView(0, 1) = mUp.x;
	mView(1, 1) = mUp.y;
	mView(2, 1) = mUp.z;
	mView(3, 1) = y;

	mView(0, 2) = mLook.x;
	mView(1, 2) = mLook.y;
	mView(2, 2) = mLook.z;
	mView(3, 2) = z;

	mView(0, 3) = 0;
	mView(1, 3) = 0;
	mView(2, 3) = 0;
	mView(3, 3) = 1.0f;


}

void jmxRCore::RoamCamera::walk(float dt)
{

	XMVECTOR dis = XMVectorReplicate(dt);
	XMVECTOR pos = XMLoadFloat3((XMFLOAT3*)&mPos);

	XMVECTOR look = XMLoadFloat3((XMFLOAT3*)&mLook);

	XMStoreFloat3((XMFLOAT3*)&mPos, XMVectorMultiplyAdd(dis, look, pos));
}

void jmxRCore::RoamCamera::strafe(float dt)
{
	XMVECTOR dis = XMVectorReplicate(dt);
	XMVECTOR pos = XMLoadFloat3((XMFLOAT3*)&mPos);

	XMVECTOR right = XMLoadFloat3((XMFLOAT3*)&mRight);

	XMStoreFloat3((XMFLOAT3*)&mPos, XMVectorMultiplyAdd(dis, right, pos));
}

void jmxRCore::RoamCamera::pitch(float angle)
{
	XMMATRIX r = DirectX::XMMatrixRotationAxis(XMLoadFloat3((XMFLOAT3*)&mRight), angle);

	XMStoreFloat3((XMFLOAT3*)&mUp, XMVector3TransformNormal(XMLoadFloat3((XMFLOAT3*)&mUp), r));
	XMStoreFloat3((XMFLOAT3*)&mLook, XMVector3TransformNormal(XMLoadFloat3((XMFLOAT3*)&mLook), r));

}

void jmxRCore::RoamCamera::yaw(float angle)
{
	XMMATRIX r = DirectX::XMMatrixRotationAxis(XMLoadFloat3((XMFLOAT3*)&mUp), angle);

	XMStoreFloat3((XMFLOAT3*)&mRight, XMVector3TransformNormal(XMLoadFloat3((XMFLOAT3*)&mRight), r));
	XMStoreFloat3((XMFLOAT3*)&mLook, XMVector3TransformNormal(XMLoadFloat3((XMFLOAT3*)&mLook), r));
}

void jmxRCore::RoamCamera::rotateY(float angle)
{
	XMMATRIX r = DirectX::XMMatrixRotationY(angle);

	XMStoreFloat3((XMFLOAT3*)&mRight, XMVector3TransformNormal(XMLoadFloat3((XMFLOAT3*)&mRight), r));
	XMStoreFloat3((XMFLOAT3*)&mLook, XMVector3TransformNormal(XMLoadFloat3((XMFLOAT3*)&mLook), r));
	XMStoreFloat3((XMFLOAT3*)&mUp, XMVector3TransformNormal(XMLoadFloat3((XMFLOAT3*)&mUp), r));
}

void jmxRCore::RoamCamera::setTarget(const Vec3& pos, const Vec3& target, const Vec3& up)
{
	mPos = pos;
	XMVECTOR p = XMLoadFloat3((XMFLOAT3*)&pos);
	XMVECTOR u = XMLoadFloat3((XMFLOAT3*)&up);
	XMVECTOR t = XMLoadFloat3((XMFLOAT3*)&target);
	XMVECTOR l = XMVector3Normalize(XMLoadFloat3((XMFLOAT3*)&(target - pos)));

	XMVECTOR r = XMVector3Cross(u, l);

	XMStoreFloat3((XMFLOAT3*)&mLook, XMVector3Normalize(l));
	XMStoreFloat3((XMFLOAT3*)&mUp, XMVector3Normalize(u));
	XMStoreFloat3((XMFLOAT3*)&mRight, XMVector3Normalize(r));



}