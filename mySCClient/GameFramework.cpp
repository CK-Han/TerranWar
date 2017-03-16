#include "stdafx.h"
#include "GameFramework.h"


CGameFramework::CGameFramework()
{
	m_pd3dDevice = NULL;
	m_pDXGISwapChain = NULL;
	m_pd3dRenderTargetView = NULL;
	m_pd3dDeviceContext = NULL;

	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;

	m_pScene = NULL;
	_tcscpy_s(m_pszBuffer, "SCClient (");

	m_pCamera = nullptr;

	m_pd3dDepthStencilBuffer = nullptr;
	m_pd3dDepthStencilView = nullptr;

	WSADATA	wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

#ifndef DEBUG_NOT_CONNECT_SERVER
	m_gameState = GAME_STATE::NotConnect;
#else
	m_gameState = GAME_STATE::InGame;
#endif

	m_ServerPort = 9000;
	
	m_WheelValue = 0;

	m_clientID = -1;
	m_roomID = -1;
	m_isRoomMaster = false;

	//방, 방장, 인원수
	roomList.reserve(MAX_ROOM);
	roomPeopleList.reserve(MAX_ROOM_PEOPLE);

	CSocketComponent::GetInstance();
}


CGameFramework::~CGameFramework()
{
	if(CSocketComponent::GetInstance())
		CSocketComponent::GetInstance()->DeleteInstance();
	CleanUp();
//#ifdef _DEBUG
	FreeConsole();
//#endif
}

//**********************************************************************
//응용 프로그램 실행과 함께 호출, D3D 인터페이스 생성
bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	if (!CreateDirect3DDisplay()) return(false);

#ifndef DEBUG_NOT_CONNECT_SERVER
	//InitializeSocket();
#else
	m_gameState = GAME_STATE::InGame;
	EnterGame();
#endif


	return(true);
}

//다음 함수는 응용 프로그램이 종료될 때 호출된다는 것에 유의하라. 
void CGameFramework::OnDestroy()
{
	ReleaseCapture();

	//게임 객체를 소멸한다.
	if(GAME_STATE::InGame == m_gameState)
		ReleaseObjects();

	//CleanSocket();

	//Direct3D와 관련된 객체를 소멸한다. 
	if (m_pd3dDeviceContext) m_pd3dDeviceContext->ClearState();
	if (m_pd3dRenderTargetView) m_pd3dRenderTargetView->Release();
	if (m_pDXGISwapChain) m_pDXGISwapChain->Release();
	if (m_pd3dDeviceContext) m_pd3dDeviceContext->Release();
	if (m_pd3dDevice) m_pd3dDevice->Release();

	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
	if (m_pd3dDepthStencilView) m_pd3dDepthStencilView->Release();

	//Shadow
	if (m_pShadowMap) delete m_pShadowMap;

}


//**********************************************************************
// D3D 디바이스 생성 코드, 뷰포트 설정
bool CGameFramework::CreateDirect3DDisplay()
{
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	m_nWndClientWidth = rcClient.right - rcClient.left;
	m_nWndClientHeight = rcClient.bottom - rcClient.top;

	UINT dwCreateDeviceFlags = 0;
#ifdef _DEBUG
	dwCreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	//디바이스를 생성하기 위하여 시도할 드라이버 유형의 순서를 나타낸다.
	D3D_DRIVER_TYPE d3dDriverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	UINT nDriverTypes = sizeof(d3dDriverTypes) / sizeof(D3D10_DRIVER_TYPE);

	//디바이스를 생성하기 위하여 시도할 특성 레벨의 순서를 나타낸다.
	D3D_FEATURE_LEVEL d3dFeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	UINT nFeatureLevels = sizeof(d3dFeatureLevels) / sizeof(D3D_FEATURE_LEVEL);

	//생성할 스왑 체인을 서술하는 구조체이다.
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	dxgiSwapChainDesc.BufferCount = 1;
	dxgiSwapChainDesc.BufferDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow = m_hWnd;
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;
	dxgiSwapChainDesc.Windowed = true;

	D3D_DRIVER_TYPE nd3dDriverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL nd3dFeatureLevel = D3D_FEATURE_LEVEL_11_0;

	HRESULT hResult = S_OK;
	//디바이스의 드라이버 유형과 특성 레벨을 지원하는 디바이스와 스왑 체인을 생성한다. 고수준의 디바이스 생성을 시도하고 실패하면 다음 수준의 디바이스를 생성한다.
	for (UINT i = 0; i < nDriverTypes; i++)
	{
		nd3dDriverType = d3dDriverTypes[i];
		if (SUCCEEDED(hResult = D3D11CreateDeviceAndSwapChain(NULL, nd3dDriverType, NULL,
			dwCreateDeviceFlags, d3dFeatureLevels, nFeatureLevels, D3D11_SDK_VERSION, &dxgiSwapChainDesc,
			&m_pDXGISwapChain, &m_pd3dDevice, &nd3dFeatureLevel, &m_pd3dDeviceContext))) break;
	}
	if (!m_pDXGISwapChain || !m_pd3dDevice || !m_pd3dDeviceContext) return(false);

	//렌더 타겟 뷰를 생성하는 함수를 호출한다.
	if (!CreateRenderTargetDepthStencilView()) return(false);

	return(true);
}

