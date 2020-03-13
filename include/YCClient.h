#pragma once
class YC_Packet_ReadManager;
class YCClient
{
	SOCKET socket;
	SOCKADDR_IN serveraddr;
	WSADATA wsa;
	char buf[1024];
	char* mIp_address = nullptr;
	YC_Packet_ReadManager* r;
public:
	
	const SOCKET& get_socket()
	{
		return socket;
	}
	bool connect(const char* ip_address, u_short port);
	int read_packet();
	YCClient();
	~YCClient();
};