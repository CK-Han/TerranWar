#include "Light.fx"

#define		MAX_BONE_COUNT		32
#define		MAP_WIDTH			205.0f
#define		MAP_HEIGHT			205.0f
#define		NUM_FOG_SLICE		1024.0f

#define		UPSIZE_SHADOW_MAP	4.0f	//-> 사용 안한다. 2DResource만들떄는 필요했지만, 셰이더 리소스 안에서는 샘플링으로 처리하므로 Scale필요가 없음

#define		HEIGHTMAP_MULT_VALUE	8.0f

//**********************************************************************
//상수 버퍼 목록
cbuffer cbViewProjectionMatrix : register(b0)	//CB_SLOT_CAMERA
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
};

cbuffer cbWorldMatrix : register(b1)		//CB_SLOT_WORLD_MATRIX
{
	matrix gmtxWorld : packoffset(c0);
	matrix gmtxWorldInvTranspose : packoffset(c4);
};

cbuffer cbShadowTransform : register(b2)		//CB_SLOT_SHADOW
{
	matrix gmtxShadowProjectView : packoffset(c0);
	matrix gmtxShadowProjectProjection : packoffset(c4);
	float4 gvShadowProjectorPosition : packoffset(c8);
}

//cb 3,4번은 조명과 재질임

//디버그 큐브
cbuffer cbDebugCubeOption : register(b5)		//CB_SLOT_DEBUGCUBE
{
	row_major matrix gmtxDebugCubeWorld : packoffset(c0);
	float4 gfDebugCubeColor : packoffset(c4);
}

static matrix gmtxTexture = { 0.5f, 0.0f, 0.0f, 0.0f,
							0.0f, -0.5f, 0.0f, 0.0f,
							0.0f, 0.0f, 1.0f, 0.0f,
							0.5f, 0.5f, 0.0f, 1.0f };

//PS_SLOT_SHADOW_TEXTURE & SAMPLER
Texture2D gtxtShadow : register(t2);
SamplerState gssShadow : register(s2);

//PS_SLOT_TEXTURE & SAMPLER
Texture2D gtxtPS : register(t3);
SamplerState gssPS : register(s3);

//DS_SLOT_TEXTURE & SAMPLER
Texture2D gtxtDSTerrain : register(t4);
SamplerState gssDSTerrain : register(s4);

//애니메이션 구조화 된 버퍼
struct SB_ANIMATION
{
	matrix sbmtxAnimation[MAX_BONE_COUNT];
};
StructuredBuffer<SB_ANIMATION> gSBAnimation : register(t5);

//PS_SLOT_FOG_TEXTURE & SAMPLER
Texture2D gtxtFog : register(t6);
SamplerState gssFog : register(s6);

//안개처리용 버퍼
Buffer<float>	gbufferFogData : register(t7);

//파티클 이미지
Texture2D gtxtParticle : register(t8);
SamplerState gssParticle : register(s8);

//1213 스펙큘러 맵
Texture2D gtxtSpecularMap : register(t9);
SamplerState gssSpecularMap : register(s9);

//1214 노말 맵
Texture2D gtxtNormalMap : register(t10);
SamplerState gssNormalMap : register(s10);


//**********************************************************************
//입력 및 출력 구조체 설정
struct VS_OUTPUT
{
	float4 position : SV_POSITION;
};

///////
//스카이박스 구조체의 나열
struct VS_SKYBOX_INPUT
{
	float3 position : POSITION;
	float2 texCoord : TEXCOORD;
};

struct VS_SKYBOX_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};
//////
//마우스 드래그 구조체의 나열
struct VS_MOUSERECT_INPUT
{
	float3 position : SCREENPOS;
	float2 texCoord : TEXCOORD;
};

struct VS_MOUSERECT_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

///////
//선택된 오브젝트들의 구조체의 나열
struct VS_SELECTEDCIRCLE_INPUT
{
	float4 position : POSSCALE;
};

struct GS_SELECTEDCIRCLE_INPUT
{
	float3 position : POSITION;
	float scaleType : SCALETYPE;
};

struct GS_SELECTEDCIRCLE_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
	float scaleType : SCALETYPE;	//어따쓰일려나... 텍스처 골라쓰기?
};

////////
//UI 메인 프레임 구조체
struct VS_MAINFRAME_INPUT
{
	float4 posTex : POSUIFRAME;
};

struct VS_MAINFRAME_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

////////
//UI Attached 메인 프레임 구조체
struct VS_ATTACHED_INPUT
{
	float4 posTex : POSATTACHED;
	float isRendered : ISRENDERED;
};

struct VS_ATTACHED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
	float isRendered : ISRENDERED;
};

////////
//MiniMap 구조체
struct VS_MINIMAP_INPUT
{
	float4 posTex : POSMINIMAP;
};

struct VS_MINIMAP_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

///////
//MiniMapObject 구조체
struct VS_MINIMAPOBJECT_INPUT
{
	float4 position : POSTYPE;
};

struct GS_MINIMAPOBJECT_INPUT
{
	float3 position : POSITION;
	float colorType : COLORTYPE;
};

struct GS_MINIMAPOBJECT_OUTPUT
{
	float4 position : SV_POSITION;
	float colorType : COLORTYPE;	
};


////////
//기본 지형 구조체의 나열
struct VS_TERRAIN_INPUT
{
	float3 position : POSITION;
	float2 texCoord : TEXCOORD;
	float3 normal : NORMAL;
};

struct VS_TERRAIN_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 texCoord : TEXCOORD;
};
////////
//테셀레이션 지형 구조체의 나열
struct VS_TS_TERRAIN_INPUT
{
	float3 posL : POSITION;
	float2 tex : TEXCOORD;
};

struct VS_TS_TERRAIN_OUTPUT
{
	float3 posW : POSITION;
	float2 tex : TEXCOORD;
};

struct HS_TS_TERRAIN_OUTPUT
{
	float3 posW : POSITION;
	float2 tex : TEXCOORD0;
};

struct HS_TS_TERRAIN_CONSTANT_OUTPUT
{
	float fTessEdges[4] : SV_TessFactor;
	float fTessInsides[2] : SV_InsideTessFactor;
	float nVertices : VERTEXNUM;
};

struct DS_TS_TERRAIN_OUTPUT
{
	float4 posH : SV_POSITION;
	float3 posW : POSITION;
	float2 tex : TEXCOORD;
	float3 normalW : NORMAL;

	//shadow
	float4 texCoordShadow : TEXCOORD1;
	float3 toShadowProjectorW : TEXCOORD2;

	//fog
	float4 texCoordFog : TEXCOORD3;
	float3 toFogProjectorW : TEXCOORD4;
};

/////////////////////////////////////////////////////////////////////////
//Instancing

