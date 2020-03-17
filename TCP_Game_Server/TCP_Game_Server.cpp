#include "pch.h"

#include <regex>
#include <fstream>
#include <wchar.h>
#include <functional>

#include "YCServer.h"
#include "YC_LOG.h"
#include "YCMempool.h"
#include "YCServer.h"
#include "YCPacket.h"
#include "YCSync.h"
#include "YCTime.h"

#pragma pack(push, 1)
struct test_t
{
	wchar_t c[100];
};
struct ping_t
{
	long long ping;
};
struct sign_in_t
{
	wchar_t id_str[20];
	wchar_t pass_str[20];
};
struct sign_up_t
{
	wchar_t id_str[20];
	wchar_t pass_str[20];
};
struct login_r_t
{
	int r;
};
struct get_name_t
{
	int user_id;
};
struct get_name_r_t
{
	int user_id;
	wchar_t name[50];
};
struct set_name_t
{
	wchar_t name[50];
};
struct set_name_r_t
{
	bool IsSuccess;
};
struct db_nickname
{
	wchar_t name[50];
};
struct db_nickname_validate
{
	char validate[20];
};


struct vec2_t
{
	float x;
	float y;
};
struct player_t
{
	int user_id;
	int speed;
	vec2_t pos;
};

#pragma pack(pop)




struct sesstion_t
{
	int id;
	bool IsLogin;
};
struct login_sesstion_t
{
	int id;
	int user_id;
	std::wstring name;
	player_t a;
};


#define DB_PATH "C:/YCDB" /*"../YCDB"*/


class YC_DBv1
{
public:
	static int register_id(bool bnew_acc, std::wstring id_str, std::wstring pass_str)
	{
		static std::wstring validate_id = L"([\\w*\\d*]{6,20})";
		static std::wstring validate_pass = L"((?=.*[A-Za-z])(?=.*\\d)(?=.*[$@$!%*#?&])[A-Za-z\\d$@$!%*#?&]{8,20})";
		static std::once_flag db_register_id_validate_f;
		static int idx = 0;
		static std::unordered_map<std::wstring, std::tuple<std::wstring, int>> users;
		std::call_once(db_register_id_validate_f, [&]() {
			std::ifstream file(fmt::format("{}/{}", DB_PATH, "register_data.txt"));
			std::ifstream validate(fmt::format("{}/{}", DB_PATH, "register_validate_data.txt"));
			if (!validate.is_open())
			{
				validate.close();
				std::ofstream o(fmt::format("{}/{}", DB_PATH, "register_validate_data.txt"));
				std::string s[2];
				
				s[0].assign(validate_id.cbegin(), validate_id.cend());
				s[0] += "\n";
				s[1].assign(validate_pass.cbegin(), validate_pass.cend());
				s[1] += "\n";

				o.write(s[0].c_str(), s[0].length());
				o.write(s[1].c_str(), s[1].length());
				o.close();
				validate.open(fmt::format("{}/{}", DB_PATH, "register_validate_data.txt"));
			}
			std::string s_row[2];
			int i = 0;
			std::getline(validate, s_row[0]);
			std::getline(validate, s_row[1]);

			validate_id.assign(s_row[0].cbegin(), s_row[0].cend());
			validate_pass.assign(s_row[1].cbegin(), s_row[1].cend());

			if (file.is_open())
			{
				std::wregex re(fmt::format(L"(\\d+),{},{}", validate_id, validate_pass));
				std::wstring s;
				std::wsmatch m;
				std::string s_row;
				while (!file.eof()) {
					std::getline(file, s_row);

					s.assign(s_row.begin(), s_row.end());

					if (std::regex_match(s, m, re))
					{
						int id = _wtoi(m[1].str().c_str());
						users[m[2].str()] = std::make_tuple(m[3].str(), id);
						idx = id;
					}
				}
			}
			else
			{
				std::ofstream o(fmt::format("{}/{}", DB_PATH, "register_data.txt"));
				o.close();
			}
			file.close();
			validate.close();
		});

		if (users.find(id_str) == users.end())
		{
			if (!bnew_acc) return -1;
			std::wregex re_id(validate_id);
			std::wregex re_pass(validate_pass);
			if (std::regex_match(id_str, re_id) && std::regex_match(pass_str, re_pass))
			{
				std::ofstream o(fmt::format("{}/{}", DB_PATH, "register_data.txt"), std::ios::app);
				auto ws = fmt::format(L"{},{},{}\n", ++idx, id_str, pass_str);
				std::string s;
				s.assign(ws.cbegin(), ws.cend());
				o.write(s.c_str(), s.length());
				o.close();

				users[id_str] = std::make_tuple(pass_str, idx);
				return idx;
			}
			else
			{
				return -1;
			}
		}
		else
		{
			if (bnew_acc) return -1;
			return 
				std::get<0>(users[id_str]) == pass_str ? 
				std::get<1>(users[id_str]) : -1;
		}
	}

