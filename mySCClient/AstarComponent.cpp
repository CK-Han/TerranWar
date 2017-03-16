//******************
//���� �ҽ�

#include "stdafx.h"
#include "AstarComponent.h"

AStarComponent* AStarComponent::instance = nullptr;

AStarComponent::AStarComponent()
{
	m_PathOfNodeIndices.clear();
}

AStarComponent::~AStarComponent()
{
	m_PathOfNodeIndices.clear();
}


void AStarComponent::InitializeGraph(int mapSize)
{
	int length = mapSize / 2;

	// ��� 102*102=42,025�� �����
	{
		NavGraphNode node;
		for (float i = -length; i <= length; ++i) {
			for (float j = -length; j <= length; ++j)
			{
				node.SetPos(Position{ j, 0.0f, -i });
				node.SetIndex(m_Graph.GetNextFreeNodeIndex());
				m_Graph.AddNode(node);
			}
		}
	}

	// �� 333,744���� Edge ����
	{
		int size = ((length)* 2) + 1;

		GraphEdge edge;
		for (int i = 0; i < ((size)*(size)); ++i) {
			int left{ i - 1 }, right{ i + 1 }, down{ i + (size) }, up{ i - (size) };
			int leftup{ up - 1 }, leftdown{ down - 1 }, rightup{ up + 1 }, rightdown{ down + 1 };

			// �׵θ� ����
			// 1. ���� �׵θ�
			if (i < size)							up = -1, leftup = -1, rightup = -1;
			// 2. �Ʒ��� �׵θ�
			if (i >= ((size)* (size - 1)))		down = -1, leftdown = -1, rightdown = -1;
			// 3. ���� �׵θ�
			if (i % (size) == 0)					left = -1, leftup = -1, leftdown = -1;
			// 4. ������ �׵θ�
			if (i % (size) == (size - 1))		right = -1, rightup = -1, rightdown = -1;

			int direction[8]{ left, right, down, up, leftup, leftdown, rightup, rightdown };

			// ���� ����
			for (int d = 0; d < 8; ++d)
			{
				if (direction[d] < 0) continue;

				edge.SetFrom(i);
				edge.SetTo(direction[d]);

				d < 4 ? edge.SetCost(1) : edge.SetCost(1.414);

				m_Graph.AddEdge(edge);
			}
		}
	}


	
	const char* filename = "Assets/Image/Terrain/TerrainCollision205x205.png";

	std::vector<unsigned char> png;
	std::vector<unsigned char> image; //the raw pixels
	unsigned width, height;

	//load and decode
	unsigned error = lodepng::load_file(png, filename);
	if (!error) error = lodepng::decode(image, width, height, png);

	bool check[205 * 205];
	int cnt{ 0 };

	//�»�ܿ��� ���������� x ����, �� �� �Ʒ��� z ����
	for (int i = 0; i < 205 * 205; ++i)
	{
		check[i] = (image[cnt] < 200 && image[cnt + 1] < 200 && image[cnt + 2] < 200) ? true : false;
		cnt += 4;
	}

	//int nCount = 0;
	for (int i = 0; i < 205 * 205; ++i)
	{
		if (check[i] == true)
		{
			m_Graph.RemoveNode(i);
	//		nCount++;
		}
	}
	
}


void AStarComponent::SearchAStarPath(int from, int to)
{
	AStar Search(m_Graph, from, to);

	m_PathOfNodeIndices = Search.GetPathToTarget();
}