struct VS_INSTANCE_STATIC_INPUT
{
	float3	position : POSITION;
	float2  texCoord : TEXCOORD;
	float3  normal   : NORMAL;
	float3  tangent : TANGENT;
	column_major float4x4  mtxTransform : POSINSTANCE;
};

struct VS_INSTANCE_STATIC_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float3 tangentW : TANGENT;
	uint   instanceID : INSTANCE_INDEX;
};


struct VS_INSTANCE_ANIMATION_INPUT
{
	float3	position : POSITION;
	float2  texCoord : TEXCOORD;
	float3  normal   : NORMAL;
	uint4   boneID   : BONEID;
	float4  boneWeight   : WEIGHT;
	float3  tangent : TANGENT;
	column_major float4x4  mtxTransform : POSINSTANCE;
	uint   instanceSlot : INSTANCESLOT;
};

struct VS_INSTANCE_ANIMATION_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float3 tangentW : TANGENT;
};

/////////////////////////////////
/////particle
struct VS_SPRITE_PARTICLE_INPUT
{
	float3 position : POSITION;
	row_major float4x4 mtxTransform : INSTANCEPOS;
	float	elapsedTime : ELAPSEDTIME;
	float	numSprites : NUMSPRITES;
	float	betweenTime : BETWEENTIME;
	float   size : SIZE;
};

struct VS_SPRITE_PARTICLE_OUTPUT
{
	row_major float4x4 mtxTransform : INSTANCEPOS;
	float3 position : POSITION;
	float elapsedTime : ELAPSEDTIME;
	float	numSprites : NUMSPRITES;
	float	betweenTime : BETWEENTIME;
	float size : SIZE;
};

struct GS_SPRITE_PARTICLE_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};



//**********************************************************************
//동작함수 정의
float GetFogValue(float3 posW)
{
	float x = posW.x;
	float z = posW.z;
	x += MAP_WIDTH / 2.0f;
	z += MAP_HEIGHT / 2.0f;

	x /= MAP_WIDTH; z /= MAP_HEIGHT;
	int xIdx = (int)(x * (NUM_FOG_SLICE - 1.0f));
	int zIdx = (NUM_FOG_SLICE - 1.0f) - (int)(z * (NUM_FOG_SLICE - 1.0f));
	//return gbufferFogData[xIdx + (zIdx * NUM_FOG_SLICE)];

	float fogValue = gbufferFogData[xIdx + (zIdx * NUM_FOG_SLICE)];
	
	if (fogValue == 1.0f) //현재 시야에는 없지만 탐험지역
	{
		return 0.33f;
	}

	//9개의 방향으로 확장
	int leftOffset = xIdx == 0 ? 0 : -1;
	int rightOffset = xIdx == (int)(NUM_FOG_SLICE - 1.0f) ? 0 : 1;
	int topOffset = zIdx == 0 ? 0 : -1;
	int bottomOffset = zIdx == (int)(NUM_FOG_SLICE - 1.0f) ? 0 : 1;

	int nVisibleCount = 0;

	int center = xIdx + (zIdx * (int)NUM_FOG_SLICE);
	if (gbufferFogData[center] == 2.0f)
		nVisibleCount++;
	int leftTop = center + leftOffset + topOffset * (int)NUM_FOG_SLICE;
	if (gbufferFogData[leftTop] == 2.0f)
		nVisibleCount++;
	int top = center + topOffset * (int)NUM_FOG_SLICE;
	if (gbufferFogData[top] == 2.0f)
		nVisibleCount++;
	int rightTop = center + rightOffset + topOffset * (int)NUM_FOG_SLICE;
	if (gbufferFogData[rightTop] == 2.0f)
		nVisibleCount++;
	int left = center + leftOffset;
	if (gbufferFogData[left] == 2.0f)
		nVisibleCount++;
	int right = center + rightOffset;
	if (gbufferFogData[right] == 2.0f)
		nVisibleCount++;
	int leftBottom = center + leftOffset + bottomOffset * (int)NUM_FOG_SLICE;
	if (gbufferFogData[leftBottom] == 2.0f)
		nVisibleCount++;
	int bottom = center + bottomOffset * (int)NUM_FOG_SLICE;
	if (gbufferFogData[bottom] == 2.0f)
		nVisibleCount++;
	int rightBottom = center + rightOffset + bottomOffset * (int)NUM_FOG_SLICE;
	if (gbufferFogData[rightBottom] == 2.0f)
		nVisibleCount++;
	
	return (float)nVisibleCount / 9.0f;

	/*
	if (0.0f == fogValue)
		return 0.0f;
	else if (2.0f == fogValue)
		return 1.0f;
	else
	{
		//각 지지 않도록 적절히 보간한다.	
	}
	*/
}


//기본 CShader
VS_OUTPUT VS(float4 Pos : POSITION)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.position = mul(Pos, gmtxWorld);
	output.position = mul(output.position, gmtxView);
	output.position = mul(output.position, gmtxProjection);
	return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
	return float4(1.0f, 1.0f, 0.0f, 1.0f); 
}
//CShader

//**********************************************************************
//MouseRectShader - Scene
VS_MOUSERECT_OUTPUT VSMouseRect(VS_MOUSERECT_INPUT input)
{
	VS_MOUSERECT_OUTPUT output = (VS_MOUSERECT_OUTPUT)0;
	output.position = mul(mul(float4(input.position, 1.0f), gmtxView), gmtxProjection);
	output.texCoord = input.texCoord;
	return(output);
}

float4 PSMouseRect(VS_MOUSERECT_OUTPUT input) : SV_Target
{
	float4 cColor = gtxtPS.Sample(gssPS, input.texCoord);
	clip(cColor.g - 0.15f);
	return(cColor);
}

//**********************************************************************
//SelectedCircleShader - Scene
GS_SELECTEDCIRCLE_INPUT VSSelectedCircle(VS_SELECTEDCIRCLE_INPUT input)
{
	GS_SELECTEDCIRCLE_INPUT output = (GS_SELECTEDCIRCLE_INPUT)0;
	output.position = input.position.xyz;
	output.scaleType = input.position.w;
	return(output);
}

