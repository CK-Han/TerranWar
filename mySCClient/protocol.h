#pragma once


#define MAX_BUFF_SIZE   4000
#define MAX_PACKET_SIZE  255

#define MY_SERVER_PORT  9000

#define MAX_STR_SIZE  100

#define MAX_ROOM 3000
#define MAX_ROOM_DATA 6
#define MAX_ROOM_PEOPLE 4

#ifndef ABOUT_OBJECT_ENUMS
#define ABOUT_OBJECT_ENUMS

#define ADD_ENEMY_INDEX 128
enum class OBJECT_TYPE {
	// 유닛
	Invalid = 0,		//0826 수정 서버랑 통합필요

	Ghost = 1,
	Hellion = 2,
	Marauder = 3,
	Marine = 4,
	Mule = 5,
	Reaper = 6,
	Scv = 7,
	Siegetank = 8,
	Thor = 9,

	Banshee = 64,
	BattleCruiser = 65,
	Medivac = 66,
	Raven = 67,
	Viking = 68,

	Ghost_Enemy = 1 + ADD_ENEMY_INDEX,
	Hellion_Enemy = 2 + ADD_ENEMY_INDEX,
	Marauder_Enemy = 3 + ADD_ENEMY_INDEX,
	Marine_Enemy = 4 + ADD_ENEMY_INDEX,
	Mule_Enemy = 5 + ADD_ENEMY_INDEX,
	Reaper_Enemy = 6 + ADD_ENEMY_INDEX,
	Scv_Enemy = 7 + ADD_ENEMY_INDEX,
	Siegetank_Enemy = 8 + ADD_ENEMY_INDEX,
	Thor_Enemy = 9 + ADD_ENEMY_INDEX,

	Banshee_Enemy = 64 + ADD_ENEMY_INDEX,
	BattleCruiser_Enemy = 65 + ADD_ENEMY_INDEX,
	Medivac_Enemy = 66 + ADD_ENEMY_INDEX,
	Raven_Enemy = 67 + ADD_ENEMY_INDEX,
	Viking_Enemy = 68 + ADD_ENEMY_INDEX,

	// 건물
	Armory = 256,
	Barrack = 257,
	Bunker = 258,
	Commandcenter = 259,
	Engineeringbay = 260,
	Factory = 261,
	Fusioncore = 262,
	Ghostacademy = 263,
	Missileturret = 264,
	Reactor = 265,
	Refinery = 266,
	Sensortower = 267,
	Starport = 268,
	Supplydepot = 269,
	Techlab = 270,

	Armory_Enemy = 256 + ADD_ENEMY_INDEX,
	Barrack_Enemy = 257 + ADD_ENEMY_INDEX,
	Bunker_Enemy = 258 + ADD_ENEMY_INDEX,
	Commandcenter_Enemy = 259 + ADD_ENEMY_INDEX,
	Engineeringbay_Enemy = 260 + ADD_ENEMY_INDEX,
	Factory_Enemy = 261 + ADD_ENEMY_INDEX,
	Fusioncore_Enemy = 262 + ADD_ENEMY_INDEX,
	Ghostacademy_Enemy = 263 + ADD_ENEMY_INDEX,
	Missileturret_Enemy = 264 + ADD_ENEMY_INDEX,
	Reactor_Enemy = 265 + ADD_ENEMY_INDEX,
	Refinery_Enemy = 266 + ADD_ENEMY_INDEX,
	Sensortower_Enemy = 267 + ADD_ENEMY_INDEX,
	Starport_Enemy = 268 + ADD_ENEMY_INDEX,
	Supplydepot_Enemy = 269 + ADD_ENEMY_INDEX,
	Techlab_Enemy = 270 + ADD_ENEMY_INDEX,


	Mineral = 511,
	Gas = 512
};
enum class Where { ground = 1, air = 2, both = 3 };
enum class State { stop = 0, move = 1, move_with_guard = 2, attack = 3, have_target = 4, death = 5, dig_resource = 6 };
enum class ROOMSTATE { STANDBY = 0, PLAYING = 1, REMOVED = 2 };

