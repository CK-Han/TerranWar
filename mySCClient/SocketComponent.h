#pragma once

class CSocketComponent
{
private:
	static CSocketComponent		*instance;
	
	CSocketComponent();
	~CSocketComponent();

public:
	static CSocketComponent*	GetInstance()
	{
		if (nullptr == instance)	instance = new CSocketComponent();
		return instance;
	}

	void SendPacket(unsigned char* packet)
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
			MessageBox(0, buf, "Error!!", MB_OK);
		}
	}

	void DeleteInstance() { delete CSocketComponent::GetInstance(); }

public:
	//소켓 통신
	SOCKET			m_ClientSocket;
	WSABUF			m_SendWsabuf;
	char			m_SendBuf[BUF_SIZE];
	WSABUF			m_RecvWsabuf;
	char			m_RecvBuf[BUF_SIZE];
	char			m_PacketBuf[BUF_SIZE];

	DWORD			m_InPacketSize{0};
	int				m_SavedPacketSize{0};

	int				m_clientID{-1};
	int				m_roomID{-1};

	bool			m_bGameStarted{ false };
};