[maxvertexcount(4)]
void GSSelectedCircle(point GS_SELECTEDCIRCLE_INPUT input[1], uint primID : SV_PrimitiveID, inout TriangleStream<GS_SELECTEDCIRCLE_OUTPUT> triStream)
{
	float distance = input[0].scaleType;

	if (input[0].scaleType >= 200.0f)
		distance -= 200.0f;
	else if (input[0].scaleType >= 100.0f)
		distance -= 100.0f;

	float4 pVertices[4];
	//순서대로 Append되므로 위치를 잘 찍
	pVertices[1] = float4((input[0].position + float3(+distance, 0.0f, +distance)), 1.0f);	//RT
	pVertices[0] = float4((input[0].position + float3(-distance, 0.0f, +distance)), 1.0f);	//LT
	pVertices[3] = float4((input[0].position + float3(+distance, 0.0f, -distance)), 1.0f);	//RB
	pVertices[2] = float4((input[0].position + float3(-distance, 0.0f, -distance)), 1.0f);	//LB
	float2 pTexCoords[4] = { float2(1.0f, 0.0f), float2(0.0f, 0.0f), float2(1.0f, 1.0f), float2(0.0f, 1.0f) };

	GS_SELECTEDCIRCLE_OUTPUT output = (GS_SELECTEDCIRCLE_OUTPUT)0;
	
	for (int i = 0; i < 4; i++)
	{
		output.position = mul(mul(pVertices[i], gmtxView), gmtxProjection);
		output.texCoord = pTexCoords[i];
		output.scaleType = input[0].scaleType;
		triStream.Append(output);
	}
//	triStream.RestartStrip();
}

float4 PSSelectedCircle(GS_SELECTEDCIRCLE_OUTPUT input) : SV_Target
{
	float4 cColor = gtxtPS.Sample(gssPS, input.texCoord);
	if (cColor.r - 0.05f < 0.0f)
		discard;

	float alpha = cColor.r;

	float4 returnColor = float4(0,0,0,1);

	if (input.scaleType >= 200.0f)
		returnColor = float4(1, 1, 0, 1);
	else if (input.scaleType >= 100.0f)
		returnColor = float4(1, 0, 0, 1);
	else //(input.scaleType >= 200.0f)
		returnColor = float4(0, 1, 0, 1);

	
	//return returnColor * (alpha / 1.428f); // 70%
	return returnColor;
}


//**********************************************************************
//MainFrameShader - Scene
VS_MAINFRAME_OUTPUT VSMainFrame(VS_MAINFRAME_INPUT input)
{
	VS_MAINFRAME_OUTPUT output = (VS_MAINFRAME_OUTPUT)0;
	output.position = float4(input.posTex[0], input.posTex[1], 0.0f, 1.0f);
	output.texCoord = float2(input.posTex[2], input.posTex[3]);
	return(output);
}

float4 PSMainFrame(VS_MAINFRAME_OUTPUT input) : SV_Target
{
	float4 cColor = gtxtPS.Sample(gssPS, input.texCoord);
	if (cColor.a < 0.15f)
		discard;
	return(cColor);
}


//**********************************************************************
//AttachedMainFrame - Scene
VS_ATTACHED_OUTPUT VSAttachedMainFrame(VS_ATTACHED_INPUT input)
{
	VS_ATTACHED_OUTPUT output = (VS_ATTACHED_OUTPUT)0;
	output.position = float4(input.posTex[0], input.posTex[1], 0.0f, 1.0f);
	output.texCoord = float2(input.posTex[2], input.posTex[3]);
	output.isRendered = input.isRendered;
	return(output);
}

float4 PSAttachedMainFrame(VS_ATTACHED_OUTPUT input) : SV_Target
{
	//원본 코드
	float4 cColor = gtxtPS.Sample(gssPS, input.texCoord);
	if (cColor.a < 0.02f)
		discard;
	
	if (input.isRendered == 0.0f)
		cColor *= 0.2f;

	return cColor;
}


//**********************************************************************
//MiniMapShader - Scene -> 맵 이미지를 안개 정보를 참조하여 그려낸다.
VS_MINIMAP_OUTPUT VSMiniMap(VS_MINIMAP_INPUT input)
{
	VS_MINIMAP_OUTPUT output = (VS_MINIMAP_OUTPUT)0;
	output.position = float4(input.posTex[0], input.posTex[1], 0.0f, 1.0f);
	output.texCoord = float2(input.posTex[2], input.posTex[3]);
	return(output);
}

float4 PSMiniMap(VS_MINIMAP_OUTPUT input) : SV_Target
{
	float x = input.texCoord.x;
	float z = input.texCoord.y;

	int xIdx = (int)(x * (NUM_FOG_SLICE - 1.0f));
	int zIdx = (int)(z * (NUM_FOG_SLICE - 1.0f));

	float fogValue = gbufferFogData[xIdx + (zIdx*NUM_FOG_SLICE)];

	float4 cFogColor;

	if (fogValue == 0.0f)
	cFogColor = float4(0, 0, 0, 1);
	else if (fogValue == 1.0f)
	cFogColor = float4(0.3f, 0.3f, 0.3f, 1.0f);
	else
	cFogColor = float4(1, 1, 1, 1);

	float4 cColor = gtxtPS.Sample(gssPS, input.texCoord);
	if (cColor.r < 0.02f)
		discard;

	return cColor * cFogColor;
}

//**********************************************************************
//MiniMapObjectShader - Scene
GS_MINIMAPOBJECT_INPUT VSMiniMapObject(VS_MINIMAPOBJECT_INPUT input)
{
	GS_MINIMAPOBJECT_INPUT output = (GS_MINIMAPOBJECT_INPUT)0;
	output.position = input.position.xyz;
	output.colorType = input.position.w;
	return(output);
}

[maxvertexcount(4)]
void GSMiniMapObject(point GS_MINIMAPOBJECT_INPUT input[1], uint primID : SV_PrimitiveID, inout TriangleStream<GS_MINIMAPOBJECT_OUTPUT> triStream)
{
	float distance = 2.2f;

	float4 pVertices[4];
	//순서대로 Append되므로 위치를 잘 찍
	pVertices[1] = float4((input[0].position + float3(+distance, 0.0f, +distance)), 1.0f);	//RT
	pVertices[0] = float4((input[0].position + float3(-distance, 0.0f, +distance)), 1.0f);	//LT
	pVertices[3] = float4((input[0].position + float3(+distance, 0.0f, -distance)), 1.0f);	//RB
	pVertices[2] = float4((input[0].position + float3(-distance, 0.0f, -distance)), 1.0f);	//LB
	
	GS_MINIMAPOBJECT_OUTPUT output = (GS_MINIMAPOBJECT_OUTPUT)0;

	float2 fMiniMapLB = float2(-0.9888f, -0.97522f);
	float fMiniMapWidth = 0.294f;
	float fMiniMapLength = 0.45312f;
	
	float x, z;
	for (int i = 0; i < 4; i++)
	{
		//0.0f ~ +1.0f으로 보간 
		x = (pVertices[i].x / (MAP_WIDTH / 2.0f) + 1.0f) / 2.0f;
		z = (pVertices[i].z / (MAP_WIDTH / 2.0f) + 1.0f) / 2.0f;
		
		//LB offset를 더해주고, 미니맵 width와 length를 곱해 미니맵 위치로 매핑한다.
		x = fMiniMapLB.x + (x * fMiniMapWidth);
		z = fMiniMapLB.y + (z * fMiniMapLength);	

		output.position = float4(x, z, 0.0f, 1.0f);
		output.colorType = input[0].colorType;
		triStream.Append(output);
	}
	//	triStream.RestartStrip();
}

