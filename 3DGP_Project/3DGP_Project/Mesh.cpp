#include "Mesh.h"

CMesh::CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

CMesh::~CMesh()
{
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
	if (m_pVertices) delete[] m_pVertices;
	if (m_pnIndices) delete[] m_pnIndices;
}

void CMesh::ReleaseUploadBuffers()
{
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다.
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;
	
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
	m_pd3dIndexUploadBuffer = NULL;
};

void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, UINT nInstances)
{
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, nInstances, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, nInstances, m_nOffset, 0);
	}
}

void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, UINT nInstances,
	D3D12_VERTEX_BUFFER_VIEW d3dInstancingBufferView)
{
	//정점 버퍼 뷰와 인스턴싱 버퍼 뷰를 입력-조립 단계에 설정한다. 
	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[] = { m_d3dVertexBufferView, d3dInstancingBufferView };
	pd3dCommandList->IASetVertexBuffers(m_nSlot, _countof(pVertexBufferViews), pVertexBufferViews);
	Render(pd3dCommandList, nInstances);
}

int CMesh::CheckRayIntersection(XMFLOAT3& xmf3RayOrigin, XMFLOAT3& xmf3RayDirection,
	float* pfNearHitDistance)
{
	int nIntersections = 0;
	BYTE* pbPositions = (BYTE*)m_pVertices;
	int nOffset = (m_d3dPrimitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? 3 : 1;
	
	int nPrimitives = (m_d3dPrimitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ?
		(m_nVertices / 3) : (m_nVertices - 2);
	if (m_nIndices > 0) nPrimitives = (m_d3dPrimitiveTopology ==
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nIndices / 3) : (m_nIndices - 2);
	XMVECTOR xmRayOrigin = XMLoadFloat3(&xmf3RayOrigin);
	XMVECTOR xmRayDirection = XMLoadFloat3(&xmf3RayDirection);
	bool bIntersected = m_xmBoundingBox.Intersects(xmRayOrigin, xmRayDirection, *pfNearHitDistance);
	if (bIntersected)
	{
		float fNearHitDistance = FLT_MAX;

		for (int i = 0; i < nPrimitives; i++)
		{
			XMVECTOR v0 = XMLoadFloat3((XMFLOAT3*)(pbPositions + ((m_pnIndices) ?
				(m_pnIndices[(i * nOffset) + 0]) : ((i * nOffset) + 0)) * m_nStride));
			XMVECTOR v1 = XMLoadFloat3((XMFLOAT3*)(pbPositions + ((m_pnIndices) ?
				(m_pnIndices[(i * nOffset) + 1]) : ((i * nOffset) + 1)) * m_nStride));
			XMVECTOR v2 = XMLoadFloat3((XMFLOAT3*)(pbPositions + ((m_pnIndices) ?
				(m_pnIndices[(i * nOffset) + 2]) : ((i * nOffset) + 2)) * m_nStride));
			float fHitDistance;
			BOOL bIntersected = TriangleTests::Intersects(xmRayOrigin, xmRayDirection, v0,
				v1, v2, fHitDistance);
			if (bIntersected)
			{
				if (fHitDistance < fNearHitDistance)
				{
					*pfNearHitDistance = fNearHitDistance = fHitDistance;
				}
				nIntersections++;
			}
		}
}
	return(nIntersections);
}

CCubeMeshDiffused::CCubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice, pd3dCommandList)
{
	//직육면체는 꼭지점(정점)이 8개이다. 
	m_nVertices = 8;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;

	//정점 버퍼는 직육면체의 꼭지점 8개에 대한 정점 데이터를 가진다. 
	m_pVertices = new CDiffusedVertex[m_nVertices];
	m_pVertices[0] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	m_pVertices[1] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	m_pVertices[2] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	m_pVertices[3] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	m_pVertices[4] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	m_pVertices[5] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	m_pVertices[6] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	m_pVertices[7] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	m_nIndices = 36;
	m_pnIndices = new UINT[m_nIndices];
	//ⓐ 앞면(Front) 사각형의 위쪽 삼각형
	m_pnIndices[0] = 3; m_pnIndices[1] = 1; m_pnIndices[2] = 0;
	//ⓑ 앞면(Front) 사각형의 아래쪽 삼각형
	m_pnIndices[3] = 2; m_pnIndices[4] = 1; m_pnIndices[5] = 3;
	//ⓒ 윗면(Top) 사각형의 위쪽 삼각형
	m_pnIndices[6] = 0; m_pnIndices[7] = 5; m_pnIndices[8] = 4;
	//ⓓ 윗면(Top) 사각형의 아래쪽 삼각형
	m_pnIndices[9] = 1; m_pnIndices[10] = 5; m_pnIndices[11] = 0;
	//ⓔ 뒷면(Back) 사각형의 위쪽 삼각형
	m_pnIndices[12] = 3; m_pnIndices[13] = 4; m_pnIndices[14] = 7;
	//ⓕ 뒷면(Back) 사각형의 아래쪽 삼각형
	m_pnIndices[15] = 0; m_pnIndices[16] = 4; m_pnIndices[17] = 3;
	//ⓖ 아래면(Bottom) 사각형의 위쪽 삼각형
	m_pnIndices[18] = 1; m_pnIndices[19] = 6; m_pnIndices[20] = 5;
	//ⓗ 아래면(Bottom) 사각형의 아래쪽 삼각형
	m_pnIndices[21] = 2; m_pnIndices[22] = 6; m_pnIndices[23] = 1;
	//ⓘ 옆면(Left) 사각형의 위쪽 삼각형
	m_pnIndices[24] = 2; m_pnIndices[25] = 7; m_pnIndices[26] = 6;
	//ⓙ 옆면(Left) 사각형의 아래쪽 삼각형
	m_pnIndices[27] = 3; m_pnIndices[28] = 7; m_pnIndices[29] = 2;
	//ⓚ 옆면(Right) 사각형의 위쪽 삼각형
	m_pnIndices[30] = 6; m_pnIndices[31] = 4; m_pnIndices[32] = 5;
	//ⓛ 옆면(Right) 사각형의 아래쪽 삼각형
	m_pnIndices[33] = 7; m_pnIndices[34] = 4; m_pnIndices[35] = 6;

	//인덱스 버퍼를 생성한다. 
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices,
	sizeof(UINT)* m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,
		& m_pd3dIndexUploadBuffer);

	//인덱스 버퍼 뷰를 생성한다. 
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx, fy, fz), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

}

