#pragma once
#include "YCPacket.h"
#include "YCServer.h"
#include <winsock2.h>

class YCSend
{
	SOCKET socket;
	char buf[1024];
public:
	YCSend(SOCKET s) : socket(s) {}

	template <typename T>
	void send(T& t)
	{
		auto len = ((packet_t<T>*) & t)->pack((unsigned char*)buf);
		::send(socket, buf, len, 0);
	}
};