float4 PSMiniMapObject(GS_MINIMAPOBJECT_OUTPUT input) : SV_Target
{
	float4 cColor = float4(1, 1, 0, 1);

	if (input.colorType == 0.0f)
		cColor = float4(0, 1, 0, 1);
	if (input.colorType == 1.0f)
		cColor = float4(1, 0, 0, 1);
	if (input.colorType == 2.0f)
		cColor = float4(1, 1, 0, 1);

		
	return(cColor);
}


//**********************************************************************
//기본 CSkyBoxShader
VS_SKYBOX_OUTPUT VSSkyBox(VS_SKYBOX_INPUT input)
{
	VS_SKYBOX_OUTPUT output = (VS_SKYBOX_OUTPUT)0;
	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProjection);
	output.texCoord = input.texCoord;

	return(output);
}

float4 PSSkyBox(VS_SKYBOX_OUTPUT input) : SV_Target
{
	float4 cColor = gtxtPS.Sample(gssPS, input.texCoord);
	return(cColor);
}

//기본 TerrainShader
VS_TERRAIN_OUTPUT VSTerrain(VS_TERRAIN_INPUT input)
{
	VS_TERRAIN_OUTPUT output = (VS_TERRAIN_OUTPUT)0;
	output.positionW = mul(float4(input.position, 1.0f), gmtxWorld).xyz;
	output.normalW = mul(float4(input.normal, 1.0f), gmtxWorld).xyz;
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.texCoord = input.texCoord;

	return(output);
}

float4 PSTerrain(VS_TERRAIN_OUTPUT input) : SV_Target
{
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW);
	float4 cColor = gtxtPS.Sample(gssPS, input.texCoord) * cIllumination;

	return(cColor * cIllumination);
}



//**********************************************************************
//테셀레이션 지형 코드
VS_TS_TERRAIN_OUTPUT VSTessellation(VS_TS_TERRAIN_INPUT input)
{
	VS_TS_TERRAIN_OUTPUT output;
	output.posW = input.posL;
	output.tex = input.tex;
	return output;
}

HS_TS_TERRAIN_CONSTANT_OUTPUT ConstantHSTerrain(InputPatch<VS_TS_TERRAIN_OUTPUT, 4> input, uint nPatchID : SV_PrimitiveID)
{
	HS_TS_TERRAIN_CONSTANT_OUTPUT output;

	output.fTessEdges[0] = 64;
	output.fTessEdges[1] = 64;
	output.fTessEdges[2] = 64;
	output.fTessEdges[3] = 64;

	output.fTessInsides[0] = 64;
	output.fTessInsides[1] = 64;

	output.nVertices = float(64 * 64);
	return output;
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHSTerrain")]
[maxtessfactor(64.0f)]
HS_TS_TERRAIN_OUTPUT HSTessellation(InputPatch<VS_TS_TERRAIN_OUTPUT, 4> input, uint i : SV_OutputControlPointID)
{
	HS_TS_TERRAIN_OUTPUT output;
	output.posW = input[i].posW;
	output.tex = input[i].tex;
	return output;
}

[domain("quad")]
DS_TS_TERRAIN_OUTPUT DSTessellation(HS_TS_TERRAIN_CONSTANT_OUTPUT input, float2 uv : SV_DomainLocation, OutputPatch<HS_TS_TERRAIN_OUTPUT, 4> quad)
{
	DS_TS_TERRAIN_OUTPUT output;

	output.posW = lerp(lerp(quad[1].posW, quad[2].posW, uv.x), lerp(quad[0].posW, quad[3].posW, uv.x), uv.y);
	output.tex = lerp(lerp(quad[1].tex, quad[2].tex, uv.x), lerp(quad[0].tex, quad[3].tex, uv.x), uv.y);
	//0 (0.0f,1.0f)  / 1 (0.0f,0.0f)  /  2 (1.0f,0.0f)  /  3 (1.0f,1.0f)
	//output.texDetail = output.tex * 8.0f;

	//새 Normal 계산
	float y1, y2, y3;

	float nVertices = input.nVertices;
	float2 xNext = saturate(float2(uv.x + (1.0f / nVertices), uv.y));
	float2 zNext = saturate(float2(uv.x, uv.y + (1.0f / nVertices)));

	y1 = (float)(int)(mul(1.0f, gtxtDSTerrain.SampleLevel(gssDSTerrain, output.tex, 0).r) * HEIGHTMAP_MULT_VALUE);
	y2 = mul(1.0f, gtxtDSTerrain.SampleLevel(gssDSTerrain, xNext, 0).r);
	y3 = mul(1.0f, gtxtDSTerrain.SampleLevel(gssDSTerrain, zNext, 0).r);

	float3 vEdge1 = float3(0.0f, y3 - y1, 1.0f);
	float3 vEdge2 = float3(1.0f, y2 - y1, 0.0f);

	output.normalW = cross(vEdge1, vEdge2);
	
	output.posW.y = y1;
	output.posH = mul(mul(float4(output.posW, 1.0f), gmtxView), gmtxProjection);

	//shadow
	output.texCoordShadow = mul(mul(mul(float4(output.posW, 1.0f), gmtxShadowProjectView), gmtxShadowProjectProjection), gmtxTexture);
	output.toShadowProjectorW = normalize(gvShadowProjectorPosition.xyz - output.posW);	
	
	return output;
}


float4 PSTessellation(DS_TS_TERRAIN_OUTPUT input) : SV_Target
{
	
	//새로 만든 안개 처리
	float fogValue = GetFogValue(input.posW);

	float4 fogColor;
	if (fogValue == 0.0f)
		return float4(0.0f, 0.0f, 0.0f, 1.0f);
	else
		fogColor = float4(fogValue, fogValue, fogValue, 1.0f);
	

	input.normalW = normalize(input.normalW);

	float4 cBaseTexColor = gtxtPS.Sample(gssPS, input.tex);
	float4 cColor = cBaseTexColor;
	float4 cIllumination = Lighting(input.posW, input.normalW);

	
	//그림자 
	//if ((dot(input.toShadowProjectorW, input.normalW) <= 0.0f) || (input.texCoordShadow.w <= 0.0f))
	if ((input.texCoordShadow.w <= 0.0f)) //normal값을 world의 역행렬의 전치행렬을 곱해줘야하는데 인스턴스라...
		cColor = (cColor*cIllumination);
	else
	{
		float4 cShadowColor = gtxtShadow.Sample(gssShadow, input.texCoordShadow.xy / input.texCoordShadow.ww);
		
		if (cShadowColor.r <= 0.66f)
			//return float4(0.0f, 0.0f, 0.0f, 1.0f);	//그림자이다.
			cColor *= float4(0.2f, 0.2f, 0.2f, 1.0f);
		else
			cColor = (cColor*cIllumination);
	}

	//return cColor;
	return(cColor * fogColor);
}


//**********************************************************************
//내 정의 코드들


//**********************************************************************
//test for Instancing

//Static
VS_INSTANCE_STATIC_OUTPUT VSStatic(VS_INSTANCE_STATIC_INPUT input, uint instanceID : SV_InstanceID)
{
	VS_INSTANCE_STATIC_OUTPUT output = (VS_INSTANCE_STATIC_OUTPUT)0;
	output.positionW = mul(float4(input.position, 1.0f), input.mtxTransform).xyz;
	output.normalW = mul(input.normal, (float3x3)input.mtxTransform);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);

	output.texCoord = input.texCoord;
	output.instanceID = instanceID;
	
	output.tangentW = mul(input.tangent, (float3x3)input.mtxTransform);
	
	return(output);
}

