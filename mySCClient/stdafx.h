// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �Ǵ� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

//**********************************************************************
#pragma once

//���� ���� ���� �׽�Ʈ ����
//#define		DEBUG_NOT_CONNECT_SERVER

//����׿� �� �ε带 ���� �ϴ� ����
//#define			DEBUG_SIMPLE_LOAD

//����� ���ڸ� �׸������� ����
//#define			DEBUG_RENDERING_GRID

//�Ȱ�ó�� ����
//#define				DEBUG_DISABLE_FOG

//���콺 ���α� ����	//-> ���־� ����� ���� ���콺 �浹�� �����ϴϱ� �� Ű����
//#define					ENABLE_CLIP_CURSOR

//���� ���� �� �ܼ� ���� ����
//#define					ENABLE_FREE_CONSOLE

//����� ť�� ������ ����
//#define					DEBUG_RENDERING_CUBE

//�̱� �÷��� ���� ���� �׽�Ʈ
//#define			DEBUG_SINGLE_FIGHT_UNITS

//�����Ҷ� ��üâ, ���콺 Ŀ�� Ŭ�� ����
//#define				DEBUG_WINDOWED_SCREEN

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:
#include <windows.h>

// C ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <algorithm>


#define _XM_NO_INTRINSICS_


// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>		//���̴� ������ �Լ��� ����ϱ� ���� ��� ����
#include <D3DX10Math.h>			//Direct3D ���� �Լ��� ����ϱ� ��
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

//AllocConsole() , FreeConsole()�� ����� ����... _cprintf()
#include <conio.h>



//���� ��ſ� ����� ��ó������ ����
#include <winsock2.h>
#pragma comment (lib, "ws2_32.lib")
//#define		MY_SERVER_IP			"127.0.0.1"
//#define		MY_SERVER_PORT			9000
#define		BUF_SIZE				255
#define		WM_SOCKET				WM_USER + 1

#include "protocol.h"
#include "GlobalFunctions.h"

//*******************************************************************************
//������� ���� ��ȣ
#define				CB_SLOT_CAMERA						0x00		//CB
#define				CB_SLOT_WORLD_MATRIX				0x01		//CB
#define				CB_SLOT_SHADOW						0x02		//CB
#define				CB_SLOT_LIGHT						0x03		//CB
#define				CB_SLOT_MATERIAL					0x04		//CB

#define				CB_SLOT_DEBUGCUBE					0x05		//CB

// ���̴� ���ҽ����� ���� ��ȣ
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

//1213 ����ŧ�� �߰�
#define				PS_SLOT_SPECULAR_TEXTURE			0x09		//TR
#define				PS_SLOT_SPECULAR_SAMPLER			0x09		//SS

//1214 �븻�� �߰�
#define				PS_SLOT_NORMAL_TEXTURE				10			//TR 0x0a
#define				PS_SLOT_NORMAL_SAMPLER				10			//SS 0x0a


//*******************************************************************************
// ��ó�� ������ ����
#define FRAME_BUFFER_WIDTH	640
#define FRAME_BUFFER_HEIGHT	480

#define				MAP_WIDTH						205.0f
#define				MAP_HEIGHT						205.0f

//�׸��� ǰ�� ����� ����
#define				UPSIZE_SHADOW_MAP				4.0f

//�׸��ڸ� ǥ���ϴ� ���⼺ ���� ���� ��ó��
#define				PI								3.1415f / 180.0f
#define				SUNSET_TIME						20.0f
#define				SUN_DISTANCE					20.0f

//�̴ϸ� ����
#define				MINIMAP_POSITION_LEFT			-1.0f + 0.0112f
#define				MINIMAP_POSITION_RIGHT			-1.0f + (2.0f * 0.1526f)
#define				MINIMAP_POSITION_BOTTOM			-1.0f + (0.6 * 0.0413f)
#define				MINIMAP_POSITION_TOP			-1.0f + (0.6f * 0.7965f)

//ī�޶� �̵�
#define			CAMERA_MIN_LOOKAT_X					-(MAP_WIDTH / 2.0f) + 15.0f
#define			CAMERA_MAX_LOOKAT_X					+(MAP_WIDTH / 2.0f) - 15.0f
#define			CAMERA_MIN_LOOKAT_Z					-(MAP_HEIGHT / 2.0f) + 12.0f
#define			CAMERA_MAX_LOOKAT_Z					+(MAP_HEIGHT / 2.0f) - 12.0f

//ī�޶� ��ġ�� ���� ���� ��� ���� �Ÿ�
#define			CAMERA_UNIT_SOUND_DISTANCE			30.0f

//�ڿ� ǥ�� ��ġ
#define			SCREEN_POSITION_MINERAL_X			0.4f
#define			SCREEN_POSITION_MINERAL_Y			0.9f
#define			SCREEN_POSITION_GAS_X				0.4f + 0.3f
#define			SCREEN_POSITION_GAS_Y				0.9f

//���� ����
#define			AIR_UNIT_OFFSET						6.0f

//�ִϸ��̼� Bone
#define			MAX_BONE_COUNT						32

//Fog
#define			NUM_FOG_SLICE						1024.0f

//���� ������ ���� ���� -> �������ݷ� ó��, ���� ���� ���̴� ������ ���� START, END ��
#define			ANIMATE_OBJECT_START				1
#define			ANIMATE_OBJECT_END					255

#define			AIR_OBJECT_START					64
#define			AIR_OBJECT_END						128

#define			STATIC_OBJECT_START					256
#define			STATIC_OBJECT_END					512


#define			MAX_KINDS_OBJECTS					64		//�ִ� ���� (Static ����)
#define			MAX_SELECTED_OBJECTS				24
#define			MAX_INSTANCE_OBJECTS				64		//�� ������ �ִ� ����


#define			DOUBLE_INPUT_DURATION				0.2f	//0.2�� �ȿ� �ٽ� Ű �Է� �� ���� Ű �Է����� �ν��Ѵ�.

//Object Update State
enum OBJ_STATE { alert = 1, move, move_with_alert, have_target };

//Update Time�� �����ϴ� ��ó���� ����
#define		UPDATE_FOG_TIME			0.033f		//30���������� ����... ���� �����ϸ� ������ ���� ���� ��������.
#define		UPDATE_ENEMYLIST_TIME	0.25f		

#define		UPDATE_MOVE_COUNT		1