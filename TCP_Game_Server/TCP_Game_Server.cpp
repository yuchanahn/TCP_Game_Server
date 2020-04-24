#include "pch.h"

#include <range/v3/all.hpp>
#include <regex>
#include <fstream>
#include <wchar.h>
#include <functional>
#include <cmath>
#include <variant>
#include <optional>
#include <experimental/coroutine>


#include "YCServer.h"
#include "YC_LOG.h"
#include "YCMempool.h"
#include "YCPacket.h"
#include "YCSync.h"
#include "YCTime.h"
#include "YCRandom.h"
#include "YCCE.h"
#include "YCFunc.h"

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
	int IsSuccess;
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
	vec2_t() : x(0), y(0) {}
	vec2_t(float _x, float _y) : x(_x), y(_y) {}
	vec2_t& operator+=(vec2_t& o)
	{
		x += o.x;
		y += o.y;
		return *this;
	}
	vec2_t& operator+=(vec2_t o)
	{
		x += o.x;
		y += o.y;
		return *this;
	}
	vec2_t operator*(float t)
	{
		return vec2_t { x*t, y*t };
	}
	vec2_t operator-(vec2_t s) {
		return vec2_t(x - s.x, y - s.y);
	}
	vec2_t operator+(vec2_t& v) {
		return vec2_t(x + v.x, y + v.y);
	}
	vec2_t operator+(vec2_t v) {
		return vec2_t(x + v.x, y + v.y);
	}
	vec2_t operator+(float s) {
		return vec2_t(x + s, y + s);
	}
	float dist(vec2_t v) const {
		vec2_t d(v.x - x, v.y - y);
		return d.length();
	}
	float length() const {
		return std::sqrt(x * x + y * y);
	}
	vec2_t& operator*=(float s) {
		x *= s;
		y *= s;
		return *this;
	}
	vec2_t& normalize() {
		if (length() == 0) return *this;
		*this *= (1.0 / length());
		return *this;
	}
	vec2_t vel_for(vec2_t o)
	{
		return (o - *this).normalize();
	}
	vec2_t vel_for(vec2_t& o)
	{
		return (o - *this).normalize();
	}
	static vec2_t get_close(const vec2_t& start_p, const std::vector<vec2_t>& ps)
	{
		//using namespace ::ranges;
		assert(!ps.empty());
		
		const auto r_ = ps	| ::ranges::views::transform([&](const auto& i) { return std::make_pair(start_p.dist(i), std::make_pair(i.x, i.y)); })
							| ::ranges::to_vector
							| ::ranges::actions::sort;
							
		for (auto r: r_ | ::ranges::views::take(1))
		{
			return vec2_t(r.second.first, r.second.second);
		}
	}
};

template <>
struct fmt::formatter<vec2_t> {
	constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

	template <typename FormatContext>
	auto format(const vec2_t& d, FormatContext& ctx) {
		return format_to(ctx.out(), "x: {}, y: {}", d.x, d.y);
	}
};

struct player_t
{
	int user_id;
	int speed;
	vec2_t pos;
	vec2_t vel;
	vec2_t dir;
};
struct champ_ani_t
{
	int user_id;
	int ani_id;
	float ani_nomal_time;
};
struct champ_hp_t
{
	int user_id;
	float hp;
	float max_hp;
};

struct stat_t
{
	yc::CE<float> hp;
	float mp;

	float max_hp;
	float max_mp;

	float op; // 물리 공격력.
	float pd; // 물리 방어력.

	float sp; // 주문력.
	float md; // 마법 방어력.

	float as; // 공속.
	float follow_range;
	float speed;

	//현재 애니메이션 시간.
	float ani_time_nomal;
	float ani_time;
};


struct req_champ_list_t
{
	int user_id;
};


enum class eItem_t : int
{
	Mounting,
	Material
};

struct item_data_t
{
	std::wstring name;
	eItem_t type;
};

struct item_t
{
	int item_code;
	int count;
};

struct item_list_t
{
	item_t items[10];
};

class ITEMDB
{
	static std::vector<item_data_t> ITEMS;

	item_data_t get(int item_code)
	{
#pragma region ItemSetting
		if (ITEMS.empty())
		{
			ITEMS.push_back(item_data_t{ L"COIN", eItem_t::Material });
		}
#pragma endregion

		assert(item_code >= ITEMS.size());
		return ITEMS[item_code];
	}
};