float4 PSStatic(VS_INSTANCE_STATIC_OUTPUT input) : SV_Target
{
	//float4 cIllumination = Lighting(input.positionW, input.normalW);
	
	float3 N = normalize(input.normalW);
	float3 T = normalize(input.tangentW - dot(input.tangentW, N) * N);
	float3 B = cross(N, T);
	float3x3 TBN = float3x3(T, B, N);
	
	float3 cNormal = gtxtNormalMap.Sample(gssNormalMap, input.texCoord).rgb;
	float3 interNormal = 2.0f * cNormal - 1.0f;
	float3 normalW = mul(interNormal, TBN);
	

	//float3 normalW = normalize(input.normalW);

	float4 vSpecular = gtxtSpecularMap.Sample(gssSpecularMap, input.texCoord);
	vSpecular.a = 200.0f;
	float4 cIllumination = Lighting(input.positionW, normalW, vSpecular);
	//float4 cIllumination = Lighting(input.positionW, normalW);
	float4 cColor = gtxtPS.Sample(gssPS, input.texCoord)*cIllumination;

	/*
	//specTest
	float4 cSpecular = gtxtSpecularMap.Sample(gssSpecularMap, input.texCoord);
	cSpecular.a = 1.0f;
	cColor *= cSpecular;
	*/

	return(cColor);
}

//Static Shadow
struct VS_STATIC_SHADOW_INPUT
{
	float3 position : POSITION;
	float4x4 mtxTransform : INSTANCEPOS;
};

struct VS_STATIC_SHADOW_OUTPUT
{
	float4 position : SV_POSITION;
};

VS_STATIC_SHADOW_OUTPUT VSStaticShadow(VS_STATIC_SHADOW_INPUT input)
{
	VS_STATIC_SHADOW_OUTPUT output = (VS_STATIC_SHADOW_OUTPUT)0;

	//현재 gmtxView와 Projection은 조명의 값이다.
	float3 position = float3(input.position);// *UPSIZE_SHADOW_MAP;
	output.position = mul(mul(mul(float4(position, 1.0f), input.mtxTransform), gmtxView), gmtxProjection);

	return output;
}

float4 PSStaticShadow(VS_STATIC_SHADOW_OUTPUT input) : SV_Target
{
	return float4(0,0,0,1);
}


//Animation
VS_INSTANCE_ANIMATION_OUTPUT VSAnimation(VS_INSTANCE_ANIMATION_INPUT input)
{
	VS_INSTANCE_ANIMATION_OUTPUT output = (VS_INSTANCE_ANIMATION_OUTPUT)0;

	float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	weights[0] = input.boneWeight.x;
	weights[1] = input.boneWeight.y;
	weights[2] = input.boneWeight.z;
	weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

	float3 posL = float3(0.0f, 0.0f, 0.0f);
	float3 normalL = float3(0.0f, 0.0f, 0.0f);
	float3 tangentL = float3(0.0f, 0.0f, 0.0f);
	
	for (int i = 0; i < 4; i++)
	{
		posL += weights[i] * mul(float4(input.position, 1.0f), gSBAnimation[input.instanceSlot].sbmtxAnimation[(int)input.boneID[i]]).xyz;
		normalL += weights[i] * mul(input.normal, (float3x3)gSBAnimation[input.instanceSlot].sbmtxAnimation[(int)input.boneID[i]]);
		tangentL += weights[i] * mul(input.tangent, (float3x3)gSBAnimation[input.instanceSlot].sbmtxAnimation[(int)input.boneID[i]]);
	}
	
	//output.positionW = mul(mul(float4(posL, 1.0f), gSBAnimation[input.instanceSlot].sbmtxAnimation[MAX_BONE_COUNT - 1]), input.mtxTransform).xyz;
	//output.normalW = mul(mul(float4(normalL, 1.0f), gSBAnimation[input.instanceSlot].sbmtxAnimation[MAX_BONE_COUNT - 1]), input.mtxTransform).xyz;
	output.positionW = mul(float4(posL, 1.0f), input.mtxTransform).xyz;
	output.normalW = mul(float4(normalL, 1.0f), input.mtxTransform).xyz;
	output.tangentW = mul(float4(tangentL, 1.0f), input.mtxTransform).xyz;

	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.texCoord = input.texCoord;

	return output;
}

float4 PSAnimation(VS_INSTANCE_ANIMATION_OUTPUT input) : SV_Target
{
	//input.normalW = normalize(input.normalW);
	
	float3 N = normalize(input.normalW);
	float3 T = normalize(input.tangentW - dot(input.tangentW, N) * N);
	float3 B = cross(N, T);
	float3x3 TBN = float3x3(T, B, N);

	float3 cNormal = gtxtNormalMap.Sample(gssNormalMap, input.texCoord).rgb;
	float3 interNormal = 2.0f * cNormal - 1.0f;
	float3 normalW = mul(interNormal, TBN);
	
	
	float4 vSpecular = gtxtSpecularMap.Sample(gssSpecularMap, input.texCoord);
	vSpecular.a = 200.0f;
	float4 cIllumination = Lighting(input.positionW, normalW, vSpecular);
	float4 cColor = gtxtPS.Sample(gssPS, input.texCoord)*cIllumination;
	
	/*
	//specTest
	float4 cSpecular = gtxtSpecularMap.Sample(gssSpecularMap, input.texCoord);
	cSpecular.a = 1.0f;
	cColor *= cSpecular;
	*/

	return(cColor);


	/*
	float4 cColor = gtxtPS.Sample(gssPS, input.texCoord); // *cIllumination;
	return cColor;
	*/
}


//Animation Shadow
struct VS_ANIMATION_SHADOW_INPUT
{
	float3	position : POSITION;
	uint4   boneID   : BONEID;
	float4  boneWeight   : WEIGHT;
	column_major float4x4  mtxTransform : POSINSTANCE;
	uint   instanceSlot : INSTANCESLOT;
};