bool CGameFramework::CreateRenderTargetDepthStencilView()
{
	HRESULT hResult = S_OK;

	//스왑 체인의 후면버퍼에 대한 렌더 타겟 뷰를 생성한다.
	ID3D11Texture2D *pd3dBackBuffer;
	if (FAILED(hResult = m_pDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pd3dBackBuffer))) return(false);
	if (FAILED(hResult = m_pd3dDevice->CreateRenderTargetView(pd3dBackBuffer, NULL, &m_pd3dRenderTargetView))) return(false);
	if (pd3dBackBuffer) pd3dBackBuffer->Release();
	//후면버퍼 렌더타겟 생성 완료

	//렌더 타겟과 같은 크기의 깊이 버퍼(Depth Buffer)를 생성한다.
	D3D11_TEXTURE2D_DESC d3dDepthStencilBufferDesc;
	ZeroMemory(&d3dDepthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
	d3dDepthStencilBufferDesc.Width = m_nWndClientWidth;
	d3dDepthStencilBufferDesc.Height = m_nWndClientHeight;
	d3dDepthStencilBufferDesc.MipLevels = 1;
	d3dDepthStencilBufferDesc.ArraySize = 1;
	d3dDepthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dDepthStencilBufferDesc.SampleDesc.Count = 1;
	d3dDepthStencilBufferDesc.SampleDesc.Quality = 0;
	d3dDepthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dDepthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	d3dDepthStencilBufferDesc.CPUAccessFlags = 0;
	d3dDepthStencilBufferDesc.MiscFlags = 0;
	if (FAILED(hResult = m_pd3dDevice->CreateTexture2D(&d3dDepthStencilBufferDesc, NULL, &m_pd3dDepthStencilBuffer))) return(false);

	//생성한 깊이 버퍼(Depth Buffer)에 대한 뷰를 생성한다.
	D3D11_DEPTH_STENCIL_VIEW_DESC d3dViewDesc;
	ZeroMemory(&d3dViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	d3dViewDesc.Format = d3dDepthStencilBufferDesc.Format;
	d3dViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	d3dViewDesc.Texture2D.MipSlice = 0;
	if (FAILED(hResult = m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, &d3dViewDesc, &m_pd3dDepthStencilView))) return(false);
	//Depth-Stencil 버퍼 생성 완료

	//출력병합기에 연결
	m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView);
	return(true);
}


//**********************************************************************
//사용자 입력 처리
void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	DWORD MouseCrushState = 0x0000; //위 아래 왼 오른
	
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		break;
	case WM_LBUTTONDBLCLK:
		break;
	case WM_RBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		break;
	case WM_MOUSEMOVE:
	case WM_SETCURSOR:
		if (GAME_STATE::InGame == m_gameState)
		{
#ifdef ENABLE_CLIP_CURSOR
			ClipMouseCursor(hWnd);
#endif // _DEBUG

		}
		GetCursorPos(&m_ptMousePosition);
		ScreenToClient(m_hWnd, &m_ptMousePosition);
		m_ptMousePosition.y = m_nWndClientHeight - m_ptMousePosition.y;

		if (m_ptMousePosition.x <= 30) //왼쪽 벽에 부딪혔다면
			MouseCrushState |= 0x0010;
		else if (m_nWndClientWidth - m_ptMousePosition.x <= 30) //오른쪽 벽에 부딪혔다면
			MouseCrushState |= 0x0001;
		if (m_ptMousePosition.y <= 30) //아래 벽에 부딪혔다면
			MouseCrushState |= 0x0100;
		else if (m_nWndClientHeight - m_ptMousePosition.y <= 30) //위 벽에 부딪혔다면
			MouseCrushState |= 0x1000;

		switch (MouseCrushState)
		{
		case 0x0001: // 오른쪽
			//SetCursor(m_hCurSizeWe);
			SetCameraMoveState(3);
			break;
		case 0x0010: //왼쪽
			//SetCursor(m_hCurSizeWe);
			SetCameraMoveState(4);
			break;
		case 0x1000: //위
			//SetCursor(m_hCurSizeNs);
			SetCameraMoveState(2);
			break;
		case 0x0100: //아래
			//SetCursor(m_hCurSizeNs);
			SetCameraMoveState(1);
			break;
		case 0x0110: // 왼쪽 아래
			//SetCursor(m_hCurSizeNesw);
			SetCameraMoveState(7);
			break;
		case 0x1001: // 오른쪽 위
			//SetCursor(m_hCurSizeNesw);
			SetCameraMoveState(6);
			break;
		case 0x0101: // 오른쪽 아래
			//SetCursor(m_hCurSizeNwse);
			SetCameraMoveState(5);
			break;
		case 0x1010: // 왼쪽 위
			//SetCursor(m_hCurSizeNwse);
			SetCameraMoveState(8);
			break;
		default: // 평상시
			//SetCursor(LoadCursor(NULL, IDC_ARROW));
			SetCameraMoveState(0);
			break;
		}
		break;

	case WM_MOUSEWHEEL:
#ifdef ENABLE_CLIP_CURSOR
		ClipMouseCursor(hWnd);