std::list<D3DXVECTOR3> AStarComponent::SearchAStarPath(D3DXVECTOR3 &vFrom, D3DXVECTOR3 &vTo)
{
	int from = GetNearstNodeIndex(vFrom);
	int to = GetNearstNodeIndex(vTo);

	SearchAStarPath(from, to);

	std::list<D3DXVECTOR3> moveList;	//��������
	
	for (int idx : m_PathOfNodeIndices)
	{
		Vector2D xz = m_Graph.GetNode(idx).Pos();
		//���̰��� ������ ���� Update�ǹǷ� �Ű�X
		moveList.emplace_back(xz.x, vFrom.y, xz.y);
	}
	
	//ó���� �� �κп��� ���ư��� �ʵ��� �Ѵ�.
	if(moveList.size() >= 2)
	{
		D3DXVECTOR3 vDirection = vTo - vFrom;
		D3DXVec3Normalize(&vDirection, &vDirection);
	
		auto pFirst = moveList.begin();
		auto pSecond = ++pFirst;
	
		D3DXVECTOR3 vFirst = *moveList.begin() - vFrom;
		D3DXVECTOR3 vSecond = *pSecond - vFrom;
		D3DXVec3Normalize(&vFirst, &vFirst); D3DXVec3Normalize(&vSecond, &vSecond);
		float dot1 = D3DXVec3Dot(&vFirst, &vDirection);
		float dot2 = D3DXVec3Dot(&vSecond, &vDirection);
		if (dot2 >= dot1)	moveList.pop_front();

		if (moveList.size() >= 2)
		{
			auto pEnd = moveList.rbegin();
			auto pEndPrev = ++pEnd;
			
			D3DXVECTOR3 vEndPrev = vTo - *pEndPrev;
			D3DXVECTOR3 vEnd = vTo - *moveList.rbegin();
			D3DXVec3Normalize(&vEndPrev, &vEndPrev); D3DXVec3Normalize(&vEnd, &vEnd);
			float dot3 = D3DXVec3Dot(&vEndPrev, &vDirection);
			float dot4 = D3DXVec3Dot(&vEnd, &vDirection);
			if (dot3 >= dot4)	moveList.pop_back();
		}
	}
	
	moveList.push_back(vTo);

	return moveList;
}

int AStarComponent::GetNearstNodeIndex(D3DXVECTOR3& pos)
{
	int x = (int)floor(pos.x + 0.5f) + int(MAP_WIDTH / 2.0f);
	int z = (int)floor(pos.z + 0.5f) + int(MAP_HEIGHT / 2.0f);
	int index = x + (int(MAP_WIDTH) * int(MAP_HEIGHT - z - 1.0f));

	return index;
}

Position AStarComponent::GetPositionByIndex(int index)
{
	//0~204
	float x, z;
	x = index % (int)MAP_WIDTH;
	z = index / (int)MAP_WIDTH;

	x -= int(MAP_WIDTH / 2.0f);
	z = int(MAP_HEIGHT / 2.0f) - z;
	
	return Position{ x, 0.0f, z };
}


