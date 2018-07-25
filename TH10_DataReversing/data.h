#pragma once
#include <windows.h>
#include <vector>

#define GAME_WIDTH 384
#define GAME_HEIGHT 448
#define GAME_X_OFFSET 192
#define GAME_Y_OFFSET 0

struct vec2
{
	float x, y;
	vec2();
	vec2(float x, float y);
	vec2(const vec2& other);
	vec2(vec2&& other) noexcept;
	vec2& operator=(const vec2& other);
	vec2& operator=(vec2&& other) noexcept;
	vec2 rotate(float arc) const;
};
vec2 operator+(const vec2& a, const vec2& b);
vec2 operator*(const vec2& a, float o);

struct Object
{
	vec2 p;
	vec2 v;
	float w, h;
	Object(){}
	Object(vec2 p, vec2 v, float w, float h);
};

struct Player : Object
{
	Player(){}
	float powers;

	int life;

	// type == 0 Reimu
	// type == 1 Marisa
	int type;

	// slow == 0 High Speed
	// slow == 1 Low Speed
	int slow;

	float itemObtainRange;

	// status == 0 重生状态，此时无敌
	// status == 1 正常状态
	// status == 2 死亡
	// status == 3 Unknown
	// status == 4 被弹、决死，此时放B可以决死
	int status;

	// 剩余无敌时间
	int invinibleTime;
};

struct Laser : Object
{
	float arc;
	Laser(vec2 p, vec2 v, float w, float h, float arc);
	std::vector<vec2> getVertices() const;
};

struct Item : Object
{
	// 正常点分为以下几种
	// type == 1 Power Items P点（红点）
	// type == 2 Point Items 得分点（蓝点）
	// type == 3 Faith Items 信仰点（绿点）
	// type == 4 Large Power Items 大P点（红点）
	// type == 5 Large Point Items 大得分点（带黄色边线的蓝点），由BOSS掉落
	// type == 6 Unknown
	// type == 7 Life Items 续命点（紫点、1UP点）
	// type == 8 Unknown
	// type == 9 Faith Items 信仰点（绿点），满灵力时由P点转化而来
	// type == 10 Power Items P点（红点），由BOSS掉落
	// 点没有宽度和高度，自机靠近点时会自动收取，为了方便显示设定为6
	int type;
	Item(vec2 p, vec2 v, float w, float h, int type);
};

class DataReader
{
public:
	DataReader(HANDLE hProcess);

	void GetPlayerInfo(Player& plyr);
	void GetBulletsInfo(std::vector<Object>& bullets);
	void GetEnemiesInfo(std::vector<Object>& enemise);
	void GetLasersInfo(std::vector<Laser>& lasers);
	void GetItemsInfo(std::vector<Item>& items);

private:
	HANDLE GameProcess;

	template <class T> 
	T Read(int addr);
};