#endif // !_DEBUF
		m_WheelValue = GET_WHEEL_DELTA_WPARAM(wParam); // 0~120, 양수는 앞쪽, 음수는 뒤쪽
		m_WheelValue >= 0 ? SetCameraMoveState(9) : SetCameraMoveState(10);
		break;
	default:
		break;
	}
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_NUMPAD1:
			m_pCamera->SetPosition(m_pCamera->GetPosition() + D3DXVECTOR3(0.0f, 50.0f, 0.0f));
			m_pCamera->GenerateViewMatrix();
			break;
		case VK_NUMPAD2:
			m_pCamera->SetPosition(m_pCamera->GetPosition() + D3DXVECTOR3(0.0f, -50.0f, 0.0f));
			m_pCamera->GenerateViewMatrix();
			break;
		case VK_ESCAPE:
			if (0x8000 & GetAsyncKeyState(VK_SHIFT))
				PostQuitMessage(0);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	int ret;
	DWORD iobyte;
	HDC hDC;

	RECT Clip;

	switch (nMessageID)
	{
	case WM_SIZE:
	{
		m_nWndClientWidth = LOWORD(lParam);
		m_nWndClientHeight = HIWORD(lParam);
		
		if (m_pScene) m_pScene->SetWindowSize(m_nWndClientWidth, m_nWndClientHeight);

		m_pd3dDeviceContext->OMSetRenderTargets(0, NULL, NULL);

		if (m_pd3dRenderTargetView) m_pd3dRenderTargetView->Release();

		//왜 count를 2로 두지? 1 아닌감 msdn은 swap을 위해 2 미만이 될 순 없다고 하는데 1 넣어도 돌아가는데 뭘까
		m_pDXGISwapChain->ResizeBuffers(2, m_nWndClientWidth, m_nWndClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

		CreateRenderTargetDepthStencilView();
		if (m_pCamera) m_pCamera->SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);
		if (m_pShadowMap) m_pShadowMap->ResizeWindow(m_pd3dDevice, m_nWndClientWidth, m_nWndClientHeight);

		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_MOUSEWHEEL:
	case WM_SETCURSOR:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		if (m_pScene) m_pScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		if (m_pScene) m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;

	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
			case ID_LISTROOM:
			{
				switch (HIWORD(wParam)) 
				{
				case LBN_SELCHANGE:
				{
					int i = SendMessage(hListRoom, LB_GETCURSEL, 0, 0);
					if (i <= -1)
						break;
					char str[30];
					sprintf_s(str, 30, "방장id : %d, 인원 %d", roomList[i].master_id, roomList[i].num_of_people);
					HDC dc = GetDC(hWnd);
					TextOut(dc, 150, 30, str, strlen(str));
					ReleaseDC(hWnd, dc);
					break;
				}
				}
			}
			break;
			case ID_MAKEROOM:
			{
				cs_packet_make_room* my_packet = reinterpret_cast<cs_packet_make_room *>(m_SendBuf);
				my_packet->size = sizeof(cs_packet_make_room);
				my_packet->type = CS_MAKE_ROOM;
				SendPacket(reinterpret_cast<unsigned char*>(my_packet));
				EnterRoom(true);
			}
			break;
			case ID_JOINROOM:
			{
				int i = SendMessage(hListRoom, LB_GETCURSEL, 0, 0);
				if (i <= -1)
				{
					//MessageBox(hWnd, "방을 선택하셔야죠!!", "SelectRoom", MB_OK);
					break;
				}
				cs_packet_enter_room* my_packet = reinterpret_cast<cs_packet_enter_room *>(m_SendBuf);
				my_packet->size = sizeof(cs_packet_enter_room);
				my_packet->type = CS_ENTER_ROOM;
				my_packet->room_id = roomList[i].id;
				SendPacket(reinterpret_cast<unsigned char*>(my_packet));
				EnterRoom(false);
			}
			break;
			case ID_EXITROOM:
			{
				cs_packet_exit_room* my_packet = reinterpret_cast<cs_packet_exit_room *>(m_SendBuf);
				my_packet->size = sizeof(cs_packet_exit_room);
				my_packet->type = CS_EXIT_ROOM;
				my_packet->room_id = m_roomID;
				SendPacket(reinterpret_cast<unsigned char*>(my_packet));
				if(false == m_isRoomMaster)	//방장이 아니라면
					EnterLobby(true);
				//나가기 처리는 ProcessPacket에서 진행
			}
			break;
			case ID_GAMESTART:
			{
				cs_packet_start_room* my_packet = reinterpret_cast<cs_packet_start_room *>(m_SendBuf);
				my_packet->size = sizeof(cs_packet_start_room);
				my_packet->type = CS_START_ROOM;
				my_packet->room_id = m_roomID;
				SendPacket(reinterpret_cast<unsigned char*>(my_packet));
				//EnterGame();			
			}
			break;
		}
	}	break; // WM_COMMAND

	case WM_SOCKET:
	{
		if (WSAGETSELECTERROR(lParam)) {
			closesocket((SOCKET)wParam);
			exit(-1);
			break;
		}
		switch (WSAGETSELECTEVENT(lParam))
		{
		case FD_READ:
			ReadPacket((SOCKET)wParam);
			break;
		case FD_CLOSE:
			closesocket((SOCKET)wParam);
			exit(-1);
			break;
		}
	}
	default:
		break;
	}

	return false;
}

