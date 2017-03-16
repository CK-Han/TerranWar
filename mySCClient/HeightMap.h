#pragma once

//**********************************************************************
//테셀레이션 지형 높이맵 클래스
class CHeightMap
{
private:
	BYTE						*m_pHeightMapImage;
	int							m_nWidth;
	int							m_nLength;
	D3DXVECTOR3					m_d3dxvScale;

public:
	CHeightMap(LPCTSTR pFileName, int nWidth, int nLength, D3DXVECTOR3 d3dxvScale);
	virtual ~CHeightMap();

	float GetHeight(float x, float z, bool bReverseQuad = false);

	//normal은 테셀레이션에서 얻는다.
	D3DXVECTOR3 GetHeightMapNormal(int x, int z);
	D3DXVECTOR3 GetScale() { return(m_d3dxvScale); }


	BYTE *GetHeightMapImage() { return(m_pHeightMapImage); }
	int GetHeightMapWidth() { return(m_nWidth); }
	int GetHeightMapLength() { return(m_nLength); }
};

