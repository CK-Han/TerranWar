#pragma once

#include "Mesh.h"
#include "protocol.h"

class CAnimationMeshBox
{
public:
	CAnimationMeshBox(ID3D11Device* pd3dDevice);
	~CAnimationMeshBox();

	void LoadModelMesh(ID3D11Device* pd3dDevice);

	map<int, CAnimationMesh*> m_mapMeshBox;
	map<int, CMATERIAL*> m_mapMaterialBox;
	map<int, CTexture*> m_mapTextureBox;

	//1213 ����ŧ�� ��
	map<int, CTexture*> m_mapSpecularMapBox;
	void LoadSpecularMap(ID3D11Device* pd3dDevice);

	//1214 �븻 ��
	map<int, CTexture*> m_mapNormalMapBox;
	void LoadNormalMap(ID3D11Device* pd3dDevice);
};

