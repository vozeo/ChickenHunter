#ifndef TRANSSTRUCTURE_H
#define TRANSSTRUCTURE_H

const int MAX_CONNECTIONS = 10;

//报头处理
const int HEAD_LENGTH = 4;
/*
* 客户端动作
* GU = 查询uid
* SN = 设置名字
* PA = 玩家操作信息
* 
* 服务端动作
* SU = 发送uid
* RO = 房间信息
* MP = 地图信息
* 
* 
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
	const char head[HEAD_LENGTH] = "PA";
	short keyboard_action = PA_NONE;
	short hp_change = 0;
	bool is_shoot = false;
	float shoot_degree, shoot_speed;
};

struct PlayerInformation
{
	bool is_dead = 0;
	int uid = 0;
	char num[10];
	int hp = 100;
	float position_x = 0, position_y = 0;
	bool is_shoot = false;
	float shoot_degree, shoot_speed;
	void clear();
};

struct RoomInformation
{
	const char head[HEAD_LENGTH] = "RO";
	int player_num;
	char player_name[MAX_CONNECTIONS][10];
};

struct MapInformation
{
	const char head[HEAD_LENGTH] = "MP";
	int player_num;
	PlayerInformation player[MAX_CONNECTIONS];
};

#endif 