struct VS_ANIMATION_SHADOW_OUTPUT
{
	float4 position : SV_POSITION;
};

VS_ANIMATION_SHADOW_OUTPUT VSAnimationShadow(VS_ANIMATION_SHADOW_INPUT input)
{
	VS_ANIMATION_SHADOW_OUTPUT output = (VS_ANIMATION_SHADOW_OUTPUT)0;

	
	//애니메이션 한 동작에 대한 그림자
	float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	weights[0] = input.boneWeight.x;
	weights[1] = input.boneWeight.y;
	weights[2] = input.boneWeight.z;
	weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

	float3 posL = float3(0.0f, 0.0f, 0.0f);
	float3 position = float3(input.position);// *UPSIZE_SHADOW_MAP;
	for (int i = 0; i < 4; i++)
	{
		posL += weights[i] * mul(float4(position, 1.0f), gSBAnimation[input.instanceSlot].sbmtxAnimation[(int)input.boneID[i]]).xyz;
	}

	//float3 positionW = mul(mul(float4(posL, 1.0f), gSBAnimation[input.instanceSlot].sbmtxAnimation[MAX_BONE_COUNT - 1]), input.mtxTransform).xyz;
	float3 positionW = mul(float4(posL, 1.0f), input.mtxTransform).xyz;

	//현재 gmtxView와 Projection은 조명의 값이다.
	output.position = mul(mul(float4(positionW, 1.0f), gmtxView), gmtxProjection);
	
	
	/*
	//기존 정점정보로 그릴때
	float3 position = float3(input.position) * UPSIZE_SHADOW_MAP;
	output.position = mul(mul(mul(float4(position, 1.0f), input.mtxTransform), gmtxView), gmtxProjection);
	*/

	return output;
}

float4 PSAnimationShadow(VS_ANIMATION_SHADOW_OUTPUT input) : SV_Target
{
	return float4(0,0,0,1);
}
//end of Instancing


//*******************************
//Particle

//********************************************************************
//0805 파티클 테스트
VS_SPRITE_PARTICLE_OUTPUT VSSpriteParticle(VS_SPRITE_PARTICLE_INPUT input)
{
	VS_SPRITE_PARTICLE_OUTPUT output = (VS_SPRITE_PARTICLE_OUTPUT)0;

	output.position = input.position.xyz;
	output.mtxTransform = input.mtxTransform;
	output.elapsedTime = input.elapsedTime;
	output.numSprites = input.numSprites;
	output.betweenTime = input.betweenTime;
	output.size = input.size;
	return output;
}


[maxvertexcount(4)]
void GSSpriteParticle(point VS_SPRITE_PARTICLE_OUTPUT input[1], uint primIDn : SV_PrimitiveID, inout TriangleStream<GS_SPRITE_PARTICLE_OUTPUT> triStream)
{
	float3 vUp = float3(0.0f, 1.0f, 0.0f);
	float3 vLook = gvCameraPosition.xyz - input[0].position;
	vLook = normalize(vLook);
	float3 vRight = cross(vUp, vLook);
	float fHalfW = input[0].size / 2.0f;
	float fHalfH = input[0].size / 4.0f;

	float fU = 1.0f / input[0].numSprites;

	int elapsedTime = (int)(input[0].elapsedTime / input[0].betweenTime);
	int nSpritePosition = elapsedTime % (int)input[0].numSprites;	//-> 0~5

	float4 pVertices[4];
	//순서대로 Append되므로 위치를 잘 찍
	/*
	pVertices[1] = float4(input[0].position + fHalfW * vRight + fHalfH*vUp, 1.0f);	//RT
	pVertices[0] = float4(input[0].position - fHalfW * vRight + fHalfH*vUp, 1.0f);	//LT
	pVertices[3] = float4(input[0].position + fHalfW * vRight - fHalfH*vUp, 1.0f);	//RB
	pVertices[2] = float4(input[0].position - fHalfW * vRight - fHalfH*vUp, 1.0f);	//LB
	*/

	//0811 빌보드로 그리면 제대로 안그려진다; y를 0으로 잡고 depth 검사를 끄고 그리니 잘 되긴 한다.
	//float distance = input[0].size / 2.0f;
	pVertices[1] = float4((input[0].position + float3(+fHalfW, 0.0f, +fHalfH)), 1.0f);	//RT
	pVertices[0] = float4((input[0].position + float3(-fHalfW, 0.0f, +fHalfH)), 1.0f);	//LT
	pVertices[3] = float4((input[0].position + float3(+fHalfW, 0.0f, -fHalfH)), 1.0f);	//RB
	pVertices[2] = float4((input[0].position + float3(-fHalfW, 0.0f, -fHalfH)), 1.0f);	//LB

	for (int i = 0; i < 4; ++i)
		pVertices[i] = mul(pVertices[i], input[0].mtxTransform);
	
	//float2 pTexCoords[4] = { float2(1.0f, 0.0f), float2(0.0f, 0.0f), float2(1.0f, 1.0f), float2(0.0f, 1.0f) };
	float2 pTexCoords[4] = { float2(fU * nSpritePosition + fU, 0.0f), float2(fU * nSpritePosition, 0.0f),
		float2(fU * nSpritePosition + fU, 1.0f), float2(fU * nSpritePosition, 1.0f) };


	/*
	float distance = 100.0f;
	float4 pVertices[4];
	//순서대로 Append되므로 위치를 잘 찍
	pVertices[1] = float4((input[0].position + float3(+distance, 0.0f, +distance)), 1.0f);	//RT
	pVertices[0] = float4((input[0].position + float3(-distance, 0.0f, +distance)), 1.0f);	//LT
	pVertices[3] = float4((input[0].position + float3(+distance, 0.0f, -distance)), 1.0f);	//RB
	pVertices[2] = float4((input[0].position + float3(-distance, 0.0f, -distance)), 1.0f);	//LB
	float2 pTexCoords[4] = { float2(1.0f, 0.0f), float2(0.0f, 0.0f), float2(1.0f, 1.0f), float2(0.0f, 1.0f) };
	*/

	GS_SPRITE_PARTICLE_OUTPUT output = (GS_SPRITE_PARTICLE_OUTPUT)0;

	for (int i = 0; i < 4; i++)
	{
		output.position = mul(mul(pVertices[i], gmtxView), gmtxProjection);
		output.texCoord = pTexCoords[i];
		triStream.Append(output);
	}
}

float4 PSSpriteParticle(GS_SPRITE_PARTICLE_OUTPUT input) : SV_Target
{
	float4 cColor = gtxtParticle.Sample(gssParticle, input.texCoord);
	if (cColor.a <= 0.15f)
		discard;
	return cColor;

	//return float4(1,0,0,1);
}