struct champ_type_t
{
	int code;
	int star;
	int lv;
	int item1;
	int item2;
	int item3;
	int item4;
	int item5;
	int item6;
	int value1;

	static champ_type_t get_champ_type_defult(int code, int star)
	{
		return champ_type_t { code, star, 1, -1, -1, -1, -1, -1, -1, -1 };
	}
};
struct champ_list_t
{
	int user_id;
	int count;
	champ_type_t champs[10];
};

enum class eGacha_type
{
	_10 = 0,
	_1 = 1
};

struct champ_gacha_t
{
	int gacha_type;
};
struct champ_gacha_r_t
{
	int new_champ_code;
};

class champ_pick_data_t
{
	int list[2];
};

class cmd_game_start_t
{
	int flag;
};

#pragma pack(pop)

#pragma region Define - Stats - Lable
#define _HP_
#define _MP_
#define _MAX_HP_
#define _MAX_MP_
#define _OP_
#define _PD_
#define _SP_
#define _MD_
#define _SPEED_
#define _ANI_T_
#define _ANINM_T_
#define _AS_
#define _FOLLOW_RANGE_
#pragma endregion

class YCObject
{
	bool mActive = true;
public:
	bool active_self()
	{
		return mActive;
	}
	void SetActive(bool active)
	{
		mActive = active;
	}
	
	virtual void fixed_update(float fixedDeltatime)
	{

	}

	static std::unordered_map<int, YCObject*> objs;
};
std::unordered_map<int, YCObject*> YCObject::objs;

YCServer* svr = nullptr;
const float fixed_deltatime = 0.1f;

enum class eAttackType
{
	NTRange,
	TRange,
	Targeting,
	NonTargeting
};
enum class eChampion
{
	Warrior,
	End
};
struct attack_data_t
{
	float attack_col_start_time;
	float attack_col_end_time;

	float attack_start_dist;
	float attack_range;
};
struct champ_t
{
	eAttackType atk_type;

	attack_data_t atk_data;

	stat_t defult_stats;
	stat_t growth_stats;

	static champ_t get_champ_defult(eChampion code)
	{
		static std::unordered_map<eChampion, champ_t> camps;

		if (camps.empty())
		{
			camps[eChampion::Warrior] = champ_t {
				eAttackType::NTRange,
				attack_data_t { 0.1f, 0.2f, 0.1f, 1.f },
				stat_t
				{
					_HP_			100,
					_MP_			0,
					_MAX_HP_		100,
					_MAX_MP_		0,
					_OP_			10,
					_PD_			10,
					_SP_			10,
					_MD_			10,
					_AS_			2,
					_FOLLOW_RANGE_	5,
					_SPEED_			2,
					_ANINM_T_		0.f,
					_ANI_T_			0.9f
				},
				stat_t
				{
					_HP_			0,
					_MP_			0,
					_MAX_HP_		1,
					_MAX_MP_		0,
					_OP_			0.1,
					_PD_			0.1,
					_SP_			0,
					_MD_			0.1,
					_AS_			-0.05f,
					_FOLLOW_RANGE_	0,
					_SPEED_			0,
					_ANINM_T_		0.f,
					_ANI_T_			0.f
				}
			};
		}

		return camps[code];
	}
	static champ_t* new_champ(eChampion champ_code)
	{
		return new champ_t(get_champ_defult(champ_code));
	}
};
struct time_val_t
{
	float t = 0;
	float rate;
	bool once_f;

	void update(const float _t) { t += _t; }
	void reset() { t = 0; }

	time_val_t(float r_) :
		t(0), rate(r_), once_f(true)
	{}

	template <typename F>
	void timer_end(F f, float ut)
	{
		update(ut);
		if (t >= rate)
		{
			reset();
			f();
		}
	}
	template <typename F>
	void timer_start(F f)
	{
		if (once_f)
		{
			f();
			once_f = false;
		}
	}
};

