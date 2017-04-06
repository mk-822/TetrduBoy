// Tetra Break by ＭＫ 2005/11/02
#include "stdio.h"
#include "tetmain.h"
#include "blocks.h"
#include <Arduboy2.h>

extern Arduboy2 arduboy;
char NEXT_DATA[7][4][4];
char BL_ADJ_DATA[7][2][4];
char BL_CENTER_DATA[7][2];
char BL_DATA[7][2][4];

#define BL_OFFSET_X 2
#define BL_OFFSET_Y 2
#define FIX_INTERVAL 30

int GetRand(int max){
  return (random() / 100) % max;
}
  
//ここからメイン関数
int TetrisGame::initialize()
{
	//各種変数初期化
	next=0;			//ねくすと
	over1g_level=60;//1Gを超えるレベル
	fall_count=0;	//おちるかうんと
	fix_count=0;	//固定した回数
	key=0;			//入力状態

	//ブロック・ブロック補正情報を定義ファイルから読み込み
  const char *ptr = BLOCK_DEF;
	for(int i=0 ; i<28 ; i++){
    for(int j=0 ; j<4 ; j++){
      NEXT_DATA[i/4][i%4][j] = pgm_read_word_near(ptr++);
    }
	}
	for(int i=0 ; i<14 ; i++){
    for(int j=0 ; j<4 ; j++){
      BL_DATA[i/2][i%2][j] = pgm_read_word_near(ptr++);
    }
	}
	for(int i=0 ; i<14 ; i++){
    for(int j=0 ; j<4 ; j++){
      BL_ADJ_DATA[i/2][i%2][j] = pgm_read_word_near(ptr++);
    }
	}
	for(int i=0 ; i<7 ; i++){
    for(int j=0 ; j<2 ; j++){
      BL_CENTER_DATA[i][j] = pgm_read_word_near(ptr++);
    }
	}

	level = 0;
	enable_levelup = true;
	enable_over1g = true;
	fix_count = 0;
	fixed_flag = true;
	score = 0;
	scene = 1;
	next = GetRand(6);
	fix_ok_flag=0;
	fieldInit(field);
}

char updateInput(){
  return
     arduboy.pressed(A_BUTTON) |
    (arduboy.pressed(B_BUTTON) << 1) |
    (arduboy.pressed(UP_BUTTON) << 2) |
    (arduboy.pressed(DOWN_BUTTON) << 3) |
    (arduboy.pressed(LEFT_BUTTON) << 4) |
    (arduboy.pressed(RIGHT_BUTTON) << 5);
}

