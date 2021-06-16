#ifndef TRANSSTRUCTURE_H
#define TRANSSTRUCTURE_H

const int MAX_CONNECTIONS = 11;
const int MAX_NAME_LENGTH = 9;
//报头处理
const int HEAD_LENGTH = 4;
/*
* 客户端动作
* GU = 查询uid
* SN = 设置名字
* PA = 玩家操作信息
* ST = 开始游戏
* GS = 收到开始信息
* 
* 服务端动作
* SU = 发送uid
* RO = 房间信息
* MP = 地图信息
* GO = 游戏结束
* ST = 通知所有玩家开始游戏
* MI = 初始化地图变量
*/



#define PA_NONE 0
#define PA_LEFT 1
#define PA_RIGHT 2
#define PA_UP 3
#define PA_DOWN 4
#define PA_PICK 5
#define PA_DISCARD 6

struct PlayerAction
{
	short speed[4] = { 0 };
	bool is_shoot = false;
	float shoot_degree, shoot_speed;
	int shoot_damage;
	bool pick = false;
};

struct PlayerInformation
{
	bool alive = 0;
	bool is_pick = false;
	bool is_shoot = false;
	short bullet = 0;
	int uid = 0;
	int hp = 100;
	float position_x = 0, position_y = 0;
	float shoot_degree, shoot_speed;
};

struct RoomInformation
{
	int player_num;
	bool player_alive[MAX_CONNECTIONS];
	char player_name[MAX_CONNECTIONS][10];
};

struct MapInformation
{
	bool is_updated = false;
	int player_left_num;
	PlayerInformation player[MAX_CONNECTIONS];
};

struct MapInformationInit
{
	bool is_updated = false;
	float m_bandage_position[30][2] = { 0 };
	float m_ammunition_position[30][2] = { 0 };
	float m_weapon_position[20][2] = { 0 };
	int m_weapon_type[20] = { 0 };
};

#endif