	template <typename T>
	static void once_set(const T& p)
	{
		std::ofstream file(fmt::format("{}/{}.txt", DB_PATH, typeid(T).name()));
		auto byte = ((packet_t<T>*) (&p))->ToByte();
		file.write((const char*)(byte), sizeof(T));
		file.close();
	}

	template <typename T>
	static auto once_get()
	{
		std::ifstream file(fmt::format("{}/{}.txt", DB_PATH, typeid(T).name()));
		if (!file.is_open()) return std::tuple(false, T());
		char byte[sizeof(T)];
		file.read(byte, sizeof(T));
		file.close();
		return std::tuple(true, *((T*)((void*)byte)));
	}

	template <typename T>
	static void set(int user_id, const T& p)
	{
		std::ofstream file(fmt::format("{}/{}@{}.txt", DB_PATH, user_id, typeid(T).name()));
		auto byte = ((packet_t<T> *) (&p))->ToByte();
		file.write((const char*)(byte), sizeof(T));
		file.close();
	}
	template <typename T>
	static auto get(int user_id)
	{
		std::ifstream file(fmt::format("{}/{}@{}.txt", DB_PATH, user_id, typeid(T).name()));
		if (!file.is_open()) return std::tuple(false, T());
		char byte[sizeof(T)];
		file.read(byte, sizeof(T));
		file.close();
		return std::tuple(true,*((T*)((void*)byte)));
	}
};
class yc_str
{
public:
	static void copy(std::string s, char* buf)
	{
		s.copy(buf, s.length());
		buf[s.length()] = '\0';
	}
	static void copy(std::wstring s, wchar_t* buf)
	{
		s.copy(buf, s.length());
		buf[s.length()] = '\0';
	}
};

