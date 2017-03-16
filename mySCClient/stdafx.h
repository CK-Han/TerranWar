// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

//**********************************************************************
#pragma once

//서버 연결 없이 테스트 여부
//#define		DEBUG_NOT_CONNECT_SERVER

//디버그용 모델 로드를 적게 하는 여부
//#define			DEBUG_SIMPLE_LOAD

//디버그 격자를 그리는지의 여부
//#define			DEBUG_RENDERING_GRID

//안개처리 여부
//#define				DEBUG_DISABLE_FOG

//마우스 가두기 여부	//-> 가둬야 제대로 위쪽 마우스 충돌이 동작하니까 꼭 키도록
//#define					ENABLE_CLIP_CURSOR

//게임 실행 후 콘솔 종료 여부
//#define					ENABLE_FREE_CONSOLE

//디버그 큐브 렌더링 여부
//#define					DEBUG_RENDERING_CUBE

//싱글 플레이 유닛 전투 테스트
//#define			DEBUG_SINGLE_FIGHT_UNITS

//시작할때 전체창, 마우스 커서 클립 여부
//#define				DEBUG_WINDOWED_SCREEN

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <algorithm>


#define _XM_NO_INTRINSICS_


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>		//쉐이더 컴파일 함수를 사용하기 위한 헤더 파일
#include <D3DX10Math.h>			//Direct3D 수학 함수를 사용하기 위
//#include <DirectXMath.h>
#include <D3D9Types.h>

//using namespace DirectX;

#include <string>
#include <list>
#include <chrono>
#include <vector>
#include <map>
#include <set>
#include <utility>
//#include <atltime.h>

//AllocConsole() , FreeConsole()의 출력을 위해... _cprintf()
#include <conio.h>



//소켓 통신용 헤더와 전처리기의 나열
#include <winsock2.h>
#pragma comment (lib, "ws2_32.lib")
//#define		MY_SERVER_IP			"127.0.0.1"
//#define		MY_SERVER_PORT			9000
#define		BUF_SIZE				255
#define		WM_SOCKET				WM_USER + 1

#include "protocol.h"
#include "GlobalFunctions.h"

//*******************************************************************************
//상수버퍼 슬롯 번호
#define				CB_SLOT_CAMERA						0x00		//CB
#define				CB_SLOT_WORLD_MATRIX				0x01		//CB
#define				CB_SLOT_SHADOW						0x02		//CB
#define				CB_SLOT_LIGHT						0x03		//CB
#define				CB_SLOT_MATERIAL					0x04		//CB

#define				CB_SLOT_DEBUGCUBE					0x05		//CB

// 셰이더 리소스들의 슬롯 번호
#define				PS_SLOT_SHADOW_TEXTURE				0x02		//TR
#define				PS_SLOT_SHADOW_SAMPLER				0x02		//SS

#define				PS_SLOT_TEXTURE						0x03		//TR
#define				PS_SLOT_SAMPLER						0x03		//SS

#define				DS_SLOT_TEXTURE						0x04		//TR
#define				DS_SLOT_SAMPLER						0x04		//SS

#define				VS_SLOT_ANIMATION_STRUCTURED		0x05		//BR

#define				PS_SLOT_FOG_TEXTURE					0x06		//TR
#define				PS_SLOT_FOG_SAMPLER					0x06		//SS

#define				PS_SLOT_FOG							0x07		//BR

#define				PS_SLOT_PARTICLE					0x08		//TR & SS

//1213 스펙큘러 추가
#define				PS_SLOT_SPECULAR_TEXTURE			0x09		//TR
#define				PS_SLOT_SPECULAR_SAMPLER			0x09		//SS

//1214 노말맵 추가
#define				PS_SLOT_NORMAL_TEXTURE				10			//TR 0x0a
#define				PS_SLOT_NORMAL_SAMPLER				10			//SS 0x0a


//*******************************************************************************
// 전처리 내용을 나열
#define FRAME_BUFFER_WIDTH	640
#define FRAME_BUFFER_HEIGHT	480

#define				MAP_WIDTH						205.0f
#define				MAP_HEIGHT						205.0f

//그림자 품질 향상을 위해
#define				UPSIZE_SHADOW_MAP				4.0f

//그림자를 표현하는 방향성 조명 관련 전처리
#define				PI								3.1415f / 180.0f
#define				SUNSET_TIME						20.0f
#define				SUN_DISTANCE					20.0f

//미니맵 관련
#define				MINIMAP_POSITION_LEFT			-1.0f + 0.0112f
#define				MINIMAP_POSITION_RIGHT			-1.0f + (2.0f * 0.1526f)
#define				MINIMAP_POSITION_BOTTOM			-1.0f + (0.6 * 0.0413f)
#define				MINIMAP_POSITION_TOP			-1.0f + (0.6f * 0.7965f)

//카메라 이동
#define			CAMERA_MIN_LOOKAT_X					-(MAP_WIDTH / 2.0f) + 15.0f
#define			CAMERA_MAX_LOOKAT_X					+(MAP_WIDTH / 2.0f) - 15.0f
#define			CAMERA_MIN_LOOKAT_Z					-(MAP_HEIGHT / 2.0f) + 12.0f
#define			CAMERA_MAX_LOOKAT_Z					+(MAP_HEIGHT / 2.0f) - 12.0f

//카메라 위치에 따른 사운드 재생 여부 거리
#define			CAMERA_UNIT_SOUND_DISTANCE			30.0f

//자원 표시 위치
#define			SCREEN_POSITION_MINERAL_X			0.4f
#define			SCREEN_POSITION_MINERAL_Y			0.9f
#define			SCREEN_POSITION_GAS_X				0.4f + 0.3f
#define			SCREEN_POSITION_GAS_Y				0.9f

//공중 유닛
#define			AIR_UNIT_OFFSET						6.0f

//애니메이션 Bone
#define			MAX_BONE_COUNT						32

//Fog
#define			NUM_FOG_SLICE						1024.0f

//유닛 종류에 대한 나열 -> 프로토콜로 처리, 정적 동적 셰이더 구분을 위한 START, END 값
#define			ANIMATE_OBJECT_START				1
#define			ANIMATE_OBJECT_END					255

#define			AIR_OBJECT_START					64
#define			AIR_OBJECT_END						128

#define			STATIC_OBJECT_START					256
#define			STATIC_OBJECT_END					512


#define			MAX_KINDS_OBJECTS					64		//최대 종류 (Static 포함)
#define			MAX_SELECTED_OBJECTS				24
#define			MAX_INSTANCE_OBJECTS				64		//한 종류당 최대 개수


#define			DOUBLE_INPUT_DURATION				0.2f	//0.2초 안에 다시 키 입력 시 더블 키 입력으로 인식한다.

//Object Update State
enum OBJ_STATE { alert = 1, move, move_with_alert, have_target };

//Update Time을 관리하는 전처리기 값들
#define		UPDATE_FOG_TIME			0.033f		//30프레임으로 갱신... 자주 갱신하면 프레임 정말 많이 떨어진다.
#define		UPDATE_ENEMYLIST_TIME	0.25f		

#define		UPDATE_MOVE_COUNT		1