struct attack_st
{
	float ani_nomal_time;
	vec2_t vel;
	vec2_t dir2d;
	template <typename F> 
	void on_attack_end(F OnAttackEnd)
	{
		if (ani_nomal_time >= 1.f)
		{
			OnAttackEnd();
		}
	}
	template <typename F>
	void on_hitable_time(F on_hit_event, const float& hit_col_stratT, const float& hit_col_endT)
	{
		if (ani_nomal_time >= hit_col_stratT && ani_nomal_time <= hit_col_endT)
		{
			on_hit_event();
		}
	}
	void update_time(float t)
	{
		ani_nomal_time += t;
	}
};
struct random_idle_st
{
	time_val_t idle;
	vec2_t last_dir2d;
};
struct random_move_st
{
	time_val_t move;
	vec2_t dir2d;
};
struct follow_st
{
	vec2_t target_pos;
};

class Champion : public YCObject
{
	int id;
	int user_id;
public:
	PROP_G(Strand*, Sync, { return svr->get_sync(id); });
	PROP_G(float, AttackPower, { return stat.op * yc::rand(1.f, 5.f); });
	vec2_t pos;
protected:
	champ_t* champ;
	stat_t stat;

	std::vector<Champion*> room;
	std::vector<Champion*> hited;
public:
	Champion(int i, int ui, eChampion champ_code) :
		id(i), user_id(ui)
	{
		objs[user_id] = this;
		champ = champ_t::new_champ(champ_code);
		stat = champ->defult_stats;
		stat.hp.on_change([this] { 
			send_hp_data();
			if (!IsAlived())
			{
				send_death();
				svr->get_server_sync()->Add([this] { SetActive(false); });
			}
		});
		champ_state = random_move_st{ time_val_t(yc::rand(1.f, 2.f)), yc::random_dir2d<vec2_t>() };
		pos = vec2_t(yc::rand(0.f, 8.f), yc::rand(0.f, 8.f));
	}
	~Champion()
	{
		svr->get_server_sync()->Add([uid = user_id] { objs.erase(uid); });
		delete champ;
	}
private:
	std::variant<
		attack_st,
		random_move_st,
		random_idle_st,
		follow_st> 
		champ_state;
	bool is_attackable()
	{
		for (auto i : room) {
			if (i == this) continue;
			if (i->pos.dist(pos) <= champ->atk_data.attack_range)
				return true;
		}
		return false;
	}
	bool is_followable()
	{
		for (auto i : room) {
			if (i == this) continue;
			if (pos.dist(i->pos) <= stat.follow_range)
				return true;
		}
		return false;
	}
	
public:
	bool IsAlived() const
	{
		return stat.hp.val > 0;
	}
	void SetRoom(std::vector<Champion*> r)
	{
		room = r;
	}
	void hit(float dmg)
	{
		Sync->Add([this, dmg] { stat.hp = stat.hp - dmg; });
	}
	void set_speed(float speed)
	{
		Sync->Add([this, speed] {
			stat.speed = speed;
		});
	}
	void send_hp_data()
	{
		float hp = stat.hp;
		float max_hp = stat.max_hp;
		Sync->Add([this, ui = user_id, hp, max_hp] {
			champ_hp_t t;
			t.user_id = ui;
			t.hp = hp;
			t.max_hp = max_hp;

			for(auto& i : room) i->send(&t);
		});
	}
	void send_ani(int ani_code, float nomalT)
	{
		champ_ani_t t
		{
			user_id,
			ani_code,
			nomalT
		};
		
		for (auto& i : room) i->send(&t);
	}
	void send_pos(float speed, vec2_t pos, vec2_t vel, vec2_t dir)
	{
		player_t p
		{ 
			user_id,
			speed,
			pos,
			vel,
			dir
		};

		for (auto& i : room) i->send(&p);
	}
	void send_win()
	{
		Sync->Add([uid = user_id] { fmt::print("{} win!\n", uid); });
	}
	void send_death()
	{
		Sync->Add([uid = user_id] { fmt::print("{} death!\n", uid); });
	}
	void DestroySelf()
	{
		svr->get_server_sync()->Add([this]() { 
			SetActive(false);
			Sync->AddLate([gabage = this]() { delete gabage; });
		});
	}

	template <typename T>
	static void send(int user_id, T* p)
	{
		svr->Send(room[user_id]->id, p);
	}
	template <typename T>
	void send(T* p)
	{
		svr->Send(id, p);
	}
	
