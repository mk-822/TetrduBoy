#pragma once
#include "stdio.h"
#define KEY_DELAY_TIME 20
#define KEY_A 1
#define KEY_B 2
#define KEY_UP 4
#define KEY_DOWN 8
#define KEY_LEFT 16
#define KEY_RIGHT 32 

enum STATE
{
	MENU,
	MAIN,
	GAME_OVER,
};

struct Block	//現在のブロック用構造体
{
	char color;		//ブロックの種類
	char spin;		//現在の回転状況
	char x[4];		//絶対座標
	char y[4];
	char ct_x;		//絶対座標の中心
	char ct_y;
	char hs_x[4];	//回転時の補正量
	char hs_y[4];
};

class TetrisGame{
public:
	int initialize();
	void update();
  void draw();

private:
  // ブロック定義の読み込み先。newしただけでは空っぽ
  char NEXT_DATA[7][4][4];
  char BL_ADJ_DATA[7][2][4];
  char BL_CENTER_DATA[7][2];
  char BL_DATA[7][2][4];

	char field[14 * 24];	//フィールド
	int level;			//ゲームレベル
	char scene;			//現在のシーン(ゲーム状態)
	char cur_menu;		//選択されたメニュー番号
	int next;			//ねくすと
	int over1g_level;	//1Gを超えるレベル
	int fall_count;		//おちるかうんと
	int fix_count;		//固定した回数
	int score;			//点数
	int add_score;		//スコア追加用
	int g;				//重力
	int spin;			//回転方向
	int error;			//接地してたり、かぶってたり。
	Block block;		//現在のブロック
	bool enable_over1g;	//1Gを超える重力を許可するかどうか
	bool enable_levelup;//レベルアップを許可するかどうか
	bool fixed_flag;	//固定済みかどうか。これがONならネクストを現在のブロックに。
	bool fix_ok_flag;	//接地しているかどうか。
	char key;			//入力状態
	char keyold[KEY_DELAY_TIME];	//入力状態のバックアップ。
	char keyonce;		//ボタンを押した瞬間だけ反応する入力状態。
	char keyleg;			//ト…トトトトトトみたいな感じ(謎)で反応する入力状態。
	
	int blockErace(char* field);
	int blockGen(int spin, int next, Block* data, char* field);
	void blockFix(Block* data, char* field);
	int blockJudge(Block* data, char* field);
	void blockMove(int x, int y, Block* data);
	void blockSpin(int spin, Block* data);
	void fieldInit(char* field);
};