CCubeMeshDiffused::~CCubeMeshDiffused()
{
}

CAirplaneMeshDiffused::CAirplaneMeshDiffused(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fDepth,
	XMFLOAT4 xmf4Color) : CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 24 * 3;
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	//위의 그림과 같은 비행기 메쉬를 표현하기 위한 정점 데이터이다. 
	m_pVertices = new CDiffusedVertex[m_nVertices];
	float x1 = fx * 0.2f, y1 = fy * 0.2f, x2 = fx * 0.1f, y3 = fy * 0.3f, y2 = ((y1 - (fy -
		y3)) / x1) * x2 + (fy - y3);
	int i = 0;
	//비행기 메쉬의 위쪽 면
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	//비행기 메쉬의 아래쪽 면
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	//비행기 메쉬의 오른쪽 면
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	//비행기 메쉬의 뒤쪽/오른쪽 면
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	//비행기 메쉬의 왼쪽 면
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	//비행기 메쉬의 뒤쪽/왼쪽 면
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color,
		RANDOM_COLOR));
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx, fy, fz), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CAirplaneMeshDiffused::~CAirplaneMeshDiffused()
{
}

CSphereMeshDiffused::CSphereMeshDiffused(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList, float fRadius, int nSlices, int nStacks) :
	CMesh(pd3dDevice, pd3dCommandList)
{
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	
	m_nVertices = 2 + (nSlices * (nStacks - 1));
	m_pVertices = new CDiffusedVertex[m_nVertices];
	float fDeltaPhi = float(XM_PI / nStacks);
	float fDeltaTheta = float((2.0f * XM_PI) / nSlices);
	int k = 0;
	m_pVertices[k++] = CDiffusedVertex(0.0f, +fRadius, 0.0f, RANDOM_COLOR);
	float theta_i, phi_j;
	for (int j = 1; j < nStacks; j++)
	{
		phi_j = fDeltaPhi * j;
		for (int i = 0; i < nSlices; i++)
		{
			theta_i = fDeltaTheta * i;
			m_pVertices[k++] = CDiffusedVertex(fRadius * sinf(phi_j) * cosf(theta_i),
				fRadius * cosf(phi_j), fRadius * sinf(phi_j) * sinf(theta_i), RANDOM_COLOR);
		}
	}
	m_pVertices[k] = CDiffusedVertex(0.0f, -fRadius, 0.0f, RANDOM_COLOR);
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	m_nIndices = (nSlices * 3) * 2 + (nSlices * (nStacks - 2) * 3 * 2);
	m_pnIndices = new UINT[m_nIndices];
	k = 0;
	for (int i = 0; i < nSlices; i++)
	{
		m_pnIndices[k++] = 0;
		m_pnIndices[k++] = 1 + ((i + 1) % nSlices);
		m_pnIndices[k++] = 1 + i;
	}
	for (int j = 0; j < nStacks-2; j++)
	{
		for (int i = 0; i < nSlices; i++)
		{
			m_pnIndices[k++] = 1 + (i + (j * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + (j * nSlices));
			m_pnIndices[k++] = 1 + (i + ((j + 1) * nSlices));
			m_pnIndices[k++] = 1 + (i + ((j + 1) * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + (j * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + ((j + 1) * nSlices));
		}
	}
	for (int i = 0; i < nSlices; i++)
	{
		m_pnIndices[k++] = (m_nVertices - 1);
		m_pnIndices[k++] = ((m_nVertices - 1) - nSlices) + i;
		m_pnIndices[k++] = ((m_nVertices - 1) - nSlices) + ((i + 1) % nSlices);
	}
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices,
		sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,
		&m_pd3dIndexUploadBuffer);
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fRadius,
		fRadius, fRadius), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CSphereMeshDiffused::~CSphereMeshDiffused()
{
}