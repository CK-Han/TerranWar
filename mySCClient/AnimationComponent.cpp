#include "stdafx.h"
#include "AnimationComponent.h"


Keyframe::Keyframe()
	: TimePos(0.0f),
	Translation(0.0f, 0.0f, 0.0f),
	Scale(1.0f, 1.0f, 1.0f),
	RotationQuat(0.0f, 0.0f, 0.0f, 1.0f)
{
}

Keyframe::~Keyframe()
{
}

float BoneAnimation::GetStartTime()const
{
	return Keyframes.front().TimePos;
}

float BoneAnimation::GetEndTime()const
{
	return Keyframes.back().TimePos;
}

void BoneAnimation::Interpolate(float t, D3DXMATRIX& M)const
{
	if (Keyframes.size() == 0)
	{
		/*
		D3DXMATRIX identity = D3DXMATRIX(1.0f, 0.0f, 0.0f, 0.0f,
									0.0f, 1.0f, 0.0f, 0.0f,
									0.0f, 0.0f, 1.0f, 0.0f,
									0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&M, identity);
		*/
		D3DXMatrixIdentity(&M);
		return;
	}

	if (t <= Keyframes.front().TimePos)
	{
		D3DXVECTOR3 S = (Keyframes.front().Scale);
		D3DXVECTOR3 P = (Keyframes.front().Translation);
		D3DXQUATERNION Q = (Keyframes.front().RotationQuat);

		D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);
		D3DXMatrixAffineTransformation(&M, S.y, &center, &Q, &P);
	}
	else if (t >= Keyframes.back().TimePos)
	{
		D3DXVECTOR3 S = (Keyframes.back().Scale);
		D3DXVECTOR3 P = (Keyframes.back().Translation);
		D3DXQUATERNION Q = (Keyframes.back().RotationQuat);

		D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);
		D3DXMatrixAffineTransformation(&M, S.y, &center, &Q, &P);
	}
	else
	{
		for (UINT i = 0; i < Keyframes.size() - 1; ++i)
		{
			if (t >= Keyframes[i].TimePos && t <= Keyframes[i + 1].TimePos)
			{
				float lerpPercent = (t - Keyframes[i].TimePos) / (Keyframes[i + 1].TimePos - Keyframes[i].TimePos);

				D3DXVECTOR3 s0 = (Keyframes[i].Scale);
				D3DXVECTOR3 s1 = (Keyframes[i + 1].Scale);

				D3DXVECTOR3 p0 = (Keyframes[i].Translation);
				D3DXVECTOR3 p1 = (Keyframes[i + 1].Translation);

				D3DXQUATERNION q0 = (Keyframes[i].RotationQuat);
				D3DXQUATERNION q1 = (Keyframes[i + 1].RotationQuat);

				D3DXVECTOR3 S;	D3DXVec3Lerp(&S, &s0, &s1, lerpPercent);
				D3DXVECTOR3 P;  D3DXVec3Lerp(&P, &p0, &p1, lerpPercent);
				D3DXQUATERNION Q;  D3DXQuaternionSlerp(&Q, &q0, &q1, lerpPercent);

				D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);
				D3DXMatrixAffineTransformation(&M, S.y, &center, &Q, &P);

				break;
			}
		}
	}
}

float AnimationClip::GetClipStartTime()const
{
	// Find smallest start time over all bones in this clip.
	float t = FLT_MAX;
	for (UINT i = 0; i < BoneAnimations.size(); ++i)
	{
		if (BoneAnimations[i].Keyframes.size() == 0) continue;
		t = min(t, BoneAnimations[i].GetStartTime());
	}

	return t;
}

float AnimationClip::GetClipEndTime()const
{
	// Find largest end time over all bones in this clip.
	float t = 0.0f;
	for (UINT i = 0; i < BoneAnimations.size(); ++i)
	{
		if (BoneAnimations[i].Keyframes.size() == 0) continue;
		t = max(t, BoneAnimations[i].GetEndTime());
	}

	return t;
}

void AnimationClip::Interpolate(float t, std::vector<D3DXMATRIX>& boneTransforms)const
{
	for (UINT i = 0; i < BoneAnimations.size(); ++i)
	{
		BoneAnimations[i].Interpolate(t, boneTransforms[i]);
	}
}

float SkinnedData::GetClipStartTime(const std::string& clipName)const
{
	auto clip = mAnimations.find(clipName);
	return clip->second.GetClipStartTime();
}

float SkinnedData::GetClipEndTime(const std::string& clipName)const
{
	auto clip = mAnimations.find(clipName);
	return clip->second.GetClipEndTime();
}

//0725 동작 구분
float SkinnedData::GetKeyFrameTime(const std::string& clipName, int nKeyFrame) const
{
	auto clip = mAnimations.find(clipName);

	int index = 0;
	for (index; index < clip->second.BoneAnimations.size(); ++index)
	{	//현 clip의 bone들 중 키프레임이 있는 bone을 찾는다.
		int nHaveKeyFrame = clip->second.BoneAnimations[index].Keyframes.size();
		if ((nKeyFrame <= nHaveKeyFrame) && (nHaveKeyFrame != 0))
			break;
	}

	return clip->second.BoneAnimations[index].Keyframes[nKeyFrame].TimePos;
}

//1001 본 행렬을 얻어 총구와 같은 offsetMatrix를 얻는다.
D3DXMATRIX SkinnedData::GetBoneOffsetMatrix(int nBoneCount)
{
	D3DXMATRIX mtxOffset;
	D3DXMatrixIdentity(&mtxOffset);
	if (mBoneOffsets.size() < nBoneCount)
		return mtxOffset;
	else //유효한 인덱스라면
		return mBoneOffsets[nBoneCount];
}



UINT SkinnedData::BoneCount()const
{
	return mBoneHierarchy.size();
}

void SkinnedData::Set(std::vector<int>& boneHierarchy,
	std::vector<D3DXMATRIX>& boneOffsets,
	std::map<std::string, AnimationClip>& animations)
{
	mBoneHierarchy = boneHierarchy;
	mBoneOffsets = boneOffsets;
	mAnimations = animations;
}

void SkinnedData::GetFinalTransforms(const std::string& clipName, float timePos, std::vector<D3DXMATRIX>& finalTransforms)const
{
	UINT numBones = mBoneOffsets.size();

	std::vector<D3DXMATRIX> toRootTransforms(numBones);

	auto clip = mAnimations.find(clipName);
	clip->second.Interpolate(timePos, toRootTransforms);

	
	for (UINT i = 0; i < numBones; ++i)
	{
		D3DXMATRIX offset = (mBoneOffsets[i]);
		
		finalTransforms[i] = offset * (toRootTransforms[i]);
	}
}