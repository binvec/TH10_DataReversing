#include "data.h"


vec2::vec2() : x(0), y(0) {}
vec2::vec2(float x, float y) : x(x), y(y) {}
vec2::vec2(const vec2& other) : x(other.x), y(other.y) {}
vec2::vec2(vec2&& other) noexcept : x(other.x), y(other.y) {}
vec2& vec2::operator=(const vec2& other) 
{
	if (this == &other)
		return *this;
	x = other.x;
	y = other.y;
	return *this;
}
vec2& vec2::operator=(vec2&& other) noexcept
{
	if (this == &other)
		return *this;
	x = other.x;
	y = other.y;
	return *this;
}
vec2 operator+(const vec2& a, const vec2& b)
{
	return vec2(a.x + b.x, a.y + b.y);
}
vec2 operator*(const vec2& a, float o)
{
	return vec2(a.x * o, a.y * o);
}
vec2 vec2::rotate(float arc) const
{
	return vec2(x * cos(arc) - y * sin(arc), x * sin(arc) + y * cos(arc));
}



Object::Object(vec2 p, vec2 v, float w, float h)
	: p(p), v(v), w(w), h(h)
{}

Item::Item(vec2 p, vec2 v, float w, float h, int type) : Object(p, v, w, h), type(type)
{}

Laser::Laser(vec2 p, vec2 v, float w, float h, float arc) : Object(p, v, w, h), arc(arc)
{}

std::vector<vec2> Laser::getVertices() const
{
	std::vector<vec2> vertices;
	// ��ʱ����
	vec2 points[] = {
		vec2(0, w / 4), vec2(0, -w / 4),
		vec2(h, -w / 4), vec2(h, w / 4)
	};
	for (int i = 0; i < 4; ++i)
	{
		vertices.push_back(points[i].rotate(arc) + p);
	}
	return vertices;
}
DataReader::DataReader(HANDLE hProcess): GameProcess(hProcess) {}

template <class T> 
T DataReader::Read(int addr)
{
	DWORD nbr;
	T ret;
	ReadProcessMemory(GameProcess, (LPCVOID)addr, &ret, 4, &nbr);
	return ret;
}

void DataReader::GetBulletsInfo(std::vector<Object>& bullets)
{
	bullets.clear();
	int base_addr = Read<int>(0x004776F0);
	if (base_addr == 0) 
		return;
	int ebx = base_addr + 0x60;
	for (int i = 0; i < 2000; i++)
	{
		int edi = ebx + 0x400;
		int bp = Read<int>(edi + 0x46);
		bp &= 0x0000FFFF;
		if (bp)
		{
			int eax = Read<int>(0x477810);
			if (eax)
			{
				eax = Read<int>(eax + 0x58);
				if (!(eax & 0x00000400))
				{
					float x = Read<float>(ebx + 0x3B4);
					float y = Read<float>(ebx + 0x3B8);
					float w = Read<float>(ebx + 0x3F0);
					float h = Read<float>(ebx + 0x3F4);
					float dx = Read<float>(ebx + 0x3C0);
					float dy = Read<float>(ebx + 0x3C4);
					bullets.emplace_back(Object(vec2(x, y), vec2(dx, dy), w, h));
				}
			}
		}
		ebx += 0x7F0;
	}
}

void DataReader::GetEnemiesInfo(std::vector<Object>& enemies)
{
	enemies.clear();
	int base_addr = Read<int>(0x00477704);
	if (base_addr == 0)
	{
		return;
	}
	base_addr = Read<int>(base_addr + 0x58);
	if (base_addr)
	{
		while (true)
		{
			int obj_addr = Read<int>(base_addr);
			int obj_next = Read<int>(base_addr + 0x4);
			obj_addr += 0x103C;
			unsigned int t = Read<unsigned int>(obj_addr + 0x1444);
			if ((t & 0x40) == 0)
			{
				if ((t & 0x12) == 0) 
				{
					float x = Read<float>(obj_addr + 0x2C);
					float y = Read<float>(obj_addr + 0x30);
					float w = Read<float>(obj_addr + 0xB8);
					float h = Read<float>(obj_addr + 0xBC);
					float dx = Read<float>(obj_addr + 0x38);
					float dy = Read<float>(obj_addr + 0x3C);
					enemies.emplace_back(Object(vec2(x, y), vec2(dx, dy), w, h));
				}
			}
			if (obj_next == 0)
			{
				break;
			}
			base_addr = obj_next;
		}
	}
}