#endif


#pragma pack (push, 1)

//PACKET번호 0번부터 ~ 10번///////////////////////////////////////////
/////////////////////// 로비 시작 ////////////////////////////////////
//////////////////////////////////////////////////////////////////////

/* server -> client 패킷 */
enum class ERROR_NO { MAKE_ROOM = 0, ENTER_ROOM = 1, EXIT_ROOM = 2, OBJECT_MOVE = 3, CREATE_BUILDING = 4 };
#define SC_FAIL 0
struct sc_packet_fail {
	BYTE size;
	BYTE type;
	int err_no;
};
#define SC_EXIT_CLIENT 1
struct sc_packet_exit_client {
	BYTE size;
	BYTE type;
	int id;
};
#define SC_ROOM_ALL_DATA 2
struct room_data {
	int id;
	int state;
	int master_id;
	int num_of_people;
};
struct sc_packet_room_datas {
	BYTE size;
	BYTE type;
	int   page;
	room_data room_datas[MAX_ROOM_DATA];
};
#define SC_ROOM_DATA 3
struct sc_packet_room_data {
	BYTE size;
	BYTE type;
	int room_id;
	int master;
	int num_of_people;
	int people_id[MAX_ROOM_PEOPLE];
};
#define SC_ROOM_CANCLE 4
struct sc_packet_room_cancle {
	BYTE size;
	BYTE type;
};
#define SC_ROOM_START 5
struct sc_packet_room_start
{
	BYTE size;
	BYTE type;
};
#define SC_CLIENT_DATA 6
struct sc_packet_client_data
{
	BYTE size;
	BYTE type;

	int client_id;
};


/* client -> server 패킷 */
#define CS_MAKE_ROOM 1      // 방 만들기
struct cs_packet_make_room {
	BYTE size;
	BYTE type;
};
#define CS_ENTER_ROOM 2      // 방 참가
struct cs_packet_enter_room {
	BYTE size;
	BYTE type;
	int room_id;
};
#define CS_EXIT_ROOM 3      // 방 나가기
struct cs_packet_exit_room {
	BYTE size;
	BYTE type;
	int room_id;
};
#define CS_START_ROOM 4      // 게임 시작하기
struct cs_packet_start_room {
	BYTE size;
	BYTE type;
	int room_id;
};
#define CS_ROOM_LIST 5
struct cs_packet_room_list {
	BYTE size;
	BYTE type;
	int page;
};


//PACKET번호 10번부터/////////////////////////////////////////////////
/////////////////////// 게임시작 /////////////////////////////////////
//////////////////////////////////////////////////////////////////////



/* server -> client 패킷 */
#define SC_GAME_START 9
struct sc_packet_game_start
{
	BYTE size;
	BYTE type;
};
#define SC_PUT_OBJECT 10
struct sc_packet_put_object
{
	BYTE size;
	BYTE type;

	int client_id;

	OBJECT_TYPE object_type;
	int object_id;

	State state;
	Where m_where;

	int hp, mp;
	float x, y, z;
};
#define SC_PUT_BUILDING 11
struct sc_packet_put_building
{
	BYTE size;
	BYTE type;

	int client_id;

	OBJECT_TYPE object_type;
	int object_id;

	float x, y, z;

	int building_time;
};

#define SC_REMOVE_OBJECT 12
struct sc_packet_remove_object {
	BYTE size;
	BYTE type;

	int object_id;
};
#define SC_DATA_OBJECT 13
struct sc_packet_data_object
{
	BYTE size;
	BYTE type;

	int object_id;

	OBJECT_TYPE object_type;

	int building_time;

	State state;
	Where m_where;

	int hp, mp;
	float x, y, z;
};
#define SC_DATA_OBJECT_STATE 14
struct sc_packet_data_object_state
{
	BYTE size;
	BYTE type;

	int client_id;

	int object_id;
	int target_id;

	OBJECT_TYPE object_type;