void CGameFramework::ProcessInput()
{
	static UCHAR pKeysBuffer[256];
	bool bProcessedByScene = false;
	GetKeyboardState(pKeysBuffer);
	//if (GetKeyboardState(pKeysBuffer) && m_pScene) bProcessedByScene = m_pScene->ProcessInput(pKeysBuffer);
	if (!bProcessedByScene)
	{
		DWORD dwDirection = 0;
		if (pKeysBuffer[VK_ADD] & 0xF0) SetCameraMoveState(9);
		if (pKeysBuffer[VK_SUBTRACT] & 0xF0) SetCameraMoveState(10);
	}

	//방향키 이벤트 처리
	//0x0000 -> 누른적X현재X / 0x0001 -> 누른적O현재X / 0x8000 -> 누른적X현재O / 0x8001 -> 누른적O현재O
	bool isKeyboardArrowPressed = true;
	int inputKeyboardArrow = 0x0000;	// l r u d
	if (0x8000 & GetAsyncKeyState(VK_LEFT)) inputKeyboardArrow |= 0x1000;
	if (0x8000 & GetAsyncKeyState(VK_RIGHT)) inputKeyboardArrow |= 0x0100;
	if (0x8000 & GetAsyncKeyState(VK_UP)) inputKeyboardArrow |= 0x0010;
	if (0x8000 & GetAsyncKeyState(VK_DOWN)) inputKeyboardArrow |= 0x0001;
	
	float MoveSpeed = 100.0f * m_GameTimer.GetTimeElapsed();

	D3DXVECTOR3 vRight = m_pCamera->GetRightVector();
	D3DXVECTOR3 vLook = m_pCamera->GetLookVector();
	D3DXVECTOR3 vDiagonal;

	D3DXVECTOR3 vMove;

	switch (inputKeyboardArrow)
	{
	case 0x1000:	//left
		vMove = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); break;
	case 0x0100:	//right
		vMove = D3DXVECTOR3(+1.0f, 0.0f, 0.0f); break;
	case 0x0010:	//up
		vMove = D3DXVECTOR3(0.0f, 0.0f, +1.0f); break;
	case 0x0001:	//down
		vMove = D3DXVECTOR3(0.0f, 0.0f, -1.0f); break;
	case 0x1010:	//left up
		vMove = D3DXVECTOR3(-1.0f, 0.0f, +1.0f); break;
	case 0x1001:	//left down
		vMove = D3DXVECTOR3(-1.0f, 0.0f, -1.0f); break;
	case 0x0110:	//right up
		vMove = D3DXVECTOR3(+1.0f, 0.0f, +1.0f); break;
	case 0x0101:	//right down
		vMove = D3DXVECTOR3(+1.0f, 0.0f, -1.0f); break;

	default: isKeyboardArrowPressed = false; // 종료
	}

	if(isKeyboardArrowPressed)
	{
		D3DXVECTOR3 prevPos = m_pCamera->GetPosition();
		D3DXVECTOR3 prevLookAtWorld = m_pCamera->GetLookAtPosition();

		m_pCamera->Move(vMove * MoveSpeed);
		m_pCamera->SetLookAtWorld(m_pCamera->GetLookAtPosition() + vMove * MoveSpeed);
		m_pCamera->GenerateViewMatrix();
		
		//임시로 꺼둠
		if (m_pScene->isCameraMoveOverMap())
		{
			m_pCamera->SetPosition(prevPos);
			m_pCamera->SetLookAtWorld(prevLookAtWorld);
			m_pCamera->GenerateViewMatrix();
		}
	}
}

//**********************************************************************
//오브젝트 관리
void CGameFramework::BuildObjects()
{
	CCamera* pCamera = new CCamera();
	pCamera->CreateShaderVariables(m_pd3dDevice);
	pCamera->SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);

	pCamera->SetLookAtWorld(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	pCamera->GenerateViewMatrix();
	//9.027977f
	pCamera->GenerateProjectionMatrix(10.0f, 1000.0f, float(m_nWndClientWidth) / float(m_nWndClientHeight), 9.027977f);

	m_pCamera = pCamera;

	m_pScene = new CScene(this);
	m_pScene->m_pd3dDeviceContext = m_pd3dDeviceContext;
	if (m_pScene && m_pCamera) m_pScene->SetCamera(m_pCamera);
	if (m_pScene) m_pScene->BuildObjects(m_pd3dDevice);
	m_pShadowMap = new CShadowMap(m_pd3dDevice, m_nWndClientWidth, m_nWndClientHeight);

#ifndef DEBUG_WINDOWED_SCREEN
	SetFullScreen();
	ClipMouseCursor(m_hWnd);
#endif

#ifndef DEBUG_NOT_CONNECT_SERVER
	CSocketComponent::GetInstance()->m_bGameStarted = false;
	cs_packet_game_ready* my_packet = reinterpret_cast<cs_packet_game_ready *>(m_SendBuf);
	my_packet->size = sizeof(cs_packet_game_ready);
	my_packet->type = CS_GAME_READY;
	my_packet->client_id = CSocketComponent::GetInstance()->m_clientID;
	my_packet->room_id = CSocketComponent::GetInstance()->m_roomID;
	SendPacket(reinterpret_cast<unsigned char*>(my_packet));
#else
	CSocketComponent::GetInstance()->m_bGameStarted = true;
#endif	
}

