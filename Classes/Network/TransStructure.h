#ifndef TRANSSTRUCTURE_H
#define TRANSSTRUCTURE_H

const int MAX_CONNECTIONS = 11;
const int MAX_NAME_LENGTH = 9;
//Header Field
const int HEAD_LENGTH = 4;
/*
* Client Command
* GU = get uid
* SN = set name
* PA = send player action
* ST = start game
* GS = get started
* CT = chat
* 
* Server Command
* SU = send uid
* RO = room information
* MP = map information
* GO = game over
* ST = start game
* MI = map init
* CT = chat
*/



#define PA_NONE 0
#define PA_LEFT 1
#define PA_RIGHT 2
#define PA_UP 3
#define PA_DOWN 4
#define PA_PICK 5
#define PA_DISCARD 6

struct PlayerAction {
    bool is_shoot = false;
    bool is_drop_grenade = false;
    bool is_bleed_locked = false;
    bool is_bullet_locked = false;
    short speed[4] = {0};
    short weapon_type = 0;
    float bullet_x = 0, bullet_y = 0;
    float grenade_x = 0, grenade_y = 0;
    int shoot_damage;
    bool pick = false;
};

struct PlayerInformation {
    bool just_game_over = false;
    bool alive = false;
    bool is_pick = false;
    bool is_shoot = false;
    bool is_drop_grenade = false;
    short bullet = 0;
    short grenade = 0;
    short weapon_type = 0;
    int uid = 0;
    int hp = 100;
    float position_x = 0, position_y = 0;
    float grenade_x = 0, grenade_y = 0;
    float bullet_x = 0, bullet_y = 0;

};

struct RoomInformation {
    int player_num;
    bool player_alive[MAX_CONNECTIONS];
    char player_name[MAX_CONNECTIONS][10];
};

struct MapInformation {
    bool is_updated = false;
    int player_left_num;
    PlayerInformation player[MAX_CONNECTIONS];
};

struct MapInformationInit {
    bool is_updated = false;
    bool player_attended[10] = {0};
    int player_num_all = 0;
    float m_bandage_position[30][2] = {0};
    float m_ammunition_position[30][2] = {0};
    float m_weapon_position[20][2] = {0};
    int m_weapon_type[20] = {0};
};

struct ChatInformation{
    bool has_new_message = false;
    int send_uid = 0;
    char message[64];
};
#endif