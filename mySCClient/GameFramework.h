#pragma once

#include "Timer.h"
#include "Scene.h"
#include "Camera.h"

#include "SocketComponent.h"
#include "ProjectedShadow\ShadowMap.h"
#include "SoundComponent\SoundManager.h"
#include "GlobalFunctions.h"

#define		ID_SUBMITIP			10000
#define		ID_LISTROOM			10001
#define		ID_MAKEROOM			10002
#define		ID_JOINROOM			10003

#define		ID_LISTPLAYER		10004
#define		ID_GAMESTART		10005
#define		ID_EXITROOM			10006

struct ROOM_DATA
{
	int id;		//room ID
	int master_id;
	int num_of_people;
};

enum GAME_STATE { NotConnect, Lobby, Room, InGame };

class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	bool CreateRenderTargetDepthStencilView();
	bool CreateDirect3DDisplay();
	
	void BuildObjects();
	void ReleaseObjects();

	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);



	void ClipMouseCursor(HWND hWnd) {
		//마우스 가두기 이벤트
		RECT Clip;
		GetClientRect(hWnd, &Clip);
		ClientToScreen(hWnd, (LPPOINT)&Clip);
		ClientToScreen(hWnd, (LPPOINT)&Clip.right);
		ClipCursor(&Clip);
	}
	//0정지 1위 2아래 3왼쪽 4오른쪽 5왼쪽위 6왼쪽아래 7오른쪽위 8오른쪽아래 9휠업 10휠다운
	void SetCameraMoveState(int move) { m_fCameraMoveState = move; }
	void ProcessMouseMove();


	//소켓 통신
	bool InitializeSocket();
	void CleanUp();
	void ReadPacket(SOCKET sock);
	void ProcessPacket(char* ptr);
	void SendPacket(unsigned char* packet);

	void ProcessConnectError();

	void EnterLobby(bool isExitRoom);
	void UpdateRoomList();
	
	void EnterRoom(bool isBoss);
	void ClearRoomData();
	void UpdateRoomPeopleList();

	void EnterGame();

	void SetFullScreen();
	
	//게임 상태 처리
	int					m_gameState;
	HINSTANCE			m_hInstance;
	HWND				m_hWnd;
	
	//Connect용
	u_short			m_ServerPort;
	char			m_ServerIP[20];
	
	//Lobby
	HWND		hListRoom;
	HWND		hButtonCreate;
	HWND		hButtonJoin;
	std::vector<room_data>	roomList;

	//Room
	HWND		hListPlayer;
	HWND		hButtonStart;
	HWND		hButtonExit;
	bool		m_isRoomMaster;
	std::vector<int>		roomPeopleList;

	//Shadow
	CShadowMap	*m_pShadowMap{ nullptr };
	
private:
	

	ID3D11Device*				   	 m_pd3dDevice;
	IDXGISwapChain*					 m_pDXGISwapChain;
	ID3D11DeviceContext*			 m_pd3dDeviceContext;
	ID3D11RenderTargetView*			 m_pd3dRenderTargetView;

	ID3D11Texture2D*				m_pd3dDepthStencilBuffer;
	ID3D11DepthStencilView*			m_pd3dDepthStencilView;

	CGameTimer		m_GameTimer;
	CScene*			m_pScene;

	_TCHAR			m_pszBuffer[50]; //윈도우 캡션에 출력하기위한 버퍼

	CCamera*		m_pCamera;


	
	//마우스 이벤트 처리
	int				m_fCameraMoveState;
	POINT			m_ptMousePosition;
	short			m_WheelValue;

	
public:	
	int m_nWndClientWidth;
	int m_nWndClientHeight;

	//소켓 통신
	SOCKET&			m_ClientSocket{ CSocketComponent::GetInstance()->m_ClientSocket};
	WSABUF&			m_SendWsabuf{ CSocketComponent::GetInstance()->m_SendWsabuf };
	char*			m_SendBuf{ CSocketComponent::GetInstance()->m_SendBuf};
	WSABUF&			m_RecvWsabuf{ CSocketComponent::GetInstance()->m_RecvWsabuf};
	char*			m_RecvBuf{ CSocketComponent::GetInstance()->m_RecvBuf};
	char*			m_PacketBuf{ CSocketComponent::GetInstance()->m_PacketBuf};

	DWORD&			m_InPacketSize{ CSocketComponent::GetInstance()->m_InPacketSize};
	int&			m_SavedPacketSize{ CSocketComponent::GetInstance()->m_SavedPacketSize};
	
	//client infomation
	int&			m_clientID{ CSocketComponent::GetInstance()->m_clientID};
	int&			m_roomID{ CSocketComponent::GetInstance()->m_roomID};
};