//****************************************
//0814 NumberFont 적용
struct VS_FONT_INPUT
{
	float4 posColor : POSCOLOR;
	float2 texCoord : TEXCOORD;
};

struct VS_FONT_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
	float  color : COLOR;
};

VS_FONT_OUTPUT VSNumberFont(VS_FONT_INPUT input)
{
	VS_FONT_OUTPUT output = (VS_FONT_OUTPUT)0;
	output.position = float4(input.posColor.xy, 0.0f, 1.0f);
	output.color = input.posColor.w;
	output.texCoord = input.texCoord;

	return output;
}

float4 PSNumberFont(VS_FONT_OUTPUT input) : SV_Target
{
	float4 cColor = gtxtPS.Sample(gssPS, input.texCoord);
	if (cColor.a <= 0.7f)
		discard;
	//if (cColor.r >= 0.2f)
	//	discard;
	return cColor;

	//return float4(0,1,0,1);
}


//****************************************
//건물 짓기 전 이미지 테스트
struct VS_CREATE_BUILDING_INPUT
{
	float4	posSize : POSSIZE;
};

struct GS_CREATE_BUILDING_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

VS_CREATE_BUILDING_INPUT VSBuildCheck(VS_CREATE_BUILDING_INPUT input)
{
	return input;
}


[maxvertexcount(4)]
void GSBuildCheck(point VS_CREATE_BUILDING_INPUT input[1], uint primID : SV_PrimitiveID, inout TriangleStream<GS_CREATE_BUILDING_OUTPUT> triStream)
{
	//본 코드
	float distance = input[0].posSize.w / 2.0f;

	float4 pVertices[4];
	//순서대로 Append되므로 위치를 잘 찍
	pVertices[1] = float4((input[0].posSize.xyz + float3(+distance, 0.0f, +distance)), 1.0f);	//RT
	pVertices[0] = float4((input[0].posSize.xyz + float3(-distance, 0.0f, +distance)), 1.0f);	//LT
	pVertices[3] = float4((input[0].posSize.xyz + float3(+distance, 0.0f, -distance)), 1.0f);	//RB
	pVertices[2] = float4((input[0].posSize.xyz + float3(-distance, 0.0f, -distance)), 1.0f);	//LB
	float2 pTexCoords[4] = { float2(1.0f, 0.0f), float2(0.0f, 0.0f), float2(1.0f, 1.0f), float2(0.0f, 1.0f) };


	/*
	float3 vUp = float3(0.0f, 1.0f, 0.0f);
	float3 vLook = gvCameraPosition.xyz - input[0].posSize.xyz;
	vLook = normalize(vLook);
	float3 vRight = cross(vUp, vLook);
	float fHalfW = input[0].posSize.w / 2.0f * 30.0f;
	float fHalfH = input[0].posSize.w / 2.0f * 30.0f;

	float4 pVertices[4];
	//순서대로 Append되므로 위치를 잘 찍
	pVertices[1] = float4(input[0].posSize.xyz + fHalfW * vRight + fHalfH*vUp, 1.0f);	//RT
	pVertices[0] = float4(input[0].posSize.xyz - fHalfW * vRight + fHalfH*vUp, 1.0f);	//LT
	pVertices[3] = float4(input[0].posSize.xyz + fHalfW * vRight - fHalfH*vUp, 1.0f);	//RB
	pVertices[2] = float4(input[0].posSize.xyz - fHalfW * vRight - fHalfH*vUp, 1.0f);	//LB
	float2 pTexCoords[4] = { float2(1.0f, 0.0f), float2(0.0f, 0.0f), float2(1.0f, 1.0f), float2(0.0f, 1.0f) };
	*/

	GS_CREATE_BUILDING_OUTPUT output = (GS_CREATE_BUILDING_OUTPUT)0;

	for (int i = 0; i < 4; i++)
	{
		output.position = mul(mul(pVertices[i], gmtxView), gmtxProjection);
		output.texCoord = pTexCoords[i];
		triStream.Append(output);
	}
	//	triStream.RestartStrip();
}

float4 PSBuildCheck(GS_CREATE_BUILDING_OUTPUT input) : SV_Target
{
	float4 cColor = gtxtPS.Sample(gssPS, input.texCoord);
	if (cColor.a <= 0.15f)
		discard;
	return cColor;
	//return float4(1,0,0,1);
}




//****************************************
// 0826 프로그레스 바 테스트
// 0904 hp도 같이 표현하기 위해 posColor의 w값으로 스크린좌표인지 월드좌표인지 구분
struct VS_PROGRESS_INPUT
{
	float4 posColor : POSCOLOR;
	float2 texCoord : TEXCOORD;
};

struct VS_PROGRESS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
	float color : COLOR;
};

VS_PROGRESS_OUTPUT VSProgressBar(VS_PROGRESS_INPUT input)
{
	VS_PROGRESS_OUTPUT output = (VS_PROGRESS_OUTPUT)0;

	if(input.posColor.w == 0.0f)
		output.position = float4(input.posColor.x, input.posColor.y, 0.0f, 1.0f);
	else
		output.position = mul(mul(float4(input.posColor.xyz, 1.0f), gmtxView), gmtxProjection);
	

	output.color = input.posColor.w;
	output.texCoord = input.texCoord;

	return output;
}

float4 PSProgressBar(VS_PROGRESS_OUTPUT input) : SV_Target
{
	/*
	float4 cColor = float4(0,0,0,1);
	
	if (input.color == 0.0f) // 프로그레스 바 렌더링인 경우
		cColor = gtxtPS.Sample(gssPS, input.texCoord);
	 if (input.color == 1.0f)
		cColor = float4(0, 0, 0, 1);
	 if (input.color == 2.0f)
		cColor = float4(0, 1, 0, 1);
	if (input.color == 3.0f)
		cColor = float4(1, 1, 0, 1);
	if(input.color == 4.0f)
		cColor = float4(1, 0, 0, 1);
	*/
	float4 cColor = gtxtPS.Sample(gssPS, input.texCoord);
	return cColor;
}


//-------------------------------------------------------------------------------------------------------------------------------
//미니맵 카메라 위치에 대한 사각형
struct VS_MINIMAPRECT_INPUT
{
	float3 position : POSITION;
	float2 texCoord : TEXCOORD;
};

struct VS_MINIMAPRECT_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texCoord: TEXCOORD;
};