	virtual void fixed_update(float fixedDeltatime)
	{
		Sync->Add([this, fixed_dt = fixedDeltatime] {

			auto near_champ_pos = [t = this]() {
				using namespace ::ranges;
				return 	vec2_t::get_close(
					t->pos,
					t->room | views::filter([t](const auto& i) { return i->active_self(); })
							| views::filter([t](const auto& i) { return t != i; })
							| views::transform([](const auto& c) { return c->pos; })
							| to_vector);
			};
			auto send_vel = [this](auto d, auto d2) { send_pos(stat.speed, pos, d, d2); };
			stat.follow_range += fixed_dt * 0.7f;
			std::visit(
				overloaded {
					[&](attack_st& s) {
						send_ani(0, s.ani_nomal_time);
						s.update_time(fixed_dt);
						s.on_hitable_time([&] {
							using namespace ::ranges;
							auto in_range = [&, 
								&atk_d = champ->atk_data,
								ori = pos + (s.vel * champ->atk_data.attack_start_dist)](const auto& i) {
								if (std::find(hited.begin(), hited.end(), i) != hited.end()) return false;
								else if (i == this)			return false;
								else if (!i->active_self())	return false;

								return ori.dist(i->pos) <= atk_d.attack_range;
							};

							for (Champion*& i : room | views::filter(in_range))
							{
								i->hit(AttackPower);
								hited.push_back(i);
							}
						},champ->atk_data.attack_col_start_time, champ->atk_data.attack_col_end_time);
						s.on_attack_end([&] { hited.clear(); champ_state = random_idle_st{ time_val_t(stat.as), s.dir2d }; });
					},
					[&](random_move_st& rm) {
						if (is_followable())
						{
							champ_state = follow_st{ near_champ_pos() };
						}
						else
						{
							pos += rm.dir2d * stat.speed * fixed_dt;
							send_pos(stat.speed, pos, rm.dir2d, rm.dir2d);
							rm.move.timer_end([&] {
								champ_state = random_idle_st { time_val_t(yc::rand(1.f, 2.f)) };
							}, fixed_dt);
						}
					},
					[&](random_idle_st& ri)
					{
						ri.idle.timer_start([&] {
							send_pos(0, pos, vec2_t{}, ri.last_dir2d);
						});
						ri.idle.timer_end([&] {
							auto r = yc::random_dir2d<vec2_t>();
							champ_state = random_move_st{ time_val_t(yc::rand(1.f, 2.f)), r };
						}, fixed_dt);
					},
					[&](const follow_st& ft){
						auto vel_of_near_champ = pos.vel_for(near_champ_pos());
						if (is_attackable())
						{
							send_vel(vec2_t{}, vel_of_near_champ);
							champ_state = attack_st { 0, vel_of_near_champ, vel_of_near_champ };
						}
						else
						{
							pos += vel_of_near_champ * stat.speed * fixed_dt;
							send_vel(vel_of_near_champ, vel_of_near_champ);
						}
					}
				},
			champ_state);
		});
	}
};

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
	Champion* a;
	champ_list_t champs;
};

//TODO :
/*
	 - 게임 상태.
	 - 플레이어 목록을 저장.

	 게임 시작시 모든 플레이어들이 챔피언을 선택하고 게임이 시작됨.
	 일단 게임이 시작되면, 랜덤위치에 모든 챔피언들이 생성된다,
	 그리고 각자의 스텟에 따라 싸움을 진행하고, 시간이 지날 수록 탐색 범위가 넓어져서, 모든 챔피언들이 다 싸우게 된다.
*/


struct battler_data_t
{
	int id;
	int user_id;
	eChampion champ_code;
};

struct matching_st
{
	int max_battler_count;
	std::vector<battler_data_t> battlers;
};

struct battle_st
{
	std::vector<Champion*> champs;
};

struct game_end_st
{
	Champion* alive_champ;
};


struct delete_battle_st
{

};

struct battle_t
{
	std::variant<
		matching_st,
		battle_st,
		game_end_st,
		delete_battle_st> 
		state;