	State state;
};
#define SC_DATA_OBJECT_POSITION 15
struct sc_packet_data_object_position
{
	BYTE size;
	BYTE type;

	int client_id;
	int object_id;

	OBJECT_TYPE object_type;

	float x, y, z;
};
#define SC_DATA_OBJECT_VALUE 16
struct sc_packet_data_object_value
{
	BYTE size;
	BYTE type;

	int client_id;

	int object_id;

	OBJECT_TYPE object_type;

	int hp, mp;

	int building_time;
};
#define SC_FAIL_OBJECT_MOVE 17
struct sc_packet_fail_object_move
{
	BYTE size;
	BYTE type;

	OBJECT_TYPE object_type;

	int client_id;

	int object_id;
};
#define SC_ATTACK_OBJECT 18
struct sc_packet_attack_object
{
	BYTE size;
	BYTE type;

	int client_id;
	int object_id;
	int target_id;

	OBJECT_TYPE object_type;
};
#define SC_ADD_NODE 19
struct sc_packet_add_node
{
	BYTE size;
	BYTE type;

	int index;
};
#define SC_REMOVE_NODE 20
struct sc_packet_remove_node
{
	BYTE size;
	BYTE type;

	int index;
};
#define SC_DATA_RESOURCE 21
struct sc_packet_data_resource
{
	BYTE size;
	BYTE type;

	int mineral;
	int gas;

	int client_id;
	int obj_id;
};
#define SC_GET_MINERAL 22
struct sc_packet_get_mineral
{
	BYTE size;
	BYTE type;

	int client_id;
	int obj_id;
};
#define SC_GET_GAS 23
struct sc_packet_get_gas
{
	BYTE size;
	BYTE type;

	int client_id;
	int obj_id;
};
#define SC_GAME_END 24
struct sc_packet_game_end
{
	BYTE size;
	BYTE type;

	bool Iswin;
};
/* client -> server 패킷 */
#define CS_GAME_READY 9
struct cs_packet_game_ready
{
	BYTE size;
	BYTE type;

	int client_id;

	int room_id;
};
#define   CS_MOVE_OBJECT 10
struct cs_packet_move_object {
	BYTE size;
	BYTE type;

	bool b_click_rbutton;

	int client_id;
	int room_id;
	int object_id;

	OBJECT_TYPE object_type;

	float x, y, z;
};
#define CS_ATTACK_OBJECT 12
struct cs_packet_attack_object {
	BYTE size;
	BYTE type;

	int room_id;
	int client_id;
	int object_id;

	OBJECT_TYPE object_type;

	int target_id;
};
#define CS_CREATE_BUILDING 13
struct cs_packet_create_building
{
	BYTE size;
	BYTE type;

	int client_id;

	int room_id;

	// 건물 만드는 object의 id
	int my_object_id;

	// 만드려는 건물 type
	OBJECT_TYPE object_type;

	// 지으려는 위치
	int index;
};
#define CS_DATA_OBJECT_STATE 14
struct cs_packet_data_object_state
{
	BYTE size;
	BYTE type;

	int room_id;
	int object_id;
	int target_id;

	OBJECT_TYPE object_type;

	State state;
};
#define CS_RESET_MOVE_OBJECT 15
struct cs_packet_reset_move_object {
	BYTE size;
	BYTE type;

	int room_id;
	int object_id;
};
#define CS_FRAME_COUNT 16
struct cs_packet_frame_count
{
	BYTE size;
	BYTE type;

	int room_id;

	int client_id;

	int frame_count;
};
#define CS_CREATE_UNIT 17
struct cs_packet_create_unit
{
	BYTE size;
	BYTE type;

	int room_id;
	int client_id;
	
	OBJECT_TYPE object_type;
	float x, y, z;
};
#define CS_DIG_RESOURCE 18
struct cs_packet_dig_resource
{
	BYTE size;
	BYTE type;

	int room_id;
	int client_id;

	int obj_id;   // scv id
	int resource_id; // 자원 id
	int command_id; // 커멘드 id            
};


#pragma pack (pop)