void CGameFramework::ReleaseObjects()
{
	//if (m_pScene) m_pScene->ReleaseObjects();	//Scene의 소멸자에서 진행한다.
	if (m_pScene) delete m_pScene;
	if (m_pCamera) delete m_pCamera;
}


//**********************************************************************
//게임 내 흐름
void CGameFramework::AnimateObjects()
{
	if (m_pCamera) ProcessMouseMove();
	if (m_pScene)
	{
		m_pScene->UpdateObject(m_GameTimer.GetTimeElapsed());	//지형 위 안착
		m_pScene->Animate(m_GameTimer.GetTimeElapsed());
	}
}

void CGameFramework::FrameAdvance()
{
	if(CSocketComponent::GetInstance()->m_bGameStarted)
	{
		m_GameTimer.Tick();
		ProcessInput();
		AnimateObjects();

		//투영 텍스처 렌더링 + 그림자 상수버퍼 Set
		m_pShadowMap->OnCreateShadowMap(m_pd3dDeviceContext, m_pScene, m_pd3dDepthStencilView);
		m_pShadowMap->UpdateShadowConstantBuffer(m_pd3dDeviceContext);

		float fClearColor[4] = { 0.05f, 0.05f, 0.05f, 0.0f };
		if (m_pd3dRenderTargetView) m_pd3dDeviceContext->ClearRenderTargetView(m_pd3dRenderTargetView, fClearColor);
		if (m_pd3dDepthStencilView) m_pd3dDeviceContext->ClearDepthStencilView(m_pd3dDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
		if (m_pCamera) 
		{
			m_pCamera->SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);
			m_pCamera->UpdateShaderVariables(m_pd3dDeviceContext);
		}

		m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView);

		m_pd3dDeviceContext->PSSetSamplers(PS_SLOT_SHADOW_SAMPLER, 1, &m_pShadowMap->m_pd3dShadowSamplerState);
		m_pd3dDeviceContext->PSSetShaderResources(PS_SLOT_SHADOW_TEXTURE, 1, &m_pShadowMap->m_pd3dProjectedTextureSRV);
	
		if (m_pScene) m_pScene->Render(m_pd3dDeviceContext, m_pCamera);
	
		m_pDXGISwapChain->Present(0, 0);

		// "SCClient ("     <-- prefix
		m_GameTimer.GetFrameRate(&m_pszBuffer[10], 50);
		SetWindowTextW(m_hWnd, (LPCWSTR)m_pszBuffer);
	}
}

//**********************************************************************
//기능 구현 사항입니다.
void CGameFramework::ProcessMouseMove()
{
	if (0 == m_fCameraMoveState)
		return;
	//마우스 이동 처리
	float MouseSpeed = 50.0f * m_GameTimer.GetTimeElapsed();

	D3DXVECTOR3 vRight = m_pCamera->GetRightVector();
	D3DXVECTOR3 vLook = m_pCamera->GetLookVector();
	
	D3DXVECTOR3 vMove;

	switch (m_fCameraMoveState)
	{
	case 1: // 위로 카메라 이동, 실제로는 +z로 움직인다.
		vMove = D3DXVECTOR3(0.0f, 0.0f, -1.0f);  break;
	case 2: // -z
		vMove = D3DXVECTOR3(0.0f, 0.0f, +1.0f); break;
	case 3: // -x
		vMove = D3DXVECTOR3(+1.0f, 0.0f, 0.0f); break;
	case 4: // +x
		vMove = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); break;
	case 5: // -x +z	 //normalize안해도 됨 약 1.414배로 움직임
		vMove = D3DXVECTOR3(+1.0f, 0.0f, -1.0f); break;
	case 6: // -x -z
		vMove = D3DXVECTOR3(+1.0f, 0.0f, +1.0f); break;
	case 7: // +x +z
		vMove = D3DXVECTOR3(-1.0f, 0.0f, -1.0f); break;
	case 8: // +x -z
		vMove = D3DXVECTOR3(-1.0f, 0.0f, +1.0f); break;
	case 9: //ZoomIn
		vMove = m_pCamera->GetLookVector(); MouseSpeed /= 4.0f;  break;
	case 10: //ZoomOut
		vMove = -(m_pCamera->GetLookVector()); MouseSpeed /= 4.0f; break;
	}

	D3DXVECTOR3 prevPos = m_pCamera->GetPosition();
	D3DXVECTOR3 prevLookAtWorld = m_pCamera->GetLookAtPosition();

	m_pCamera->Move(vMove * MouseSpeed);
	m_pCamera->SetLookAtWorld(m_pCamera->GetLookAtPosition() + vMove * MouseSpeed);
	m_pCamera->GenerateViewMatrix();


	//임서로 꺼놓음
	if (m_pScene->isCameraMoveOverMap())
	{
		m_pCamera->SetPosition(prevPos);
		m_pCamera->SetLookAtWorld(prevLookAtWorld);
		m_pCamera->GenerateViewMatrix();
	}
}