	template <typename F1, typename F2>
	void run(F1 get_user_queue, F2 win_event)
	{
		std::visit(
			overloaded {
			[&](matching_st& m) {
				using namespace ::ranges;
				::ranges::for_each(get_user_queue(), [&](auto& i) { 
					m.battlers.push_back(i); 
				});
				if (m.battlers.size() == m.max_battler_count) {
					auto room = m.battlers | views::transform([&](const auto& i) { return new Champion(i.id, i.user_id, i.champ_code); })
										   | to_vector;
					::ranges::for_each(room, [&](auto& i) { i->SetRoom(room); });
					state = battle_st{ room };
				}
			},
			[&](battle_st& st) {
				using namespace ::ranges;
				auto is_alived = [](const auto& i) { return i->IsAlived(); };
				auto is_not_alived = [](const auto& i) { return !i->IsAlived(); };

				auto alive_champs = st.champs | views::filter(is_alived)
											  | to_vector;
				if (alive_champs.size() <= 1)
				{
					for (Champion*& i : st.champs | views::filter(is_not_alived))
					{
						i->DestroySelf();
					}
					if (alive_champs.size())
					{
						alive_champs[0]->SetActive(false);
					}
					state = game_end_st { alive_champs.size() ? alive_champs[0] : nullptr };
				}
			},
			[&](game_end_st& end) {
				if (end.alive_champ) {
					win_event(end.alive_champ);
					end.alive_champ->DestroySelf();
				}
				state = delete_battle_st{ };
			},			
			[&](auto&) { assert("이 상태가 실해되면 안댐!"); }
		},state);
	}
};

#define DB_PATH "C:/YCDB"

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

