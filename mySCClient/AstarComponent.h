#pragma once

//#include "stdafx.h"
#include "AStarComponent\GraphEdge.h"
#include "AStarComponent\GraphNode.h"
#include "AStarComponent\SparseGraph.h"
#include "AStarComponent\GraphAlgorithm.h"
#include "LoadPNG\lodepng.h"

typedef Graph_SearchAStar<SparseGraph<NavGraphNode, GraphEdge>, Heuristic_Euclid> AStar;

//Object에서도 사용하려면 일일히 인자를 넘겨주지 말고... 싱글톤 패턴을 적용해야겠다.
class AStarComponent 
{
private:
	static AStarComponent		*instance;

	AStarComponent();
	~AStarComponent();

public:
	static AStarComponent*	GetInstance()
	{
		if (nullptr == instance)	instance = new AStarComponent();
		//static AStarComponent instance;
		return instance;
	}

public:
	SparseGraph<NavGraphNode, GraphEdge> m_Graph{ false };
	std::list<int> m_PathOfNodeIndices;

public:
	void DeleteInstance() { delete AStarComponent::GetInstance(); }
	void InitializeGraph(int mapSize);
	void SearchAStarPath(int from, int to);
	std::list<D3DXVECTOR3> SearchAStarPath(D3DXVECTOR3 &vFrom, D3DXVECTOR3 &vTo);

	int GetNearstNodeIndex(D3DXVECTOR3 &pos);
	Position GetPositionByIndex(int index);
};