void TetrisGame::update(){
	keyold[0] = key;
	for(int i=KEY_DELAY_TIME-1 ; 0<i ; i--){
		keyold[i] = keyold[i-1]&key;
	}
	key = updateInput();
	keyonce = key & (keyold[0] ^ 0xFF);
	keyleg = keyonce|(keyold[KEY_DELAY_TIME-1]);

	switch(scene){
	case MENU:
		break;
	case MAIN:
		if(fixed_flag){
			add_score = blockErace(field)*10;
			score += add_score * (add_score/2);
			if(key&KEY_A)
				spin=-1;
			else if(key&KEY_B)
				spin=1;
			else
				spin=0;
			error = blockGen(spin, next, &block, field);
			if(error){
				scene=GAME_OVER;
				break;
			}
			fixed_flag = 0;
			next = -1;
		}
	
		if(keyonce&KEY_A)
			spin=-1;
		else if(keyonce&KEY_B)
			spin=1;
		else
			spin=0;

		if(next == -1){	//ネクストが無効だった場合、新規に生成。
			next = GetRand(6);
		}
		fall_count--;
		if(fall_count <= 0){
			fall_count = over1g_level - level;
			g = 1;
			if(fall_count<0){
				for(int g=1 ; g<=20 ; g++){
					fall_count++;
					if(fall_count==0)
						break;
				}
			}
			if(!enable_over1g)
				g = 1;
			if(fix_ok_flag){
				fix_count++;
				if(fix_count%5==0)level++;

				blockFix(&block , field);
				fixed_flag=true;
				fix_ok_flag=false;
			}else{
				for(int i=g;i>0;i--){
					blockMove(0,1,&block);
					error = blockJudge(&block,field);
					if(error==1){
						fix_ok_flag=true;
						fall_count += 30;
						break;
					}
				}
			}
		}
		if(keyonce&KEY_UP){
			if(fix_ok_flag){
				fix_count++;
				if(fix_count%5==0)level++;

				blockFix(&block , field);
				fixed_flag=true;
				fix_ok_flag=false;
			}else{
				for(int i=20;i>0;i--){
					blockMove(0,1,&block);
					error = blockJudge(&block,field);
					if(error==1){
						fix_ok_flag=true;
						fall_count += 30;
						break;
					}
				}
			}
		}else if(keyleg&KEY_LEFT){
			blockMove(-1,0,&block);
			error = blockJudge(&block,field);
			if(error==2)
				blockMove(1,0,&block);
			if(error==1)
				fix_ok_flag=1;
			if(error==0)
				fix_ok_flag=0;
		}else if(keyleg&KEY_RIGHT){
			blockMove(1,0,&block);
			error = blockJudge(&block,field);
			if(error==2)
				blockMove(-1,0,&block);
			if(error==1)
				fix_ok_flag=1;
			if(error==0)
				fix_ok_flag=0;
		}else if(key&KEY_DOWN){
			if(fix_ok_flag){
				fix_count++;
				if(fix_count%5==0)level++;

				blockFix(&block , field);
				fixed_flag=true;
				fix_ok_flag=false;
			}else{
				blockMove(0,1,&block);
				error = blockJudge(&block,field);
				if(error==1){
					fix_ok_flag=true;
					fall_count += 30;
					break;
				}
			}
		}
		if(spin){
			blockSpin(spin,&block);		//回転
			error = blockJudge(&block,field);
			if(error==2){
				blockMove(-1,0,&block);		//ダメなら左にズレてみろ！！
				error = blockJudge(&block,field);
				if(error==2){
					blockMove(2,0,&block);		//それでもダメなら右だ！！
					error = blockJudge(&block,field);
					if(error==2){
						blockMove(-1,0,&block);//全部ダメなら元に戻すしかない…！！
						blockSpin(spin*-1,&block);
					}
				}
			}
			if(error==1)
				fix_ok_flag=1;
			if(error==0)
				fix_ok_flag=0;
		}
		break;
	case GAME_OVER:
		printf("げーむ おーばー\n\n");
		printf("SCORE:%d LEVEL:%d\n\nPress Z key.",score,level);
		break;
	default:
		break;
	}
}

void TetrisGame::draw()
{
  const int FIELD_OFFSET_X = 40;
  const int FIELD_OFFSET_Y = -5;
  
  for(int i=0; i<4; i++){
    arduboy.drawRect(
      block.x[i] * 3 + FIELD_OFFSET_X,
      block.y[i] * 3 + FIELD_OFFSET_Y, 2, 2);
  }
  
  for(int j=BL_OFFSET_Y ; j<24-BL_OFFSET_Y; j++){
    for(int i=BL_OFFSET_X ; i<14-BL_OFFSET_X ; i++){
      if(*(field+i*24+j) != 0)
        arduboy.drawRect(
          i * 3 + FIELD_OFFSET_X, j * 3 + FIELD_OFFSET_Y, 2, 2);
    }
  }

  arduboy.drawRect(43, 1, 2, 62);
  arduboy.drawRect(76, 1, 2, 62);
  arduboy.drawRect(45, 61, 31, 2);
}