int main() {
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
	ioev::Map<champ_hp_t>().To<11>();
	ioev::Map<champ_ani_t>().To<12>();
	ioev::Map<champ_type_t>().To<13>();
	ioev::Map<champ_list_t>().To<14>();
	ioev::Map<req_champ_list_t>().To<15>();
	ioev::Map<champ_gacha_t>().To<16>();
	ioev::Map<champ_gacha_r_t>().To<17>();
	ioev::Map<champ_pick_data_t>().To<18>();
	ioev::Map<cmd_game_start_t>().To<19>();
#pragma endregion

#pragma region Curried Functions
	auto sign_in = make_curried(YC_DBv1::register_id)(false);
	auto sign_up = make_curried(YC_DBv1::register_id)(true);
	auto get_db_nickname = YC_DBv1::get<db_nickname>;
	auto set_nickname = make_curried(YC_DBv1::set<db_nickname>);
	auto set_champlist = make_curried(YC_DBv1::set<champ_list_t>);
	auto get_db_champlist = YC_DBv1::get<champ_list_t>;
	auto get_db_nickname_validate = YC_DBv1::once_get<db_nickname_validate>;
	auto set_db_nickname_validate = make_curried(YC_DBv1::once_set<db_nickname_validate>);
#pragma endregion

#pragma region Flags
	std::once_flag db_nickname_validate_f;
#pragma endregion

#pragma region Sesstions HashMap
	std::unordered_map<int, sesstion_t> clients;
	std::unordered_map<int, login_sesstion_t> login_clients;
	std::unordered_map<int, int> get_user_id;
#pragma endregion

#pragma region Server Init

	std::vector<Champion*> test_champs;
	std::vector<battle_t> battles;
	std::vector<battler_data_t> user_q;

	YCServer server(51234,
		[&](int id) {
			clients[id] = sesstion_t{ id, false };
			yc::log("connect client! [{}]", id);
		},
		[&](int id) {
			if (clients[id].IsLogin)
			{
				delete login_clients[get_user_id[id]].a;
				login_clients.erase(get_user_id[id]);
				get_user_id.erase(id);
			}
			clients.erase(id);
			yc::log("disconnect client! [{}]", id);
		},
		[&] {
			static float dt = 0;
			static size_t FPS = 0;
			
			static float fixed_deltatimeT = 0;

			dt += YCTime::deltaTime;
			FPS++;

			fixed_deltatimeT += YCTime::deltaTime;
			if (fixed_deltatimeT >= fixed_deltatime) {
				for (auto& i : YCObject::objs) {
					if (i.second->active_self()) {
						i.second->fixed_update(fixed_deltatimeT);
					}
				}

				bool already_matching = false;
				std::vector<battle_t*> erase_battle_buffer;
				for (auto& i : battles)
				{
					i.run(
						[&, v = user_q] { 
							user_q.clear(); 
							return v; 
						},
						[&](Champion* winner) {
							winner->send_win();
						});
					std::visit(overloaded{
						[&](const matching_st&)		 { already_matching = true; },
						[&](const delete_battle_st&) { erase_battle_buffer.push_back(&i); },
						[](auto&) {}
					}, i.state);
				}
				for (auto& i : erase_battle_buffer)
				{
					battles.erase(std::remove_if(battles.begin(), battles.end(), [t = i](auto& i) {
						return &i == t;
					}), battles.end());
				}
				if (!already_matching)
				{
					battles.push_back(battle_t{ matching_st{ 2, {} } });
				}

				fixed_deltatimeT -= fixed_deltatime;
			}

			if (dt >= 1)
			{
				dt -= 1;
				yc::log("fps : {}", FPS);
				FPS = 0;
			}
		});
	svr = &server;
#pragma endregion

#pragma region Bind Functions
	auto send_nickname_to = [&](int id) {
		server.get_server_sync()->AddLate([&, id] {
			auto& clnt = login_clients[get_user_id[id]];
			auto get_n = get_name_r_t{ clnt.user_id };
			yc_str::copy(clnt.name.c_str(), get_n.name);
			fmt::print(L"name : {}\n", get_n.name);
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
	auto set_db_champlist = [&](const int user_id, champ_list_t list) {
		set_champlist(user_id, list);
	};
	auto r_login = [&](auto&& f) { return [&](auto* p, int id) {
		server.get_server_sync()->Add([&, login_data = *p, id]() {
			auto user_id = f(login_data.id_str, login_data.pass_str);
			auto r = login_r_t{ user_id };

			if (user_id != -1)
			{
				if (!std::get<0>(get_db_nickname(user_id)))
				{
					set_db_nickname(L"None", user_id);
					set_db_champlist(user_id, champ_list_t{ user_id, 1, { champ_type_t::get_champ_type_defult(0,1), } });
				}
				clients[id].IsLogin = true;
				login_clients[user_id] = login_sesstion_t {
					id, 
					user_id, 
					std::get<1>(get_db_nickname(user_id)).name,
					nullptr/*new Champion(id, user_id, eChampion::Warrior)*/,
					std::get<1>(get_db_champlist(user_id))
				};
				get_user_id[id] = user_id;
				send_nickname_to(id);

				static bool once_f = true;

				if (once_f)
				{
					//test_champs.push_back(new Champion(0, 1001, eChampion::Warrior));
					//test_champs.push_back(new Champion(0, 1002, eChampion::Warrior));
					//test_champs.push_back(new Champion(0, 1003, eChampion::Warrior));
					//test_champs.push_back(new Champion(0, 1004, eChampion::Warrior));

					once_f = false;
				}
			}
			server.Send(id, &r);
		});
	}; };
	auto gacha_start = [&](int user_id) {
		auto c = &login_clients[user_id];

		server.get_sync(c->id)->Add([set_db_champlist, c, user_id, srv = &server]() {
			auto new_champ = champ_type_t::get_champ_type_defult(yc::rand(0, (int)eChampion::End - 1), 1);
			c->champs.champs[c->champs.count++] = new_champ;
			set_db_champlist(user_id, c->champs);
			champ_gacha_r_t r;
			r.new_champ_code = new_champ.code;
			srv->Send(c->id, &r);
		});
	};
#pragma endregion

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
	ioev::Signal<sign_in_t>(r_login(sign_in));
	ioev::Signal<sign_up_t>(r_login(sign_up));
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
	ioev::Signal<player_t>([&](player_t* d, int id) {
		login_clients[get_user_id[id]].a->set_speed(d->speed);
	});
	ioev::Signal<req_champ_list_t>([&](req_champ_list_t* d, int id){
		server.Send(id, &login_clients[d->user_id].champs);
	});
	ioev::Signal<champ_gacha_t>([&](champ_gacha_t* d, int id) {
		for (int i = 0; i < (d->gacha_type == (int)eGacha_type::_1 ? 1 : 1); i++)
			gacha_start(get_user_id[id]);
	});
	ioev::Signal<cmd_game_start_t>([&](cmd_game_start_t* d, int id) {
		server.get_server_sync()->Add([&, d, id] {
			auto data = battler_data_t{ id, get_user_id[id], eChampion::Warrior };
			user_q.push_back(data);
		});
	});

	server.Srv_Start();
	return 0;
}