void CGameFramework::SetFullScreen()
{
	/*
	int xScreen = GetSystemMetrics(SM_CXSCREEN);
	int yScreen = GetSystemMetrics(SM_CYSCREEN);

	m_nWndClientWidth = xScreen;
	m_nWndClientHeight = yScreen;

	if (m_pScene) m_pScene->SetWindowSize(m_nWndClientWidth, m_nWndClientHeight);

	m_pd3dDeviceContext->OMSetRenderTargets(0, NULL, NULL);

	if (m_pd3dRenderTargetView) m_pd3dRenderTargetView->Release();

	m_pDXGISwapChain->ResizeBuffers(2, m_nWndClientWidth, m_nWndClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	CreateRenderTargetDepthStencilView();
	if (m_pCamera) m_pCamera->SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);
	if (m_pShadowMap) m_pShadowMap->ResizeWindow(m_pd3dDevice, m_nWndClientWidth, m_nWndClientHeight);
	*/
	
	/*
	LPRECT rc;
	int windowWidth = GetSystemMetrics(SM_CXSCREEN);
	int windowHeight = GetSystemMetrics(SM_CYSCREEN);
	
	int style = ::GetWindowLong(this->m_hWnd, GWL_STYLE);
	GetWindowRect(m_hWnd, rc);
	SetWindowLong(m_hWnd, GWL_STYLE, NULL);
	SetWindowPos(m_hWnd, 0, 0, 0, windowWidth, windowHeight, NULL);
	ShowWindow(m_hWnd, SW_MAXIMIZE);

	WORD xScreen = (WORD)GetSystemMetrics(SM_CXSCREEN);
	WORD yScreen = (WORD)GetSystemMetrics(SM_CYSCREEN);
	DWORD size = ((DWORD)yScreen << 16) + xScreen;
	SendMessage(m_hWnd, WM_SIZE, 0, LPARAM(size));
	*/
}

//Lobby*************************************************
void CGameFramework::EnterLobby(bool isExitRoom)
{
	if (isExitRoom)
		ClearRoomData();

	m_gameState = GAME_STATE::Lobby;

	hListRoom =CreateWindow("listbox", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY
		, 10, 10, 100, 200, m_hWnd, (HMENU)ID_LISTROOM, m_hInstance, NULL);
	
	for (auto i = 0; i < roomList.size(); ++i)
	{
		char Buf[20];
		sprintf_s(Buf, 20, "RoomID : %d", roomList[i].id);
		SendMessage(hListRoom, LB_ADDSTRING, 0, (LPARAM)Buf);
	}
	
	hButtonCreate = CreateWindow("button", "Create Room", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		10, 220, 100, 50, m_hWnd, (HMENU)ID_MAKEROOM, m_hInstance, NULL);

	hButtonJoin = CreateWindow("button", "Join Room", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		10, 280, 100, 50, m_hWnd, (HMENU)ID_JOINROOM, m_hInstance, NULL);

	m_isRoomMaster = false;
	InvalidateRect(m_hWnd, nullptr, true);
}

void CGameFramework::UpdateRoomList()
{
	SendMessage(hListRoom, LB_RESETCONTENT, 0, 0);
	for (auto i = 0; i < roomList.size(); ++i)
	{
		char str[30];
		sprintf_s(str, 30, "Room ID : %d", roomList[i].id);
		SendMessage(hListRoom, LB_ADDSTRING, 0, (LPARAM)str);
	}
	InvalidateRect(m_hWnd, nullptr, true);
}


//InRoom*****************************************************
void CGameFramework::EnterRoom(bool isBoss)
{
	m_gameState = GAME_STATE::Room;

	DestroyWindow(hListRoom);
	DestroyWindow(hButtonCreate);
	DestroyWindow(hButtonJoin);
	roomList.clear();

	hListPlayer = CreateWindow("listbox", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY
		, 10, 10, 300, 300, m_hWnd, (HMENU)ID_LISTPLAYER, m_hInstance, NULL);
	
	if (true == isBoss)
	{
		m_isRoomMaster = true;
		hButtonStart = CreateWindow("button", "Game Start", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			m_nWndClientWidth - 120, m_nWndClientHeight - 120, 100, 50, m_hWnd, (HMENU)ID_GAMESTART, m_hInstance, NULL);
	}

	hButtonExit = CreateWindow("button", "Exit", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		m_nWndClientWidth - 120, m_nWndClientHeight - 60, 100, 50, m_hWnd, (HMENU)ID_EXITROOM, m_hInstance, NULL);
}

void CGameFramework::UpdateRoomPeopleList()
{
	SendMessage(hListPlayer, LB_RESETCONTENT, 0, 0);
	for (auto i = 0; i < roomPeopleList.size(); ++i)
	{
		char str[30];
		sprintf_s(str, 30, "유저 : %d", roomPeopleList[i]);
		SendMessage(hListPlayer, LB_ADDSTRING, 0, (LPARAM)str);
	}
	InvalidateRect(m_hWnd, nullptr, true);
}

void CGameFramework::ClearRoomData()
{
	roomPeopleList.clear();
	DestroyWindow(hListPlayer);
	DestroyWindow(hButtonExit);
	if (true == m_isRoomMaster)
		DestroyWindow(hButtonStart);
}

