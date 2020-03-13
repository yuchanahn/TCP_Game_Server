#include "pch.h"

#include "YCServer.h"
#include "YC_LOG.h"
#include "YCMempool.h"
#include "YCServer.h"
#include "YCPacket.h"
#include "YCSync.h"
#include "YCTime.h"

#include <regex>

#pragma pack(push, 1)
struct test_t
{
	wchar_t c[100];
};
#pragma pack(pop)

struct sesstion_t
{
	int id;
	std::wstring name;
};

int main()
{
#pragma region PACKET_SET
	ioev::Map<test_t>().To<0>();
#pragma endregion
	// this hash_map, Have to used in Server_Sync!!!
	std::unordered_map<int, sesstion_t> clients;

	YCServer server(51234,
		[&](int id) {
			clients[id] = sesstion_t{ id, fmt::format(L"client{}", id) };
			yc::log("connect client! [{}]", id);
		},
		[&](int id) {
			clients.erase(id);
			yc::log("disconnect client! [{}]", id);
		},
			[]
		{
			static float dt = 0;
			static size_t FPS = 0;

			dt += YCTime::deltaTime;
			FPS++;
			if (dt > 1)
			{
				dt = 0;
				yc::log("fps : {}", FPS);
				FPS = 0;
			}
		});

	ioev::Signal<test_t>([&server, &clients](test_t* d, int id) {
		std::wregex re(L"-n (.*)");
		std::wsmatch m;
		std::wstring s = d->c;
		if (std::regex_match(s, m, re))
		{
			if (m.size() > 0) clients[id].name.assign(m[1]);
			return;
		}
		server.get_server_sync()->Add([&server, &clients, t = *d, ID = id]{
			auto str = fmt::format(L"{} : {}", clients[ID].name, t.c);

			wchar_t* c = (wchar_t*)t.c;
			str.copy(c, str.size());
			c[str.size()] = '\0';

			for (auto& i : clients)
			{
				server.Send(i.first, &t);
			}
			});
	});
	server.Srv_Start();
	return 0;
}