void DataReader::GetItemsInfo(std::vector<Item>& items)
{
	items.clear();
	int base_addr = Read<int>(0x00477818);
	if (base_addr == 0)
	{
		return;
	}
	int esi = base_addr + 0x14;
	int ebp = esi + 0x3B0;
	for (int i = 0; i < 2000; i++)
	{
		int eax = Read<int>(ebp + 0x2C);
		// ������ͷ�Ϊ���¼���
		// eax == 0 ������
		// eax == 1 �����Ŀ��յ�
		// eax == 2 ��B�����ӵ������ĵ�
		// eax == 3 �����յ��ߡ���B���Զ����յĵ�
		// eax == 4 ��������ȡ��Χ���Զ����յĵ�
		if (eax == 1)
		{
			float x = Read<float>(ebp - 0x4);
			float y = Read<float>(ebp);
			float dx = Read<float>(ebp + 0x8);
			float dy = Read<float>(ebp + 0xC);
			int type = Read<int>(ebp + 0x30);
			// �������Ϊ���¼���
			// type == 1 Power Items P�㣨��㣩
			// type == 2 Point Items �÷ֵ㣨���㣩
			// type == 3 Faith Items �����㣨�̵㣩
			// type == 4 Large Power Items ��P�㣨��㣩
			// type == 5 Large Point Items ��÷ֵ㣨����ɫ���ߵ����㣩����BOSS����
			// type == 6 Unknown
			// type == 7 Life Items �����㣨�ϵ㡢1UP�㣩
			// type == 8 Unknown
			// type == 9 Faith Items �����㣨�̵㣩��������ʱ��P��ת������
			// type == 10 Power Items P�㣨��㣩����BOSS����
			// ��û�п�Ⱥ͸߶ȣ��Ի�������ʱ���Զ���ȡ��Ϊ�˷�����ʾ�趨Ϊ6
			items.emplace_back(Item(vec2(x, y), vec2(dx, dy), 6, 6, type));
		}
		ebp += 0x3F0;
	}
}

void DataReader::GetPlayerInfo(Player& plyr)
{
	int base_addr = Read<int>(0x00477834);
	if (base_addr == 0)
	{
		plyr.p.x = 0;
		plyr.p.y = 400;
		plyr.h = plyr.w = 0;
		return;
	}
	plyr.p.x = Read<float>(base_addr + 0x3C0);
	plyr.p.y = Read<float>(base_addr + 0x3C4);
	plyr.v.x = Read<int>(base_addr + 0x3F0) / 100.0f;
	plyr.v.y = Read<int>(base_addr + 0x3F4) / 100.0f;
	plyr.w = Read<float>(base_addr + 0x41C) * 2;
	plyr.slow = Read<int>(base_addr + 0x4474);
	plyr.h = plyr.w;
	plyr.powers = Read<int>(0x00474C48) / 20;
	plyr.type = Read<int>(0x00474C68);
	plyr.itemObtainRange = Read<float>(0x00476FB0) + plyr.type * 4;
	if (plyr.slow)
	{
		plyr.itemObtainRange *= 2.5;
	}
	plyr.life = Read<int>(0x474C70) + 1;
	plyr.status = Read<int>(base_addr + 0x458);
	plyr.invinibleTime = Read<int>(base_addr + 0x4310);
}

void DataReader::GetLasersInfo(std::vector<Laser>& lasers)
{
	lasers.clear();
	int base_addr = Read<int>(0x0047781C);
	if (base_addr == 0)
	{
		return;
	}
	int obj_addr = Read<int>(base_addr + 0x18);
	if (obj_addr)
	{
		while (true)
		{
			int obj_next = Read<int>(obj_addr + 0x8);
			float x = Read<float>(obj_addr + 0x24);
			float y = Read<float>(obj_addr + 0x28);
			float arc = Read<float>(obj_addr + 0x3C);
			float h = Read<float>(obj_addr + 0x40);
			float w = Read<float>(obj_addr + 0x44);
			float dx = Read<float>(obj_addr + 0x30);
			float dy = Read<float>(obj_addr + 0x34);
			lasers.push_back(Laser(vec2(x, y), vec2(dx, dy), w, h, arc));
			if (obj_next == 0)
			{
				break;
			}
			obj_addr = obj_next;
		}
	}
}