void CGameFramework::EnterGame()
{
	
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	printf("Now Loading...\n");

	m_gameState = GAME_STATE::InGame;
	ClearRoomData();
	BuildObjects();

	//디버그 모드라면 로딩 이후로도 콘솔창을 사용한다.
#ifndef _DEBUG
	//FreeConsole();
#endif

	/*
	m_nWndClientWidth = GetSystemMetrics(SM_CXSCREEN);
	m_nWndClientHeight = GetSystemMetrics(SM_CYSCREEN);

	m_pd3dDeviceContext->OMSetRenderTargets(0, NULL, NULL);
	if (m_pd3dRenderTargetView) m_pd3dRenderTargetView->Release();

	m_pDXGISwapChain->ResizeBuffers(2, m_nWndClientWidth, m_nWndClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	CreateRenderTargetDepthStencilView();
	if (m_pCamera) m_pCamera->SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);

	ShowWindow(m_hWnd, SW_SHOWMAXIMIZED);
	*/
}

//**********************************************************************
//통신 처리 관련 함수들의 목록입니다.
bool CGameFramework::InitializeSocket()
{
	m_ClientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(m_ServerPort);
	ServerAddr.sin_addr.s_addr = inet_addr(m_ServerIP);

	int Result = WSAConnect(m_ClientSocket, (sockaddr *)&ServerAddr, sizeof(ServerAddr), nullptr, nullptr, nullptr, nullptr);
	if (0 != Result)
	{
		MessageBox(m_hWnd, "WSAConnect Error!!", "Error!!", MB_OK);
		ProcessConnectError();
		return false;
	}

	WSAAsyncSelect(m_ClientSocket, m_hWnd, WM_SOCKET, FD_CLOSE | FD_READ);

	m_SendWsabuf.buf = m_SendBuf;
	m_SendWsabuf.len = BUF_SIZE;
	m_RecvWsabuf.buf = m_RecvBuf;
	m_RecvWsabuf.len = BUF_SIZE;

	return true;
}

void CGameFramework::CleanUp()
{
	WSACleanup();
}

void CGameFramework::ProcessConnectError()
{
	if (m_ClientSocket) closesocket(m_ClientSocket);
}

void CGameFramework::ReadPacket(SOCKET sock)
{
	DWORD iobyte, ioflag = 0;
	int ret = WSARecv(sock, &m_RecvWsabuf, 1, &iobyte, &ioflag, nullptr, nullptr);
	if (ret)
	{
		int err_code = WSAGetLastError();
		char buf[50];
		sprintf_s(buf, 50, "WSARecv Error [%d]", err_code);
		MessageBox(m_hWnd, buf, "Error!!", MB_OK);
	}

	BYTE *ptr = reinterpret_cast<BYTE *>(m_RecvBuf);

	while (0 != iobyte) {
		if (0 == m_InPacketSize) m_InPacketSize = ptr[0];
		if (iobyte + m_SavedPacketSize >= m_InPacketSize) {
			memcpy(m_PacketBuf + m_SavedPacketSize, ptr, m_InPacketSize - m_SavedPacketSize);
			ProcessPacket(m_PacketBuf);
			ptr += m_InPacketSize - m_SavedPacketSize;
			iobyte -= m_InPacketSize - m_SavedPacketSize;
			m_InPacketSize = 0;
			m_SavedPacketSize = 0;
		}
		else {
			memcpy(m_PacketBuf + m_SavedPacketSize, ptr, iobyte);
			m_SavedPacketSize += iobyte;
			iobyte = 0;
		}
	}
}

