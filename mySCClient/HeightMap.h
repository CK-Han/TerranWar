#pragma once

//**********************************************************************
//�׼����̼� ���� ���̸� Ŭ����
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

	//normal�� �׼����̼ǿ��� ��´�.
	D3DXVECTOR3 GetHeightMapNormal(int x, int z);
	D3DXVECTOR3 GetScale() { return(m_d3dxvScale); }


	BYTE *GetHeightMapImage() { return(m_pHeightMapImage); }
	int GetHeightMapWidth() { return(m_nWidth); }
	int GetHeightMapLength() { return(m_nLength); }
};