int main()
{
#pragma region PACKET_SET
	ioev::Map<test_t>().To<0>();
	ioev::Map<ping_t>().To<1>();
	ioev::Map<sign_in_t>().To<2>();
	ioev::Map<sign_up_t>().To<3>();
	ioev::Map<login_r_t>().To<4>();
	ioev::Map<set_name_t>().To<5>();
	ioev::Map<set_name_r_t>().To<6>();
	ioev::Map<get_name_t>().To<7>();
	ioev::Map<get_name_r_t>().To<8>();
	ioev::Map<vec2_t>().To<9>();
	ioev::Map<player_t>().To<10>();
#pragma endregion
	// this hash_map, Have to used in Server_Sync!!!
	std::unordered_map<int, sesstion_t> clients;
	std::unordered_map<int, login_sesstion_t> login_clients;
	std::unordered_map<int, int> get_user_id;

	auto sign_in = make_curried(YC_DBv1::register_id)(false);
	auto sign_up = make_curried(YC_DBv1::register_id)(true);
	auto get_db_nickname = YC_DBv1::get<db_nickname>;
	auto set_nickname = make_curried(YC_DBv1::set<db_nickname>);
	auto get_db_nickname_validate = YC_DBv1::once_get<db_nickname_validate>;
	auto set_db_nickname_validate = make_curried(YC_DBv1::once_set<db_nickname_validate>);

	srand(time(0));
	YCServer* svr = nullptr;
	YCServer server(51234,
		[&](int id) {
			clients[id] = sesstion_t{ id, false };
			yc::log("connect client! [{}]", id);
		},
		[&](int id) {
			if (clients[id].IsLogin)
			{
				login_clients.erase(get_user_id[id]);
				get_user_id.erase(id);
			}
			clients.erase(id);
			yc::log("disconnect client! [{}]", id);
		},
			[&]
		{
			static float dt = 0;
			static size_t FPS = 0;

			static float player_movement_rate = 0.1f;
			static float player_movement_rateT = 0;
			

			dt += YCTime::deltaTime;
			player_movement_rateT += YCTime::deltaTime;
			FPS++;

			if (player_movement_rateT > player_movement_rate)
			{
				player_movement_rateT = 0;

				for (auto& i : login_clients)
				{
					int dir[] = { -1, 1 };

					i.second.a.pos.x += dir[rand() % 2] * i.second.a.speed * player_movement_rate;
					i.second.a.pos.y += dir[rand() % 2] * i.second.a.speed * player_movement_rate;

					player_t p = i.second.a;
					svr->Send(i.second.id, &p);
				}
			}

			if (dt > 1)
			{
				dt = 0;
				yc::log("fps : {}", FPS);
				FPS = 0;
			}
		});
	svr = &server;

	auto send_nickname_to = [&](int id) {
		server.get_server_sync()->Add([&, id] {
			auto& clnt = login_clients[get_user_id[id]];
			auto get_n = get_name_r_t{ clnt.user_id };
			yc_str::copy(clnt.name, get_n.name);
			server.Send(id, &get_n);
		});
	};
	auto set_db_nickname = [&](std::wstring s, int id) {
		server.get_server_sync()->Add([&, s, id]() {
			auto data = db_nickname{ };
			yc_str::copy(s, data.name);
			set_nickname(id, data);
			});
	};

	ioev::Signal<test_t>([&](test_t* d, int id) {
		
		if (!clients[id].IsLogin) return;

		server.get_server_sync()->Add([&, t = *d, id]{
			auto str = fmt::format(L"{} : {}", login_clients[get_user_id[id]].name, t.c);

			yc_str::copy(str, (wchar_t*)t.c);

			for (auto& i : login_clients)
			{
				server.Send(i.second.id, &t);
			}
		});
	});
	ioev::Signal<ping_t>([&server](ping_t* p, int id) {
		server.Send(id, p);
	});

	auto r_login = [&](auto&& f) { return [&](auto* p, int id) {
		server.get_server_sync()->Add([&, login_data = *p, id]() {
			auto user_id = f(login_data.id_str, login_data.pass_str);
			auto r = login_r_t{ user_id };

			if (user_id != -1)
			{
				if (!std::get<0>(get_db_nickname(user_id)))
				{
					set_db_nickname(L"None", user_id);
				}
				auto nname = get_db_nickname(user_id);
				clients[id].IsLogin = true;
				login_clients[user_id] = login_sesstion_t{ id, user_id, std::get<1>(nname).name, player_t {user_id, 1, vec2_t{0,0} } };
				get_user_id[id] = user_id;
				send_nickname_to(id);
			}
			server.Send(id, &r);
		});
	}; };

	ioev::Signal<sign_in_t>(r_login(sign_in));
	ioev::Signal<sign_up_t>(r_login(sign_up));

	std::once_flag db_nickname_validate_f;
	ioev::Signal<set_name_t>([&](set_name_t* d, int id) {
		static std::wregex re(L"^.{2,8}$");
		std::call_once(db_nickname_validate_f, [&]() {
			auto validate_row = get_db_nickname_validate();
			if (!std::get<0>(validate_row))
			{
				set_db_nickname_validate(db_nickname_validate{ "^.{2,8}$" });
				validate_row = get_db_nickname_validate();
			}
			std::wstring get_nickname_validate = L"";
			std::string s = std::get<1>(validate_row).validate;
			get_nickname_validate.assign(s.cbegin(), s.cend());
			re.assign(get_nickname_validate);
		});

		auto r = set_name_r_t{ std::regex_match(d->name, re) };

		if (r.IsSuccess) { 
			set_db_nickname(d->name, get_user_id[id]);
			server.get_sync(id)->Add([&, n = d->name, id]() {
				login_clients[get_user_id[id]].name = n;
			});
		}
		server.Send(id, &r);
	});
	ioev::Signal<get_name_t>([&](get_name_t* d, int id) {
		if(clients[id].IsLogin) send_nickname_to(id);
	});



	server.Srv_Start();
	return 0;
}