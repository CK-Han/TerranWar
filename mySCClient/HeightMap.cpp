#include "stdafx.h"
#include "HeightMap.h"



//**********************************************************************
// 지형 높이맵
CHeightMap::CHeightMap(LPCTSTR pFileName, int nWidth, int nLength, D3DXVECTOR3 d3dxvScale)
{
	m_nWidth = nWidth;
	m_nLength = nLength;
	m_d3dxvScale = d3dxvScale;

	BYTE *pHeightMapImage = new BYTE[m_nWidth * m_nLength];

	HANDLE hFile = ::CreateFile(pFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);
	DWORD dwBytesRead;
	::ReadFile(hFile, pHeightMapImage, (m_nWidth * m_nLength), &dwBytesRead, NULL);
	::CloseHandle(hFile);



	m_pHeightMapImage = new BYTE[m_nWidth * m_nLength];
	for (int y = 0; y < m_nLength; y++)
	{
		for (int x = 0; x < m_nWidth; x++)
		{
			m_pHeightMapImage[x + (y * m_nWidth)] = pHeightMapImage[x + ((m_nLength - 1 - y) * m_nWidth)] / 32;
		}
	}

	/*
	//test
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	int inputX, inputY;
	while (true)
	{
	scanf("%d %d", &inputX, &inputY);
	printf("높이 : %f\n", GetHeight((float)inputX, (float)inputY, false));
	}
	*/

	if (pHeightMapImage) delete[] pHeightMapImage;
}

CHeightMap::~CHeightMap()
{
	if (m_pHeightMapImage) delete[] m_pHeightMapImage;
	m_pHeightMapImage = NULL;
}


D3DXVECTOR3 CHeightMap::GetHeightMapNormal(int x, int z)
{
	//지형의 x-좌표와 z-좌표가 지형(높이 맵)의 범위를 벗어나면 지형의 법선 벡터는 y-축 방향 벡터이다.
	if ((x < 0.0f) || (z < 0.0f) || (x >= m_nWidth) || (z >= m_nLength)) return(D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	/*높이 맵에서 (x, z) 좌표의 픽셀 값과 인접한 두 개의 점 (x+1, z), (z, z+1)에 대한
	픽셀 값을 사용하여 법선 벡터를 계산한다.*/
	int nHeightMapIndex = x + (z * m_nWidth);
	int xHeightMapAdd = (x < (m_nWidth - 1)) ? 1 : -1;
	int zHeightMapAdd = (z < (m_nLength - 1)) ? m_nWidth : -(signed)m_nWidth;
	//(x, z), (x+1, z), (z, z+1)의 지형의 높이 값을 구한다.
	float y1 = (float)m_pHeightMapImage[nHeightMapIndex] * m_d3dxvScale.y;
	float y2 = (float)m_pHeightMapImage[nHeightMapIndex + xHeightMapAdd] * m_d3dxvScale.y;
	float y3 = (float)m_pHeightMapImage[nHeightMapIndex + zHeightMapAdd] * m_d3dxvScale.y;

	//vEdge1은 (0, y3, m_vScale.z) - (0, y1, 0) 벡터이다.
	D3DXVECTOR3 vEdge1 = D3DXVECTOR3(0.0f, y3 - y1, m_d3dxvScale.z);
	//vEdge2는 (m_vScale.x, y2, 0) - (0, y1, 0) 벡터이다.
	D3DXVECTOR3 vEdge2 = D3DXVECTOR3(m_d3dxvScale.x, y2 - y1, 0.0f);
	//법선 벡터는 vEdge1과 vEdge2의 외적을 정규화하면 된다.
	D3DXVECTOR3 vNormal;
	D3DXVec3Cross(&vNormal, &vEdge1, &vEdge2);
	D3DXVec3Normalize(&vNormal, &vNormal);
	return(vNormal);
}


#define _WITH_APPROXIMATE_OPPOSITE_CORNER
float CHeightMap::GetHeight(float fx, float fz, bool bReverseQuad)
{
	fx = fx / m_d3dxvScale.x;
	fz = fz / m_d3dxvScale.z;

	//if ((fx < 0.0f) || (fz < 0.0f) || (fx >= m_nWidth) || (fz >= m_nLength)) return(0.0f);

	//맵은 -102~102이나 이미지 파일은 0~204 (205개임)이므로 보간한다.
	float xpos = fx + (MAP_WIDTH / 2.0f);
	float zpos = fz + (MAP_HEIGHT / 2.0f);

	int x = (int)xpos;
	int z = (int)zpos;
	float fxPercent = xpos - x;
	float fzPercent = zpos - z;

	//원래 사용했던 높이맵
	//float fHeight = (float)m_pHeightMapImage[(int)xpos + ((int)zpos * m_nWidth)];

	/*
	float fBottomLeft = (float)m_pHeightMapImage[x + (z*m_nWidth)];
	float fBottomRight = (float)m_pHeightMapImage[(x + 1) + (z*m_nWidth)];
	float fTopLeft = (float)m_pHeightMapImage[x + ((z + 1)*m_nWidth)];
	float fTopRight = (float)m_pHeightMapImage[(x + 1) + ((z + 1)*m_nWidth)];

	float fTopHeight = fTopLeft * (1 - fxPercent) + fTopRight * fxPercent;
	float fBottomHeight = fBottomLeft * (1 - fxPercent) + fBottomRight * fxPercent;

	float fHeight = fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent;

	return fHeight;
	*/
	
	float fBottomLeft = (float)m_pHeightMapImage[x + (z*m_nWidth)];
	float fBottomRight = (float)m_pHeightMapImage[(x + 1) + (z*m_nWidth)];
	float fTopLeft = (float)m_pHeightMapImage[x + ((z + 1)*m_nWidth)];
	float fTopRight = (float)m_pHeightMapImage[(x + 1) + ((z + 1)*m_nWidth)];
#ifdef _WITH_APPROXIMATE_OPPOSITE_CORNER
	if (bReverseQuad)
	{
		if (fzPercent >= fxPercent)
			fBottomRight = fBottomLeft + (fTopRight - fTopLeft);
		else
			fTopLeft = fTopRight + (fBottomLeft - fBottomRight);
	}
	else
	{
		if (fzPercent < (1.0f - fxPercent))
			fTopRight = fTopLeft + (fBottomRight - fBottomLeft);
		else
			fBottomLeft = fTopLeft + (fBottomRight - fTopRight);
	}
#endif
	float fTopHeight = fTopLeft * (1 - fxPercent) + fTopRight * fxPercent;
	float fBottomHeight = fBottomLeft * (1 - fxPercent) + fBottomRight * fxPercent;
	//float fHeight = fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent;
	int fHeight = (int)(fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent);

	return(fHeight);
}