VS_MINIMAPRECT_OUTPUT VSMiniMapRect(VS_MINIMAPRECT_INPUT input)
{
	VS_MINIMAPRECT_OUTPUT output = (VS_MINIMAPRECT_OUTPUT)0;

	float2 fMiniMapLB = float2(-0.9888f, -0.97522f);
	float fMiniMapWidth = 0.294f;
	float fMiniMapLength = 0.45312f;

	float x, z;
	//0.0f ~ +1.0f으로 보간 
	x = (input.position.x / (MAP_WIDTH / 2.0f) + 1.0f) / 2.0f;
	z = (input.position.z / (MAP_WIDTH / 2.0f) + 1.0f) / 2.0f;

		//LB offset를 더해주고, 미니맵 width와 length를 곱해 미니맵 위치로 매핑한다.
	x = fMiniMapLB.x + (x * fMiniMapWidth);
	z = fMiniMapLB.y + (z * fMiniMapLength);
	
	output.position = float4(x, z, 0.0f, 1.0f);
	output.texCoord = input.texCoord;

	return output;
}

float4 PSMiniMapRect(VS_MINIMAPRECT_OUTPUT input) : SV_Target
{
	float4 cColor = gtxtPS.Sample(gssPS, input.texCoord);
	if (cColor.a <= 0.15f)
		discard;

	//return float4(1,1,1,1);
	return cColor;
}



//-------------------------------------------------------------------------------------------------------------------------------
//중립 자원 렌더링
//VSNeutralResource
struct VS_NEUTRALRESOURCE_INPUT
{
	float4 posType : POSTYPE;
};

struct GS_NEUTRALRESOURCE_INPUT
{
	float3 position : POSITION;
	float resourceType : TYPE;
};

struct GS_NEUTRALRESOURCE_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

GS_NEUTRALRESOURCE_INPUT VSNeutralResource(VS_NEUTRALRESOURCE_INPUT input)
{
	GS_NEUTRALRESOURCE_INPUT output = (GS_NEUTRALRESOURCE_INPUT)0;
	output.position = input.posType.xyz;
	output.resourceType = input.posType.w;
	return(output);
}

[maxvertexcount(4)]
void GSNeutralResource(point GS_NEUTRALRESOURCE_INPUT input[1], uint primID : SV_PrimitiveID, inout TriangleStream<GS_NEUTRALRESOURCE_OUTPUT> triStream)
{
	float distance = 0.25f;

	float4 pVertices[4];
	float2 pTexCoord[4];
	//순서대로 Append되므로 위치를 잘 찍
	pVertices[1] = float4((input[0].position + float3(+distance, 0.0f, +distance)), 1.0f);	//RT
	pVertices[0] = float4((input[0].position + float3(-distance, 0.0f, +distance)), 1.0f);	//LT
	pVertices[3] = float4((input[0].position + float3(+distance, 0.0f, -distance)), 1.0f);	//RB
	pVertices[2] = float4((input[0].position + float3(-distance, 0.0f, -distance)), 1.0f);	//LB

	if(input[0].resourceType == 1.0f) //mineral
	{
		pTexCoord[1] = float2(0.5f, 0.0f);
		pTexCoord[0] = float2(0.0f, 0.0f);
		pTexCoord[3] = float2(0.5f, 1.0f);
		pTexCoord[2] = float2(0.0f, 1.0f);
	}
	else //gas
	{
		pTexCoord[1] = float2(1.0f, 0.0f);
		pTexCoord[0] = float2(0.5f, 0.0f);
		pTexCoord[3] = float2(1.0f, 1.0f);
		pTexCoord[2] = float2(0.5f, 1.0f);
	}

	GS_NEUTRALRESOURCE_OUTPUT output = (GS_NEUTRALRESOURCE_OUTPUT)0;

	for (int i = 0; i < 4; i++)
	{
		output.position = mul(mul(pVertices[i], gmtxView), gmtxProjection);
		output.texCoord = pTexCoord[i];
		triStream.Append(output);
	}
}

float4 PSNeutralResource(GS_NEUTRALRESOURCE_OUTPUT input) : SV_Target
{
	float4 cColor = gtxtPS.Sample(gssPS, input.texCoord);
	if (cColor.a <= 0.15f)
		discard;

	return cColor;
}



//-------------------------------------------------------------------------------------------------------------------------------
//군중 유닛 이미지
//VSCrowdImage
struct VS_CROWDIMAGE_INPUT
{
	float4 posTex : POSCROWD;
	float color : COLOR;
};

struct VS_CROWDIMAGE_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
	float color : COLOR;
};

VS_CROWDIMAGE_OUTPUT VSCrowdImage(VS_CROWDIMAGE_INPUT input)
{
	VS_CROWDIMAGE_OUTPUT output = (VS_CROWDIMAGE_OUTPUT)0;
	output.position = float4(input.posTex.x, input.posTex.y, 0.0f, 1.0f);
	output.texCoord = float2(input.posTex.z, input.posTex.w);
	output.color = input.color;
	return(output);
}

float4 PSCrowdImage(VS_CROWDIMAGE_OUTPUT input) : SV_Target
{
	float4 cColor = gtxtPS.Sample(gssPS, input.texCoord);
	if (cColor.r <= 0.05f)
		discard;

	float4 retColor;
	if (input.color == 0.0f)
		retColor = cColor;
	if (input.color == 1.0f)
		retColor = float4(0, 1, 0, 1);
	if (input.color == 2.0f)
		retColor = float4(1, 1, 0, 1);
	if (input.color == 3.0f)
		retColor = float4(1, 0, 0, 1);

	return retColor;
	//return cColor;
	//return float4(0, 1, 0, 1);
}


//-------------------------------------------------------------------------------------------------------------------------------
//test용 디버그 그리드
struct VS_DEBUGGRID_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct VS_DEBUGGRID_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VS_DEBUGGRID_OUTPUT VSDebugGrid(VS_DEBUGGRID_INPUT input)
{
	VS_DEBUGGRID_OUTPUT output = (VS_DEBUGGRID_OUTPUT)0;
	output.position = mul(mul(float4(input.position, 1.0f), gmtxView), gmtxProjection);
	output.color = input.color;

	return(output);
}

float4 PSDebugGrid(VS_DEBUGGRID_OUTPUT input) : SV_Target
{
	return input.color;
}

//-------------------------------------------------------------------------------------------------------------------------------
//test용 디버그 큐브
struct VS_DEBUGCUBE_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VS_DEBUGCUBE_OUTPUT VSDebugCube(float4 Position : POSITION)
{
	VS_DEBUGCUBE_OUTPUT output = (VS_DEBUGCUBE_OUTPUT)0;
	output.position = mul(float4(Position.x, Position.y, Position.z, 1.0f), gmtxDebugCubeWorld);
	output.position = mul(output.position, gmtxView);
	output.position = mul(output.position, gmtxProjection);
	output.color = gfDebugCubeColor;

	return output;
}

float4 PSDebugCube(VS_DEBUGCUBE_OUTPUT input) : SV_Target
{
	//return float4(1.0f, 1.0f, 0.0f, 1.0f);
	return input.color;
}