/*
#include "stdafx.h"
#include "AstarComponent.h"

AStarComponent* AStarComponent::instance = nullptr;

AStarComponent::AStarComponent()
{
	m_PathOfNodeIndices.clear();
}

AStarComponent::~AStarComponent()
{
	m_PathOfNodeIndices.clear();
}


void AStarComponent::InitializeGraph(int mapSize)
{
	int length = mapSize / 2;

	// ��� 102*102=42,025�� �����
	{
		NavGraphNode node;
		for (float i = -length; i <= length; ++i) {
			for (float j = -length; j <= length; ++j)
			{
				node.SetPos(Position{ j, 0.0f, i });
				node.SetIndex(m_Graph.GetNextFreeNodeIndex());
				m_Graph.AddNode(node);
			}
		}
	}

	// �� 333,744���� Edge ����
	{
		int size = ((length)* 2) + 1;

		GraphEdge edge;
		for (int i = 0; i < ((size)*(size)); ++i) {
			int left{ i - 1 }, right{ i + 1 }, down{ i + (size) }, up{ i - (size) };
			int leftup{ up - 1 }, leftdown{ down - 1 }, rightup{ up + 1 }, rightdown{ down + 1 };

			// �׵θ� ����
			// 1. ���� �׵θ�
			if (i < size)							up = -1, leftup = -1, rightup = -1;
			// 2. �Ʒ��� �׵θ�
			if (i >= ((size)* (size - 1)))		down = -1, leftdown = -1, rightdown = -1;
			// 3. ���� �׵θ�
			if (i % (size) == 0)					left = -1, leftup = -1, leftdown = -1;
			// 4. ������ �׵θ�
			if (i % (size) == (size - 1))		right = -1, rightup = -1, rightdown = -1;

			int direction[8]{ left, right, down, up, leftup, leftdown, rightup, rightdown };

			// ���� ����
			for (int d = 0; d < 8; ++d)
			{
				if (direction[d] < 0) continue;

				edge.SetFrom(i);
				edge.SetTo(direction[d]);

				d < 4 ? edge.SetCost(1) : edge.SetCost(1.414);

				m_Graph.AddEdge(edge);
			}
		}
	}
}


void AStarComponent::SearchAStarPath(int from, int to)
{
	AStar Search(m_Graph, from, to);

	m_PathOfNodeIndices = Search.GetPathToTarget();
}

std::list<D3DXVECTOR3> AStarComponent::SearchAStarPath(D3DXVECTOR3 &vFrom, D3DXVECTOR3 &vTo)
{
	int from = GetNearstNodeIndex(vFrom);
	int to = GetNearstNodeIndex(vTo);

	SearchAStarPath(from, to);

	std::list<D3DXVECTOR3> moveList;	//��������

	for (int idx : m_PathOfNodeIndices)
	{
		Vector2D xz = m_Graph.GetNode(idx).Pos();
		//���̰��� ������ ���� Update�ǹǷ� �Ű�X
		moveList.emplace_back(xz.x, vFrom.y, xz.y);
	}

	//ó���� �� �κп��� ���ư��� �ʵ��� �Ѵ�.
	if (moveList.size() >= 2)
	{
		D3DXVECTOR3 vDirection = vTo - vFrom;
		D3DXVec3Normalize(&vDirection, &vDirection);

		auto pFirst = moveList.begin();
		auto pSecond = ++pFirst;

		D3DXVECTOR3 vFirst = *moveList.begin() - vFrom;
		D3DXVECTOR3 vSecond = *pSecond - vFrom;
		D3DXVec3Normalize(&vFirst, &vFirst); D3DXVec3Normalize(&vSecond, &vSecond);
		float dot1 = D3DXVec3Dot(&vFirst, &vDirection);
		float dot2 = D3DXVec3Dot(&vSecond, &vDirection);
		if (dot2 >= dot1)	moveList.pop_front();

		if (moveList.size() >= 2)
		{
			auto pEnd = moveList.rbegin();
			auto pEndPrev = ++pEnd;

			D3DXVECTOR3 vEndPrev = vTo - *pEndPrev;
			D3DXVECTOR3 vEnd = vTo - *moveList.rbegin();
			D3DXVec3Normalize(&vEndPrev, &vEndPrev); D3DXVec3Normalize(&vEnd, &vEnd);
			float dot3 = D3DXVec3Dot(&vEndPrev, &vDirection);
			float dot4 = D3DXVec3Dot(&vEnd, &vDirection);
			if (dot3 >= dot4)	moveList.pop_back();
		}
	}

	moveList.push_back(vTo);

	return moveList;
}

int AStarComponent::GetNearstNodeIndex(D3DXVECTOR3& pos)
{
	int x = (int)floor(pos.x + 0.5f) + int(MAP_WIDTH / 2.0f);
	int z = (int)floor(pos.z + 0.5f) + int(MAP_HEIGHT / 2.0f);
	int index = x + ((MAP_WIDTH)* z);

	return index;
}

Position AStarComponent::GetPositionByIndex(int index)
{
	//0~204
	float x, z;
	x = index % (int)MAP_WIDTH;
	z = index / (int)MAP_WIDTH;

	x -= int(MAP_WIDTH / 2.0f);
	z -= int(MAP_HEIGHT / 2.0f);

	return Position{ x, 0.0f, z };
}

*/