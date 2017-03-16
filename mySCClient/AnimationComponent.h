#pragma once
#include "stdafx.h"


struct VS_SKINNED_MATRIX
{
	D3DXMATRIX	m_d3dxmtxAnimation[MAX_BONE_COUNT];
};


struct Keyframe
{
	Keyframe();
	~Keyframe();

	float TimePos;
	D3DXVECTOR3 Translation;
	D3DXVECTOR3 Scale;
	D3DXVECTOR4 RotationQuat;
};


struct BoneAnimation
{
	float GetStartTime()const;
	float GetEndTime()const;

	void Interpolate(float t, D3DXMATRIX& M)const;

	std::vector<Keyframe> Keyframes;

};


struct AnimationClip
{
	float GetClipStartTime()const;
	float GetClipEndTime()const;

	void Interpolate(float t, std::vector<D3DXMATRIX>& boneTransforms)const;

	std::vector<BoneAnimation> BoneAnimations;
};

class SkinnedData
{
public:
	void AddRef() { m_nReference++; }
	void Release() { if (--m_nReference <= 0) delete this; }

public:
	UINT BoneCount()const;

	float GetClipStartTime(const std::string& clipName)const;
	float GetClipEndTime(const std::string& clipName)const;

	float GetKeyFrameTime(const std::string& clipName, int nKeyFrame) const;

	void Set(
		std::vector<int>& boneHierarchy,
		std::vector<D3DXMATRIX>& boneOffsets,
		std::map<std::string, AnimationClip>& animations);

	void GetFinalTransforms(const std::string& clipName, float timePos,
		std::vector<D3DXMATRIX>& finalTransforms)const;

	D3DXMATRIX GetBoneOffsetMatrix(int nBoneCount);

private:
	int		m_nReference{ 0 };

	std::vector<int> mBoneHierarchy;
	std::vector<D3DXMATRIX> mBoneOffsets;
	std::map<std::string, AnimationClip> mAnimations;
};