void CGameFramework::ProcessPacket(char *ptr)
{
	//패킷 처리 switch

	switch (ptr[1])
	{
	case SC_FAIL:
	{
		/*
		sc_packet_fail* my_packet = reinterpret_cast<sc_packet_fail*>(ptr);
		if ((int)ERROR_NO::MAKE_ROOM == my_packet->err_no);
//			MessageBox(m_hWnd, "방을 생성할 수 없습니다.", "MaxRoom", MB_OK);
		else if ((int)ERROR_NO::MAKE_ROOM == my_packet->err_no);
//			MessageBox(m_hWnd, "접속할 수 없습니다.", "MaxRoomPeople", MB_OK);
		*/
		sc_packet_fail* my_packet = reinterpret_cast<sc_packet_fail*>(ptr);
		if ((int)ERROR_NO::CREATE_BUILDING == my_packet->err_no)
		{
			printf("건물을 지을 수 없답니다.\n");
			CSoundManager::GetInstance()->PlayEffectSound(start_offset::cannot_build);
		}
		printf("SC_FAIL을 받았습니다.\n");
	}
	break;

	case SC_EXIT_CLIENT: //게임도중 나감
	{
		printf("상대방이 나갔습니다!\n");
		if(false == m_pScene->m_bGameEnd)
		{
			m_pScene->m_bGameEnd = true;
			m_pScene->m_bGameWin = true;
		}
	}
	break;

	case SC_FAIL_OBJECT_MOVE:
	{
		sc_packet_fail_object_move* my_packet = reinterpret_cast<sc_packet_fail_object_move*>(ptr);
		m_pScene->Process_SC_Packet_fail_Object_Move(my_packet);
	}
	break;
	
	case SC_CLIENT_DATA:
	{
		sc_packet_client_data* my_packet = reinterpret_cast<sc_packet_client_data*>(ptr);
		m_clientID = my_packet->client_id;
		CSocketComponent::GetInstance()->m_clientID = m_clientID;
	}
	break;
	case SC_ROOM_ALL_DATA:
	{
		sc_packet_room_datas* my_packet = reinterpret_cast<sc_packet_room_datas*>(ptr);
		roomList.clear();

		//페이지 적용 전
		int page = my_packet->page;
		room_data room_datas[MAX_ROOM_DATA];

		for (auto i = 0; i < MAX_ROOM_DATA; ++i)
		{
			roomList.push_back(my_packet->room_datas[i]);
		}
		UpdateRoomList();
	}
	break;
	case SC_ROOM_DATA:
	{
		sc_packet_room_data* my_packet = reinterpret_cast<sc_packet_room_data*>(ptr);
		m_roomID = my_packet->room_id;
		roomPeopleList.clear();

		//master와 방 아이디 처리 필요
		//페이지 적용 전
		
		for (auto i = 0; i < my_packet->num_of_people; ++i)
			roomPeopleList.push_back(my_packet->people_id[i]);
		UpdateRoomPeopleList();
	}
	break;
	case SC_ROOM_CANCLE:
	{	
		m_roomID = -1;
//		MessageBox(m_hWnd, "방장이 나갔습니다.", "RoomCancle", MB_OK);
		EnterLobby(true);
	}
	break;
	case SC_ROOM_START:
	{
		EnterGame();
	}
	break;
	case SC_GAME_START:
	{
		CSocketComponent::GetInstance()->m_bGameStarted = true;
	}
	break;
	case SC_PUT_OBJECT:
	{
		sc_packet_put_object* my_packet = reinterpret_cast<sc_packet_put_object*>(ptr);
		m_pScene->Process_SC_Packet_Put_Object(my_packet);
	}
	break;

	case SC_REMOVE_OBJECT:
	{
	}
	break;

	//0803 사용 안하고 세분화함
	/*
	case SC_DATA_OBJECT:
	{
		sc_packet_data_object* my_packet = reinterpret_cast<sc_packet_data_object*>(ptr);
		m_pScene->Process_SC_Packet_Data_Object(my_packet);
	}	
	break;
	*/


	case SC_DATA_OBJECT_STATE: 
	{
		sc_packet_data_object_state* my_packet = reinterpret_cast<sc_packet_data_object_state*>(ptr);
		m_pScene->Process_SC_Packet_Data_Object_State(my_packet);
	}
	break;

	case SC_DATA_OBJECT_POSITION:
	{	
		sc_packet_data_object_position* my_packet = reinterpret_cast<sc_packet_data_object_position*>(ptr);
		m_pScene->Process_SC_Packet_Data_Object_Position(my_packet);
	}
	break;

	case SC_DATA_OBJECT_VALUE:
	{
		sc_packet_data_object_value* my_packet = reinterpret_cast<sc_packet_data_object_value*>(ptr);
		m_pScene->Process_SC_Packet_Data_Object_Value(my_packet);
	}
	break;

	case SC_PUT_BUILDING:
	{
		sc_packet_put_building* my_packet = reinterpret_cast<sc_packet_put_building*>(ptr);
		m_pScene->Process_SC_Packet_Put_Building(my_packet);
	}
	break;

	case SC_ATTACK_OBJECT:
	{
		sc_packet_attack_object* my_packet = reinterpret_cast<sc_packet_attack_object*>(ptr);
		m_pScene->Process_SC_Packet_Attack_Object(my_packet);
	}
	break;

	case SC_ADD_NODE:
	{
		sc_packet_add_node* my_packet = reinterpret_cast<sc_packet_add_node*>(ptr);
		m_pScene->Process_SC_Packet_Add_Node(my_packet);
	}
	break;

	case SC_REMOVE_NODE:
	{
		sc_packet_remove_node* my_packet = reinterpret_cast<sc_packet_remove_node*>(ptr);
		m_pScene->Process_SC_Packet_Remove_Node(my_packet);
	}
	break;

	case SC_DATA_RESOURCE:
	{
		sc_packet_data_resource* my_packet = reinterpret_cast<sc_packet_data_resource*>(ptr);
		m_pScene->Process_SC_Packet_Data_Resource(my_packet);
	}
	break;

	case SC_GET_MINERAL:
	{
		sc_packet_get_mineral* my_packet = reinterpret_cast<sc_packet_get_mineral*>(ptr);
		m_pScene->Process_SC_Packet_Get_Mineral(my_packet);
	}
	break;

	case SC_GET_GAS:
	{
		sc_packet_get_gas* my_packet = reinterpret_cast<sc_packet_get_gas*>(ptr);
		m_pScene->Process_SC_Packet_Get_Gas(my_packet);
	}
	break;
	
	
	case SC_GAME_END:
	{
		sc_packet_game_end* my_packet = reinterpret_cast<sc_packet_game_end*>(ptr);
		bool isWin = my_packet->Iswin;
	}
	break;
	

	default:
		printf("Unknown PACKET type [%d]\n", ptr[1]);
	}
}

void CGameFramework::SendPacket(unsigned char* packet)
{
	DWORD iobyte;
	
	m_SendWsabuf.len = packet[0];
	int ret = WSASend(m_ClientSocket, &m_SendWsabuf, 1, &iobyte, 0, NULL, NULL);
	if (ret)
	{
		int error_code = WSAGetLastError();
		int err_code = WSAGetLastError();
		char buf[50];
		sprintf_s(buf, 50, "WSASend Error [%d]", err_code);
		MessageBox(m_hWnd, buf, "Error!!", MB_OK);
	}
}