int TetrisGame::blockErace(char* field)
{
	int erace=0,ok;

	for(int j=BL_OFFSET_Y ; j<20+BL_OFFSET_Y ; j++){
		ok=0;
		for(int i=BL_OFFSET_X ; i<10+BL_OFFSET_X ; i++){
			if(*(field+i*24+j)!=0)
				ok++;
		}
		if(ok==10){
			for(int k=j ; j>1 ; j--){
				for(int i=BL_OFFSET_X ; i<10+BL_OFFSET_X ; i++){
					*(field+i*24+j) = *(field+i*24+j-1);
				}
			}
			erace++;
		}
	}
	return erace;
}

int TetrisGame::blockGen(int spin, int next, Block* data, char* field)
{
	int k=0;

	data->color = next;
	data->spin = 0;
	data->ct_x = BL_CENTER_DATA[next][0] + BL_OFFSET_X;
	data->ct_y = BL_CENTER_DATA[next][1] + BL_OFFSET_Y;
	for(int i=0 ; i<4 ; i++){
		data->hs_x[i] = BL_ADJ_DATA[next][0][i];
		data->hs_y[i] = BL_ADJ_DATA[next][1][i];
		data->x[i] = BL_DATA[next][0][i] + BL_OFFSET_X;
		data->y[i] = BL_DATA[next][1][i] + BL_OFFSET_Y;
	}
	blockSpin(spin,data);
	for(int i=0 ; i<4 ; i++){
		if(*(field + data->x[i]*24 + data->y[i])==1){
			k++;
			break;
		}
	}
	return k;
}

void TetrisGame::blockFix(Block* data, char* field)
{
	int x,y,i;
	for(int i=0 ; i<4 ; i++){
		x = data -> x[i];
		y = data -> y[i];
		*(field+x*24+y) = data -> color+1;
	}
}

int TetrisGame::blockJudge(Block* data, char* field)
{
	int error=0;

	for(int i=0 ; i<4 ; i++){
		if(*(field + data->x[i]*24 + data->y[i])){
			error=2;
			break;
		}
	}
	if(error!=2){
		for(int i=0 ; i<4 ; i++){
			if(*(field + data->x[i]*24 + data->y[i]+1)){
				error=1;
				break;
			}
		}
	}
	return error;
}

void TetrisGame::blockMove(int x, int y, Block* data)
{
	for(int i=0 ; i<4 ; i++){
		data->x[i] += x;
		data->y[i] += y;
	}
	data->ct_x += x;
	data->ct_y += y;
}

void TetrisGame::blockSpin(int spin, Block* data)
{
	int x,y,tmp;
	tmp = data->spin;

	for(int i=0 ; i<4 ; i++){
		data->x[i] -= data->hs_x[tmp];
		data->y[i] -= data->hs_y[tmp];
	}
	tmp += spin;
	if(tmp<0)
		tmp=3;
	if(tmp>3)
		tmp=0;
	data->spin = tmp;

	switch(spin){
	case 1:
		for(int i=0 ; i<4 ; i++){
			x = ((data->y[i] - data->ct_y) * -1) + data->ct_x;
			y = data->x[i] - data->ct_x + data->ct_y;
			data->x[i] = x;
			data->y[i] = y;
		}
		break;
	case -1:
		for(int i=0 ; i<4 ; i++){
			x = data->y[i] - data->ct_y + data->ct_x;
			y = ((data->x[i] - data->ct_x) * -1) + data->ct_y;
			data->x[i] = x;
			data->y[i] = y;
		}
		break;
	default:
		break;
	}

	for(int i=0 ; i<4 ; i++){
		data->x[i] += data->hs_x[tmp];
		data->y[i] += data->hs_y[tmp];
	}
}

void TetrisGame::fieldInit(char* field)
{
	for(int i=0 ; i<13 ; i++){	//フィールド初期化
		for(int j=0 ; j<23 ; j++){
			*(field+i*24+j) = 0;
		}
	}
	for(int i=1 ; i<13 ; i+=11){ //左右に壁作成
		for(int j=1 ; j<23 ; j++){
			*(field+i*24+j) = 8;
		}
	}
	for(int i=1 ; i<12 ; i++){ //上下に壁作成
		*(field+i*24+22) = 8;
	}
}
