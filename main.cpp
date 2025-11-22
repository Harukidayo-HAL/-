#include "conioex_new.h"
#include <cstdlib> // rand(), srand() 用
#include <ctime> // time() 用
#include <algorithm> // std::shuffle 用
#include <random> // std::mt19937, std::random_device
#include <set>
#include <iterator> // std::advance を使うために必要
#include <windows.h> // Sleep() を使うため
#include <iostream>
using namespace std;

void resetGame();
void initializeGame();
void reset();

std::set<int>usedFireIndices;//ファイアー
std::set<int>usedWalterIndices;//ウォーター
std::set<int>usedGreenIndices;//グリーン
std::set<int>usedWhiteIndices;//ホワイト



std::set<int> usedTargetIndices; // 使用済み出現位置の管理

//キャラクターの動き
void kaede_right_move();
void kaede_front_move();
void kaede_left_move();
void kaede_back_move();
//キャラクターの入力処理
void updateCharacterMovement();
// キャラクターを描画
void drawCharacter();
// 毎フレームの処理
void gameLoop();

void ChangeSpeed();



bool CheckCollision(int x1, int y1, int w1, int h1,
    int x2, int y2, int w2, int h2) {
    return (x1 < x2 + w2 &&
        x1 + w1 > x2 &&
        y1 < y2 + h2 &&
        y1 + h1 > y2);
}

//あたり判定//
void CheckPlayerCollision();
void CheckBomCollision();

int enemy_kill = 0;



void SpawnEnemy();
void MoveEnemies();

void chooseImage(int type, std::set<int>& usedIndices, bool isUsed[], int& currentIndex, int& selectedIndex);
void chooseFireImage();
void chooseWalterImage();
void chooseGreenImage();
void chooseWhiteImage();
void drawImages();//描画する画像を管理する

void onMouseDown(int mouseX, int mouseY);//マウスドラッグで画像を動かす
void onMouseMove(int mouseX, int mouseY);
void onMouseUp();

void drawNumber(int number, int x, int y);

void spawn_bom(int x, int y);

enum ImageType { IMG_NONE, FIRE, WALTER ,green ,white}; // 画像の種類を管理する列挙型
ImageType imageType[6] = { IMG_NONE, IMG_NONE, IMG_NONE,IMG_NONE, IMG_NONE, IMG_NONE }; // 各ターゲット位置に表示する画像を管理


using namespace std;

int* game_bgm = 0;
int* title_bgm = 0;
int* rose = 0;
int* bom_set = 0;
int* win = 0;
int* win_bgm = 0;
int* bom = 0;


//Cキートリガー
int c_t = 0;
int c_n = 0;
int c_o = 0;


//スライディング
int normal_speed = 5;
int boost_speed = 15;
int change_speed = 0;
int speed = 0;
int slide= 0;
int slide_change = 0;
int* slide_sound = 0;

// Sliding system variables
bool slidingActive = false;            // 現在スライド中か
int slideHoldCounter = 0;             // SHIFT を押し続けたフレーム数
int slideTimerGlobal = 0;             // スライドの残りフレーム
int slideDirX = 0;                    // スライド方向ベクトル
int slideDirY = 0;
const int SLIDE_HOLD_FRAMES = 15;     // SHIFT を押し続けてスライドを開始するまでのフレーム数
const int SLIDE_DURATION_FRAMES = 30; // スライド継続フレーム数


// **ドラッグ時の一時的な座標を保持する配列**
int tempX = -1;
int tempY = -1;


//gameのシーン切り替え
int GameScene = 0;

time_t startTime; // ゲーム開始時間

bool isUsedTarget[6] = { false };  // 各スロットの使用状態を管理


bool isUsedFire[6] = { false };  // FIRE の使用状態を管理
bool isUsedWalter[6] = { false };  // WALTER の使用状態
bool isUsedGreen[6] = { false };  // green の使用状態
bool isUsedWhite[6] = { false };  // white の使用状態


int cursorX = 0;
int cursorY = 0;
int mouseX = 0;
int mouseY = 0;
bool isRightClick = false;//右クリックしてるかどうか


bool isDragging = false;  // ドラッグ中かどうか
int draggingIndex = -1;   // 現在ドラッグ中の画像のインデックス
int offsetX = 0, offsetY = 0; // ドラッグ開始時のオフセット



//キャラクターの位置
int player_x = 750;
int player_y = 300;
bool isFacingRight = false;
bool isFacingLeft = false;
bool isFacingUp = false;
bool isFacingDown = false;

int clearTime = 0;  // ゲームクリア時の秒数
bool gameCleared = false;  // ゲームクリア状態を管理

// プレイヤーの座標とサイズ

int playerWidth = 48;
int playerHeight = 48;

// 敵の座標とサイズ
int enemyWidth = 128;
int enemyHeight = 128;

//キャラ画像

Bmp* kaede_front[3] = {LoadBmp("pic/kaede_front_1.bmp") ,
 LoadBmp("pic/kaede_front_2.bmp") ,
 LoadBmp("pic/kaede_front_3.bmp") };

Bmp* kaede_back[3] = {LoadBmp("pic/kaede_back_1.bmp"),
 LoadBmp("pic/kaede_back_2.bmp") ,
 LoadBmp("pic/kaede_back_3.bmp") };

Bmp* kaede_right[3] = {LoadBmp("pic/kaede_right_1.bmp"),
LoadBmp("pic/kaede_right_2.bmp") ,
 LoadBmp("pic/kaede_right_3.bmp") };

Bmp* kaede_left[3] = {
    LoadBmp("pic/kaede_left_1.bmp"),
    LoadBmp("pic/kaede_left_2.bmp") ,
    LoadBmp("pic/kaede_left_3.bmp") };

// 現在のキャラ画像

Bmp* currentImage = kaede_front[0];//初期画像

int ani_data[] = { 0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,0,0xff };
int ani_cnt = 0;

Bmp* fire_bom[10] = {
    LoadBmp("pic/bom_fire_1.bmp"),
    LoadBmp("pic/bom_fire_1 - コピー.bmp"),
     LoadBmp("pic/bom_fire_1 - コピー (2).bmp"),
     LoadBmp("pic/bom_fire_1 - コピー (3).bmp"),
     LoadBmp("pic/bom_fire_1 - コピー (4).bmp"),
     LoadBmp("pic/bom_fire_1 - コピー (5).bmp"),
     LoadBmp("pic/bom_fire_1 - コピー (6).bmp"),
     LoadBmp("pic/bom_fire_1 - コピー (7).bmp"),
     LoadBmp("pic/bom_fire_1 - コピー (8).bmp"),
     LoadBmp("pic/bom_fire_1 - コピー (9).bmp"),
};
Bmp* walter_bom[10] = {
    LoadBmp("pic/bom_walter_1.bmp"),
       LoadBmp("pic/bom_walter_1 - コピー.bmp"),
     LoadBmp("pic/bom_walter_1 - コピー (2).bmp"),
     LoadBmp("pic/bom_walter_1 - コピー (3).bmp"),
     LoadBmp("pic/bom_walter_1 - コピー (4).bmp"),
     LoadBmp("pic/bom_walter_1 - コピー (5).bmp"),
     LoadBmp("pic/bom_walter_1 - コピー (6).bmp"),
     LoadBmp("pic/bom_walter_1 - コピー (7).bmp"),
     LoadBmp("pic/bom_walter_1 - コピー (8).bmp"),
     LoadBmp("pic/bom_walter_1 - コピー (9).bmp"),
};
Bmp* green_bom[10] = {
        LoadBmp("pic/bom_green_1.bmp"),
       LoadBmp("pic/bom_green_1 - コピー.bmp"),
     LoadBmp("pic/bom_green_1 - コピー (2).bmp"),
      LoadBmp("pic/bom_green_1 - コピー (3).bmp"),
       LoadBmp("pic/bom_green_1 - コピー (4).bmp"),
        LoadBmp("pic/bom_green_1 - コピー (5).bmp"),
         LoadBmp("pic/bom_green_1 - コピー (6).bmp"),
          LoadBmp("pic/bom_green_1 - コピー (7).bmp"),
           LoadBmp("pic/bom_green_1 - コピー (8).bmp"),
            LoadBmp("pic/bom_green_1 - コピー (9).bmp"),
};
Bmp* white_bom[10] = {
        LoadBmp("pic/bom_white_1.bmp"),
       LoadBmp("pic/bom_white_1 - コピー.bmp"),
     LoadBmp("pic/bom_white_1 - コピー (2).bmp"),
      LoadBmp("pic/bom_white_1 - コピー (3).bmp"),
       LoadBmp("pic/bom_white_1 - コピー (4).bmp"),
        LoadBmp("pic/bom_white_1 - コピー (5).bmp"),
         LoadBmp("pic/bom_white_1 - コピー (6).bmp"),
          LoadBmp("pic/bom_white_1 - コピー (7).bmp"),
           LoadBmp("pic/bom_white_1 - コピー (8).bmp"),
            LoadBmp("pic/bom_white_1 - コピー (9).bmp"),
};




// **爆弾の情報**
struct Bom {
    int x;
    int y;
    bool drawn;
};

// **各爆弾の最大数**
const int MAX_BOM = 10;

// **爆弾リスト**
Bom fire_boms[MAX_BOM];
Bom walter_boms[MAX_BOM];
Bom green_boms[MAX_BOM];
Bom white_boms[MAX_BOM];

// **fire_bom の座標を管理する構造体**
/*struct FireBom {
    int x;
    int y;
    bool drawn;
};*/

/*FireBom fire_boms[10]; // fire_bom の最大個数
int nextFireBom = 0;   // 次に使う fire_bom のインデックス*/





// 画面サイズ
const int SCREEN_WIDTH = 1500;
const int SCREEN_HEIGHT = 875;
int enemy_spawn_time = 0;

// 敵の構造体
struct Enemy {
    int x, y;       // 位置
    int dx, dy;     // 移動速度
    bool active;    // 使用中かどうか
    Bmp* image;     // 画像
    int width;      //横幅
    int height;     //立幅
};

// 敵リスト
Enemy enemy[100];


Bmp* enemyImages[100] = {
    LoadBmp("pic/ikuzo_1.bmp"),
    LoadBmp("pic/ikuzo_1 - コピー.bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (2).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (3).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (4).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (5).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (6).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (7).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (8).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (9).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (10).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (11).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (12).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (13).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (14).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (15).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (16).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (17).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (18).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (19).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (20).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (21).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (22).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (23).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (24).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (25).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (26).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (27).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (28).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (29).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (30).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (31).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (32).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (33).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (34).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (35).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (36).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (37).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (38).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (39).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (40).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (41).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (42).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (43).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (44).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (45).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (46).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (47).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (48).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (49).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (50).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (51).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (52).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (53).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (54).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (55).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (56).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (57).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (58).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (59).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (60).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (61).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (62).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (63).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (64).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (65).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (66).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (67).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (68).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (69).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (70).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (71).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (72).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (73).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (74).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (75).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (76).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (77).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (78).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (79).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (80).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (81).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (82).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (83).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (84).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (85).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (86).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (87).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (88).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (89).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (90).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (91).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (92).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (93).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (94).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (95).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (96).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (97).bmp"),
    LoadBmp("pic/ikuzo_1 - コピー (98).bmp") 
};


Bmp* title = { LoadBmp("pic/title.bmp") };
Bmp* result = { LoadBmp("pic/result.bmp") };
Bmp* gameover = { LoadBmp("pic/gameover.bmp") };
Bmp* clear = { LoadBmp("pic/clear.bmp") };


// グローバルまたは main() 内の static 変数として保持
bool fireVisible = false;
int fireIndex = 0;  // どの fire 画像を表示するか
int fireTargetIndex = 0;  // どの座標に表示するか
int usedFireIndex = -1;     // どのfire画像が使われたか記録する
bool drawn[6] = { false, false, false, false, false, false }; // 初期状態は未描画


int walterIndex = 0;  // どの walter 画像を表示するか
int walterTargetIndex = 0;  // どの座標に表示するか
int usedwalterIndex = -1;     // どのwalter画像が使われたか記録する

int whiteIndex = 0;  // どの white 画像を表示するか
int whiteTargetIndex = 0;  // どの座標に表示するか
int usedwhiteIndex = -1;     // どのwhite画像が使われたか記録する

int greenIndex = 0;  // どの green 画像を表示するか
int greenTargetIndex = 0;  // どの座標に表示するか
int usedgreenIndex = -1;     // どのgreen画像が使われたか記録する


// ペアが揃った回数をカウント
int pairCount = 0;

int flippedCards[2] = { -1,-1 };//現在めくられている2枚のカードの院デックス

// 選択状態を管理するフラグ
bool isFlipped[8] = { false };

bool checkingPair = false;//ペアチェック中のフラグ

bool imagesRevealed = false; //画像が公開中かどうか

// エンターキーが押された回数を記録
bool enterPressed = false;

int shuffledIndices[8] = { 0,1,2,3,4,5,6,7 };//元のインデックス


//deck目的地の座標
int deck_targetX[6] = { 30,160,290,30,160,290 };
int deck_targetY[6] = { 550,550,550,680,680,680 };



//deck画像
Bmp* deck_walter[6] = { NULL };
Bmp* deck_green[6] = { NULL };
Bmp* deck_fire[6] = { NULL };
Bmp* deck_white[6] = { NULL };

Bmp* images[8] = { NULL };
Bmp* backs[8] = { NULL };
Bmp* originalImages[8] = { NULL };


Bmp* score9_1 = NULL;
Bmp* score8_1 = NULL;
Bmp* score7_1 = NULL;
Bmp* score6_1 = NULL;
Bmp* score5_1 = NULL;
Bmp* score4_1 = NULL;
Bmp* score3_1 = NULL;
Bmp* score2_1 = NULL;
Bmp* score1_1 = NULL;
Bmp* score0_1 = NULL;


// 0~9の画像を5セット用意
Bmp* scoreSet[5][10] = {
LoadBmp("pic/1_0.bmp"), LoadBmp("pic/1_1.bmp"), LoadBmp("pic/1_2.bmp"), LoadBmp("pic/1_3.bmp"), LoadBmp("pic/1_4.bmp"),
      LoadBmp("pic/1_5.bmp"), LoadBmp("pic/1_6.bmp"), LoadBmp("pic/1_7.bmp"), LoadBmp("pic/1_8.bmp"), LoadBmp("pic/1_9.bmp"),

    { LoadBmp("pic/2_0.bmp"), LoadBmp("pic/2_1.bmp"), LoadBmp("pic/2_2.bmp"), LoadBmp("pic/2_3.bmp"), LoadBmp("pic/2_4.bmp"),
      LoadBmp("pic/2_5.bmp"), LoadBmp("pic/2_6.bmp"), LoadBmp("pic/2_7.bmp"), LoadBmp("pic/2_8.bmp"), LoadBmp("pic/2_9.bmp") },

    { LoadBmp("pic/3_0.bmp"), LoadBmp("pic/3_1.bmp"), LoadBmp("pic/3_2.bmp"), LoadBmp("pic/3_3.bmp"), LoadBmp("pic/3_4.bmp"),
      LoadBmp("pic/3_5.bmp"), LoadBmp("pic/3_6.bmp"), LoadBmp("pic/3_7.bmp"), LoadBmp("pic/3_8.bmp"), LoadBmp("pic/3_9.bmp") },

    { LoadBmp("pic/4_0.bmp"), LoadBmp("pic/4_1.bmp"), LoadBmp("pic/4_2.bmp"), LoadBmp("pic/4_3.bmp"), LoadBmp("pic/4_4.bmp"),
      LoadBmp("pic/4_5.bmp"), LoadBmp("pic/4_6.bmp"), LoadBmp("pic/4_7.bmp"), LoadBmp("pic/4_8.bmp"), LoadBmp("pic/4_9.bmp") },

    { LoadBmp("pic/5_0.bmp"), LoadBmp("pic/5_1.bmp"), LoadBmp("pic/5_2.bmp"), LoadBmp("pic/5_3.bmp"), LoadBmp("pic/5_4.bmp"),
      LoadBmp("pic/5_5.bmp"), LoadBmp("pic/5_6.bmp"), LoadBmp("pic/5_7.bmp"), LoadBmp("pic/5_8.bmp"), LoadBmp("pic/5_9.bmp") }

};



// images の位置をランダム化する関数
auto shuffleImages = [&]() {
    random_device rd;
    mt19937 g(rd());



    //画像のインデックスをシャッフル
    shuffle(shuffledIndices, shuffledIndices + 8, g);


    //シャッフル後の順番でimagesを並び変え
    Bmp* tempImages[8];
    Bmp* tempBacks[8];

    for (int i = 0; i < 8; i++)
    {
        tempImages[i] = images[shuffledIndices[i]];
        tempBacks[i] = backs[shuffledIndices[i]];
    }

    for (int i = 0; i < 8; i++) {
        images[i] = tempImages[i];
        backs[i] = tempBacks[i];
    }
    };

Bmp* white_bomb = NULL;
Bmp* walter_bomb = NULL;
Bmp* fire_bomb = NULL;
Bmp* green_bomb = NULL;

void UpdateSlide();

//wのトリガー設定
int w_t = 0;
int w_n = 0;
int w_o = 0;

//aのトリガー設定
int a_t = 0;
int a_n = 0;
int a_o = 0;

//dのトリガー設定
int d_t = 0;
int d_n = 0;
int d_o = 0;

//sのトリガー設定
int s_t = 0;
int s_n = 0;
int s_o = 0;


int main() {
    startTime = time(NULL); // ゲーム開始時間を記録
    int lastSpawnTime = startTime; // 最後にスポーンした時間を記録

    // マウスのトリガーを設定
    int mouse_t = 0;
    int mouse_n = 0;
    int mouse_o = 0;

    //右クリックのトリガー設定
    int mouse_r_t = 0;
    int mouse_r_n = 0;
    int mouse_r_o = 0;

    //エンターのトリガー設定
    int enter_t = 0;
    int enter_n = 0;
    int enter_o = 0;

    //Rのトリガー設定
    int r_t = 0;
    int r_n = 0;
    int r_o = 0;



    time_t flipStartTime = 0;//2枚めくられた時間を記録



    time_t revealStartTime = 0; //画像が公開される時間の記録

    // 乱数の種を設定
    srand(time(NULL));

    // 初期画面設定
    InitConioEx(1500, 875, 1, 1, true);


    // 画像データ
    Bmp* table = NULL;
    Bmp* mouse = NULL;
    Bmp* club = NULL;
    Bmp* diamond = NULL;
    Bmp* heart = NULL;
    Bmp* spade = NULL;
    Bmp* deck[6] = { NULL };

    white_bomb = LoadBmp("pic/white_bomb.bmp");
    green_bomb = LoadBmp("pic/green_bomb.bmp");
    fire_bomb = LoadBmp("pic/fire_bomb.bmp");
    walter_bomb = LoadBmp("pic/walter_bomb.bmp");

    club = LoadBmp("pic/trump_club_L.bmp");
    diamond = LoadBmp("pic/trump_diamond_L.bmp");
    heart = LoadBmp("pic/trump_heart_L.bmp");
    spade = LoadBmp("pic/trump_spade_L.bmp");

    // 画像を読み込む
    table = LoadBmp("pic/table.bmp");

    // マウスの画像を読み込む
    mouse = LoadBmp("pic/mause.bmp");

    // images（カードの表）を読み込む
    images[0] = LoadBmp("pic/card_fire_1.bmp");
    images[1] = LoadBmp("pic/card_green_1.bmp");
    images[2] = LoadBmp("pic/card_walter_1.bmp");
    images[3] = LoadBmp("pic/card_white_1.bmp");
    images[4] = LoadBmp("pic/card_fire_2.bmp");
    images[5] = LoadBmp("pic/card_green_2.bmp");
    images[6] = LoadBmp("pic/card_walter_2.bmp");
    images[7] = LoadBmp("pic/card_white_2.bmp");


    // backs（カードの裏）を読み込む
    backs[0] = LoadBmp("pic/back_1.bmp");
    backs[1] = LoadBmp("pic/back_2.bmp");
    backs[2] = LoadBmp("pic/back_3.bmp");
    backs[3] = LoadBmp("pic/back_4.bmp");
    backs[4] = LoadBmp("pic/back_5.bmp");
    backs[5] = LoadBmp("pic/back_6.bmp");
    backs[6] = LoadBmp("pic/back_7.bmp");
    backs[7] = LoadBmp("pic/back_8.bmp");

    //deck画像
    deck[0] = LoadBmp("pic/deck_1.bmp");
    deck[1] = LoadBmp("pic/deck_2.bmp");
    deck[2] = LoadBmp("pic/deck_3.bmp");
    deck[3] = LoadBmp("pic/deck_4.bmp");
    deck[4] = LoadBmp("pic/deck_5.bmp");
    deck[5] = LoadBmp("pic/deck_6.bmp");



    //walterのデッキ用画像を読み込む
    deck_walter[0] = LoadBmp("pic/walter_1.bmp");
    deck_walter[1] = LoadBmp("pic/walter_2.bmp");
    deck_walter[2] = LoadBmp("pic/walter_1.bmp");
    deck_walter[3] = LoadBmp("pic/walter_1.bmp");
    deck_walter[4] = LoadBmp("pic/walter_1.bmp");
    deck_walter[5] = LoadBmp("pic/walter_1.bmp");


    //fireのデッキ画像を読み込む
    deck_fire[0] = LoadBmp("pic/fire_1.bmp");
    deck_fire[1] = LoadBmp("pic/fire_2.bmp");
    deck_fire[2] = LoadBmp("pic/fire_3.bmp");
    deck_fire[3] = LoadBmp("pic/fire_4.bmp");
    deck_fire[4] = LoadBmp("pic/fire_5.bmp");
    deck_fire[5] = LoadBmp("pic/fire_6.bmp");

    //greenのデッキ画像を読み込む
    deck_green[0] = LoadBmp("pic/green_1.bmp");
    deck_green[1] = LoadBmp("pic/green_2.bmp");
    deck_green[2] = LoadBmp("pic/green_3.bmp");
    deck_green[3] = LoadBmp("pic/green_4.bmp");
    deck_green[4] = LoadBmp("pic/green_5.bmp");
    deck_green[5] = LoadBmp("pic/green_6.bmp");

    //whiteのデッキ画像を読み込む
    deck_white[0] = LoadBmp("pic/white_1.bmp");
    deck_white[1] = LoadBmp("pic/white_2.bmp");
    deck_white[2] = LoadBmp("pic/white_3.bmp");
    deck_white[3] = LoadBmp("pic/white_4.bmp");
    deck_white[4] = LoadBmp("pic/white_5.bmp");
    deck_white[5] = LoadBmp("pic/white_6.bmp");

    //音声データ
    game_bgm = MciOpenSound("sound/bgm/game_music.mp3");
    title_bgm = MciOpenSound("sound/bgm/title_music.mp3");
    win_bgm = MciOpenSound("sound/bgm/win_music.mp3");
    rose = MciOpenSound("sound/se/rose.mp3");
    bom_set = MciOpenSound("sound/se/bom_set.mp3");
    win = MciOpenSound("sound/se/win.mp3");
    bom = MciOpenSound("sound/se/bom.mp3");



    

    // 色を設定
    SetPalette(table);
    SetPalette(mouse);
    SetPalette(scoreSet[0][1]);
    SetPalette(club);
    for (int i = 0; i < 8; i++) {
        SetPalette(images[i]);
        SetPalette(backs[i]);
    }

    // 目的地の座標
    int targetX[8] = { 880, 1010, 1140, 1270, 880, 1010, 1140, 1270 };
    int targetY[8] = { 680, 680, 680, 680, 550, 550, 550, 550 };
    


    // 現在の座標（初期位置を中央上部に設定）
    int currentX[8], currentY[8];
    for (int i = 0; i < 8; i++) {
        currentX[i] = 750; // 画面中央
        currentY[i] = 100; // 上部
    }

    bool isPairedMoved[4] = { false }; //ペアが移動されたか動かを管理


    // backs を初回移動（開始時にカードの裏が目的地へ移動）
    bool isMoving = true; // 最初から移動を開始


    int elapsedTime = 0;

    // 全てのカードが目的地に到達したら移動終了
    bool allArrived = true;

    initializeGame();
    //imagesを初期状態に戻す（シャッフル前の状態）
    for (int i = 0; i < 8; i++) {
        images[i] = originalImages[i];
    }
    MciPlaySound(title_bgm, true);

    // ゲームループ
    while (InputKeyMouse(PK_ESC) == 0) {
        switch (GameScene) {
        case 0:
            ClearScreen();
            DrawBmp(1, 1, title, true);
            if (InputKeyMouse(PK_ENTER) == 1)
            {
                //MciStopSound(title_bgm);
                //MciPlaySound(game_bgm, true);
                    // **タイマーリセット**
                startTime = time(NULL);
                GameScene = 1;
            }
            break;
        case 1:
            change_speed = 0;


            //リロード初期化
            r_o = r_n;

            w_o = w_n;
            a_o = a_n;
            s_o = s_n;
            d_o = d_n;

            //スライディング初期化
            c_o = c_n;
            


            //MciPlaySound(game_bgm, true);
            mouse_o = mouse_t;
            mouse_r_o = mouse_r_n;


            //マウストリガー初期化


            //エンタートリガー初期化
            enter_o = enter_n;


            // スペースキーが押されたらゲームをリセット
            if (InputKeyMouse(PK_R) == 1) {
                //shuffleImages();//一度だけシャッフ
                //resetGame();  // ゲームをリセットする関数を呼び出す
                r_n = 1;
          

            }
            else
            {
                r_n = 0;
            }


            if (r_n == 1&&r_o == 0)
            {
                r_t = 1;
            }
            else
            {
                r_t = 0;
            }
            if (r_n == 1 && r_t == 1 && r_o == 0)
            {
                shuffleImages();//一度だけシャッフ
                resetGame();  // ゲームをリセットする関数を呼び出す
            }


            // 背景を描画
            DrawBmp(1, 1, table, true);
            DrawBmp(30, 550, deck[0], true);
            DrawBmp(160, 550, deck[1], true);
            DrawBmp(290, 550, deck[2], true);
            DrawBmp(30, 680, deck[3], true);
            DrawBmp(160, 680, deck[4], true);
            DrawBmp(290, 680, deck[5], true);


            // 毎フレームで秒数を計算して描画
            elapsedTime = time(NULL) - startTime;
            clearTime = elapsedTime;
            drawNumber(elapsedTime, 1, 1); // 画面の適当な位置に描画



          


           /* //スライディングトリガー
            if (InputKeyMouse(PK_SHIFT) == 1)
            {
                c_n = 1;
            }
            else
            {
                c_n = 0;
            }


            if (c_n == 1&&c_o == 0)
            {
                c_t = 1;
            }
            else
            {
                c_t = 0;
            }


            if (c_n == 1 && c_t == 1 && c_o == 0)
            {
                slide_change = 1;
            }
            if (slide_change == 1)
            {
                normal_speed = 15;
                slide++;
            }
            else
            {
                normal_speed = 5;
                slide = 0;
            }
            if (slide == 10)
            {
                slide_change = 0;
            }*/

           /* // スライディングトリガー
            if (InputKeyMouse(PK_SHIFT) == 1)
            {
                c_n = 1;
            }
            else
            {
                c_n = 0;
            }

            if (c_n == 1 && c_o == 0)
            {
                c_t = 1;
            }
            else
            {
                c_t = 0;
            }

            if (c_n == 1 && c_t == 1 && c_o == 0)
            {
                slide_change = 1;
                slide = 0;  // スライドカウンターのリセット
            }
            slide++;
            if (slide > 10)
            {
                slide_change = 0;
            }
            ChangeSpeed();

            /*if (slide_change == 1)
            {
                normal_speed = 15;
                slide++;



                if (slide > 10)
                {
                    slide_change = 0;
                    normal_speed = 5; // スライディング後のスピードをリセット

                }
            }*/

            gameLoop();


            // 画像を滑らかに移動
            for (int i = 0; i < 8; i++) {
                if (isMoving) {
                    // 目的地へ徐々に近づける（イージング効果）
                    currentX[i] += (targetX[i] - currentX[i]) / 5;
                    currentY[i] += (targetY[i] - currentY[i]) / 5;
                    // ほぼ目的地に到達したらピタッと合わせる
                    if (abs(targetX[i] - currentX[i]) < 2 && abs(targetY[i] - currentY[i]) < 2) {
                        currentX[i] = targetX[i];
                        currentY[i] = targetY[i];
                    }
                }
                //カードの表示（裏or表）
                if (isFlipped[i]) {
                    DrawBmp(currentX[i], currentY[i], images[i], true);
                }
                else {
                    DrawBmp(currentX[i], currentY[i], backs[i], true);
                }
            }

            for (int i = 0; i < 8; i++) {
                if (currentX[i] != targetX[i] || currentY[i] != targetY[i]) {
                    allArrived = false;
                    break;
                }
            }
            if (allArrived) {
                isMoving = false;
            }

            // enterキーが押されたら1回だけシャッフルして表を表示
            if (InputKeyMouse(PK_SP) == 1 && !enterPressed) {
                enterPressed = true; // 2回目以降のシャッフルを防ぐ
                imagesRevealed = true;
                revealStartTime = time(NULL);
                shuffleImages(); //カードの並びをシャッフル
                for (int i = 0; i < 8; i++) {
                    isFlipped[i] = true;
                }
            }

            if (enter_n == 1&&enter_o==0) {
                enter_t = 1;
            }

            //5秒経過したらbacksに戻す
            if (imagesRevealed && time(NULL) - revealStartTime >= 5) {
                for (int i = 0; i < 8; i++) {
                    isFlipped[i] = false;
                }
                imagesRevealed = false;
            }


            if (InputKeyMouse(VK_RBUTTON) == 1) {
                mouse_r_n = 1;
            }
            else
            {
                mouse_r_n = 0;
            }
            if (mouse_r_n == 1&&mouse_r_o ==0) {
                mouse_r_t = 1;
            }
            else
            {
                mouse_r_t = 0;
            }
            // 🖱️ 右クリックが押されたら `onMouseDown()` を呼ぶ
            if (mouse_r_n == 1 && mouse_r_t == 0&&mouse_r_o ==1) {
                isRightClick = true;

                POINT mousePoint;
                GetCursorMousePos(&mousePoint);
                onMouseDown(mousePoint.x, mousePoint.y);
            }

            POINT mousePoint;
            GetCursorMousePos(&mousePoint); // 現在のカーソル位置を取得
            onMouseMove(mousePoint.x, mousePoint.y);

            //右クリックが話されたらonMouseUpを呼ぶ
            if (mouse_r_n == 0 && mouse_r_t == 0&&mouse_r_o == 1)
            {
                isRightClick = false;
                onMouseUp();

            }

            // マウスクリックでカードをめくる（エンターを押してからのみ有効）
            if (InputKeyMouse(VK_LBUTTON) == 1) {
                mouse_n = 1;
            }
            else
            {
                mouse_n = 0;
            }
            if (mouse_n == 1&&mouse_o == 0) {
                mouse_t = 1;
            }
            else
            {
                mouse_t = 0;
            }
            if (mouse_n == 1 && mouse_t == 1 && mouse_o == 0 && enterPressed && !checkingPair) {
                POINT mousePoint;
                GetCursorMousePos(&mousePoint);
                int mouseX = mousePoint.x;
                int mouseY = mousePoint.y;

                for (int i = 0; i < 8; i++) {
                    if (mouseX >= currentX[i] && mouseX <= currentX[i] + 110 &&
                        mouseY >= currentY[i] && mouseY <= currentY[i] + 110) {

                        if (!isFlipped[i]) {
                            isFlipped[i] = true;

                            //めくられたカードを記録**
                            if (flippedCards[0] == -1)
                            {
                                flippedCards[0] = i;
                            }
                            else if (flippedCards[1] == -1)
                            {
                                flippedCards[1] = i;
                                checkingPair = true;//2枚めくったのでチェック開始
                                flipStartTime = time(NULL);//めくった時間を記録
                            }
                            break;
                        }
                    }
                }
            }


            if (checkingPair && time(NULL) - flipStartTime >= 1) {
                int idx1 = flippedCards[0];
                int idx2 = flippedCards[1];

                //**シャッフル後のインデックスを参照**
                int originalIdx1 = shuffledIndices[idx1];
                int originalIdx2 = shuffledIndices[idx2];


                //**ペアなら移動**
                if ((originalIdx1 == 0 && originalIdx2 == 4) || (originalIdx1 == 4 && originalIdx2 == 0))
                {
                    chooseFireImage();
                }
                else if ((originalIdx1 == 1 && originalIdx2 == 5) || (originalIdx1 == 5 && originalIdx2 == 1))
                {
                    chooseGreenImage();
                }
                else if ((originalIdx1 == 2 && originalIdx2 == 6) || (originalIdx1 == 6 && originalIdx2 == 2))
                {
                    chooseWalterImage();
                }
                else if ((originalIdx1 == 3 && originalIdx2 == 7) || (originalIdx1 == 7 && originalIdx2 == 3))
                {
                    chooseWhiteImage();
                }
                else
                {
                    //**ペアじゃない場合、裏返す**
                    isFlipped[idx1] = false;
                    isFlipped[idx2] = false;
                }

                //**判定が終わったらリセット**
                flippedCards[0] = -1;
                flippedCards[1] = -1;
                checkingPair = false;
            }

            enemy_spawn_time++;

            // ここに追加！ (敵の処理)
            if (enemy_spawn_time == 100) {
                for (int i = 0; i < 10; i++)
                {
                    SpawnEnemy();
                }
                enemy_spawn_time = 0;
            }
            MoveEnemies();
            CheckPlayerCollision();
            CheckBomCollision();
            for (int i = 0; i < 100; i++) {
                if (enemy[i].active) {
                    DrawBmp(enemy[i].x, enemy[i].y, enemy[i].image, true);
                }
            }

            if (enemy_kill == 5)
            {
                MciStopSound(title_bgm);
                MciPlaySound(win_bgm, true);
                MciPlaySound(win, true);
                GameScene = 3;
            }

            drawImages();

            // マウスカーソルの座標取得
            GetCursorMousePos(&mousePoint); // カーソル座標を取得
            cursorX = mousePoint.x;
            cursorY = mousePoint.y;

            // カスタムマウスカーソルを描画
            DrawBmp(cursorX, cursorY, mouse, true);
            break;


            case 2:
                ClearScreen();
                DrawBmp(1, 1, gameover, true);
                reset();
                enemy_kill;
                if (InputKeyMouse(PK_ENTER) == 1)
                {
                    //MciPlaySound(game_bgm, true);
                    MciPlaySound(title_bgm, true);
                    enemy_kill = 0;
                    GameScene = 1;
                }
                break;
            case 3:
                ClearScreen();
                DrawBmp(1, 1, clear, true);
                // **クリアタイムを表示**
                drawNumber(clearTime, 500, 350);  // クリアタイムを数値として描画
                if (InputKeyMouse(PK_O) == 1)
                {
                    reset();
                    enemy_kill = 0;
                    MciStopSound(win);
                    MciStopSound(win_bgm);
                    //MciPlaySound(game_bgm, true);
                    MciPlaySound(title_bgm, true);
                    GameScene = 0;
                }
                break;
        }


        

        // 画面更新
        PrintFrameBuffer();
        FlipScreen();
        Sleep(16);
    }
    return 0;
}
/*
int* game_bgm = 0;
int* title_bgm = 0;
int* rose = 0;
int* bom_set = 0;
int* win = 0;
int* win_bgm = 0;*/

void drawNumber(int number, int x, int y) {
    int digits[5] = { 0, 0, 0, 0, 0 }; // 最大5桁
    int numDigits = 0;

    // **数字を1桁ずつ分解**
    int temp = number;
    do {
        digits[numDigits++] = temp % 10;
        temp /= 10;
    } while (temp > 0);

    // **数字を描画（各桁ごとに異なる画像セットを使う）**
    for (int i = 0; i < numDigits; i++) {
        DrawBmp(x + (i * 50), y, scoreSet[i][digits[numDigits - 1 - i]], true);
    }
}


// **ゲームをリセットする関数**
void resetGame() {

    //imagesを初期状態に戻す（シャッフル前の状態）
    for (int i = 0; i < 8; i++) {
        images[i] = originalImages[i];
    }

    enterPressed = false;  // Enterキーのフラグをリセット
    imagesRevealed = false;  // 画像が表向きになっている状態をリセット
    checkingPair = false;  // ペアチェックの状態をリセット
    flippedCards[0] = -1;
    flippedCards[1] = -1;
    pairCount = 0;  // ペアカウントをリセット

    for (int i = 0; i < 8; i++) {
        isFlipped[i] = false;  // すべてのカードを裏向きにする
    }
}

void initializeGame() {
    images[0] = LoadBmp("pic/card_fire_1.bmp");
    images[1] = LoadBmp("pic/card_green_1.bmp");
    images[2] = LoadBmp("pic/card_walter_1.bmp");
    images[3] = LoadBmp("pic/card_white_1.bmp");
    images[4] = LoadBmp("pic/card_fire_2.bmp");
    images[5] = LoadBmp("pic/card_green_2.bmp");
    images[6] = LoadBmp("pic/card_walter_2.bmp");
    images[7] = LoadBmp("pic/card_white_2.bmp");

    //正しい順序をoriginalImagesにコピー
    for (int i = 0; i < 8; i++) {
        originalImages[i] = images[i];
    }
}

void chooseImage(int type, std::set<int>& usedIndices, bool isUsed[], int& currentIndex, int& selectedIndex) {
    std::vector<int> emptySlots;

    // **未使用のスロットをリストアップ**
    for (int i = 0; i < 6; i++) {
        if (!drawn[i]) { // **描画されていないスロットを探す**
            emptySlots.push_back(i);
        }
    }

    // **未使用スロットがある場合は、ランダムに選択**
    if (!emptySlots.empty()) {
        int newTargetIndex = emptySlots[rand() % emptySlots.size()];

        // **画像をセット**
        selectedIndex = newTargetIndex;
        usedIndices.insert(selectedIndex);
        drawn[newTargetIndex] = true;
        imageType[newTargetIndex] = static_cast<ImageType>(type);

        // **正しい位置に描画**
        deck_targetX[newTargetIndex] = deck_targetX[newTargetIndex];
        deck_targetY[newTargetIndex] = deck_targetY[newTargetIndex];

        // **次のインデックスへ**
        currentIndex = (currentIndex + 1) % 6;
    }
}

void chooseFireImage() {
    static int fireIndex = 0;
    chooseImage(FIRE, usedFireIndices, isUsedFire, fireIndex, fireIndex);
}

void chooseWalterImage() {
    static int walterIndex = 0;
    chooseImage(WALTER, usedWalterIndices, isUsedWalter, walterIndex, walterIndex);
}

void chooseGreenImage() {
    static int greenIndex = 0;
    chooseImage(green, usedGreenIndices, isUsedGreen, greenIndex, greenIndex);
}

void chooseWhiteImage() {
    static int whiteIndex = 0;
    chooseImage(white, usedWhiteIndices, isUsedWhite, whiteIndex, whiteIndex);
}




// **爆弾をスポーン（生成）する関数**
void spawn_bom(int x, int y, ImageType type) {
    Bom* bomArray;

    // **爆弾の種類を判別**
    if (type == FIRE) {
        bomArray = fire_boms;
    }
    else if (type == WALTER) {
        bomArray = walter_boms;
    }
    else if (type == green) {
        bomArray = green_boms;
    }
    else if (type == white) {
        bomArray = white_boms;
    }
    else {
        return; // NONE なら何もしない
    }

    // **未使用（drawn == false）のスロットを探す**
    for (int i = 0; i < MAX_BOM; i++) {
        if (!bomArray[i].drawn) {
            // **爆弾を設定**
            bomArray[i] = { x, y, true };
            return;
        }
    }
}

// **画像を描画する関数**
void drawImages() {
    for (int i = 0; i < 6; i++) {
        if (drawn[i]) {
            int drawX = deck_targetX[i];
            int drawY = deck_targetY[i];

            // **ドラッグ中の画像は一時変数の座標で描画**
            if (isDragging && draggingIndex == i) {
                drawX = tempX;
                drawY = tempY;
            }

            switch (imageType[i]) {
            case FIRE:
                DrawBmp(drawX, drawY, deck_fire[i], true);
                break;
            case WALTER:
                DrawBmp(drawX, drawY, deck_walter[i], true);
                break;
            case green:
                DrawBmp(drawX, drawY, deck_green[i], true);
                break;
            case white:
                DrawBmp(drawX, drawY, deck_white[i], true);
                break;
            }
        }
    }

    // **爆弾を描画**
    for (int i = 0; i < MAX_BOM; i++) {
        if (fire_boms[i].drawn) {
            DrawBmp(fire_boms[i].x, fire_boms[i].y, fire_bom[i], true);
        }
        if (walter_boms[i].drawn) {
            DrawBmp(walter_boms[i].x, walter_boms[i].y, walter_bom[i], true);
        }
        if (green_boms[i].drawn) {
            DrawBmp(green_boms[i].x, green_boms[i].y, green_bom[i], true);
        }
        if (white_boms[i].drawn) {
            DrawBmp(white_boms[i].x, white_boms[i].y, white_bom[i], true);
        }
    }
}




// 🖱️ マウスが押された時（右クリック）
//void onMouseDown(int mouseX, int mouseY) {
//
//    for (int i = 0; i < 6; i++) {
//        if (drawn[i]) { // **描画されている画像だけチェック**
//            int imgX = deck_targetX[i];
//            int imgY = deck_targetY[i];
//
//            // **クリック位置が画像の範囲内ならドラッグ開始**
//            if (mouseX >= imgX && mouseX <= imgX + 110 &&
//                mouseY >= imgY && mouseY <= imgY + 110) {
//
//                isDragging = true;
//                draggingIndex = i;
//
//                // **クリックした時点の座標を一時変数に保存**
//                tempX = imgX;
//                tempY = imgY;
//
//                // **オフセット計算**
//                offsetX = mouseX - imgX;
//                offsetY = mouseY - imgY;
//                break;
//            }
//        }
//    }
//}

void onMouseDown(int mouseX, int mouseY) {
    // マウスが押された位置に基づいてドラッグ開始
    for (int i = 0; i < 6; i++) {
        if (drawn[i]) {
            int imgX = deck_targetX[i];
            int imgY = deck_targetY[i];

            // 画像の範囲内でクリックされたらドラッグを開始
            if (mouseX >= imgX && mouseX <= imgX + 110 &&
                mouseY >= imgY && mouseY <= imgY + 110) {

                isDragging = true;
                draggingIndex = i;

                // クリックした位置を一時的な座標として保存
                tempX = mouseX;
                tempY = mouseY;

                // ドラッグ開始時のオフセットを計算
                offsetX = mouseX - imgX;
                offsetY = mouseY - imgY;

                break;
            }
        }
    }
}

// 🖱️ マウスが動いた時
void onMouseMove(int mouseX, int mouseY) {
    if (isDragging && draggingIndex != -1) {
        // **一時変数を更新（本来の座標には影響を与えない）**
        tempX = mouseX - offsetX;
        tempY = mouseY - offsetY;
    }
}




// 🖱️ マウスが離された時（右クリック解除で画像を削除）
//void onMouseUp() {
//
//    if (draggingIndex != -1) {
//
//        int finalX = tempX;
//        int finalY = tempY;
//
//        // **種類ごとに爆弾を生成**
//        if (imageType[draggingIndex] != IMG_NONE) {
//            spawn_bom(finalX, finalY, imageType[draggingIndex]);
//        }
//        MciPlaySound(bom_set, false);
//
//        drawn[draggingIndex] = false;
//        isUsedTarget[draggingIndex] = false;
//
//        if (imageType[draggingIndex] == FIRE) {
//            isUsedFire[draggingIndex] = false;
//        }
//        else if (imageType[draggingIndex] == WALTER) {
//            isUsedWalter[draggingIndex] = false;
//        }
//        else if (imageType[draggingIndex] == green) {
//            isUsedGreen[draggingIndex] = false;
//        }
//        else if (imageType[draggingIndex] == white) {
//            isUsedWhite[draggingIndex] = false;
//        }
//
//        // **削除したスロットを再利用リストに追加**
//        usedTargetIndices.insert(draggingIndex);
//
//        imageType[draggingIndex] = IMG_NONE;
//        isDragging = false;
//        draggingIndex = -1;
//
//        // **一時変数をリセット**
//        tempX = -1;
//        tempY = -1;
//    }
//}

void onMouseUp() {
    if (draggingIndex != -1) {

        int finalX = tempX;
        int finalY = tempY;

        // **種類ごとに爆弾を生成**
        if (imageType[draggingIndex] != IMG_NONE) {
            spawn_bom(finalX, finalY, imageType[draggingIndex]);
        }
        MciPlaySound(bom_set, false);

        drawn[draggingIndex] = false;
        isUsedTarget[draggingIndex] = false;

        if (imageType[draggingIndex] == FIRE) {
            isUsedFire[draggingIndex] = false;
        }
        else if (imageType[draggingIndex] == WALTER) {
            isUsedWalter[draggingIndex] = false;
        }
        else if (imageType[draggingIndex] == green) {
            isUsedGreen[draggingIndex] = false;
        }
        else if (imageType[draggingIndex] == white) {
            isUsedWhite[draggingIndex] = false;
        }

        // **削除したスロットを再利用リストに追加**
        usedTargetIndices.insert(draggingIndex);

        imageType[draggingIndex] = IMG_NONE;
        isDragging = false;
        draggingIndex = -1;

        // **一時変数をリセット**
        tempX = -1;
        tempY = -1;
    }
}

// 左移動
void kaede_left_move() {
    


       // player_x -= normal_speed; // 左に移動
    isFacingLeft = true;
    isFacingRight = isFacingUp = isFacingDown = false;
    currentImage = kaede_left[0];
    ani_cnt++;
    if (ani_data[ani_cnt] == 0xff)
    {
        ani_cnt = 0;
    }
}

// 右移動
void kaede_right_move() {
    
     //   player_x += normal_speed; // 左に移
    isFacingRight = true;
    isFacingLeft = isFacingUp = isFacingDown = false;
    currentImage = kaede_right[0];
    ani_cnt++;
    if (ani_data[ani_cnt] == 0xff)
    {
        ani_cnt = 0;
    }
}

// 上移動
void kaede_back_move() {

      //  player_y -= normal_speed; // 左に移動

    isFacingUp = true;
    isFacingLeft = isFacingRight = isFacingDown = false;
    currentImage = kaede_back[0];
    ani_cnt++;
    if (ani_data[ani_cnt] == 0xff)
    {
        ani_cnt = 0;
    }
}

// 下移動
void kaede_front_move() {
    /*/if (InputKeyMouse(PK_C) == 1)
    {

    }*/
       // player_y += normal_speed; // 左に移動
    isFacingDown = true;
    isFacingLeft = isFacingRight = isFacingUp = false;
    currentImage = kaede_front[0];
    ani_cnt++;
    if (ani_data[ani_cnt] == 0xff)
    {
        ani_cnt = 0;
    }
}



// キャラクターの向きを保持する列挙型を追加（オプションで可読性を向上）
enum FacingDirection { LEFT, RIGHT, UP, DOWN, NONE };
FacingDirection lastFacingDirection = NONE;  // 最後の向きを保持

// 残留移動防止用の明示的速度ベクトル
static int velX = 0; // -1,0,1
static int velY = 0; // -1,0,1

// キー入力による移動処理（残留対策版）
void updateCharacterMovement() {
    int curA = (InputKeyMouse(PK_A) == 1);
    int curD = (InputKeyMouse(PK_D) == 1);
    int curW = (InputKeyMouse(PK_W) == 1);
    int curS = (InputKeyMouse(PK_S) == 1);
    int curShift = (InputKeyMouse(PK_SHIFT) == 1);

    static int prevA = 0, prevD = 0, prevW = 0, prevS = 0, prevShift = 0;

    // スライド中はプレイヤー入力による方向変更を受け付けない。
    if (slidingActive) {
        // スライド中は保存された方向で移動を続ける
        velX = slideDirX;
        velY = slideDirY;

        // アニメーション向きをスライド方向に合わせる
        if (slideDirX < 0) { lastFacingDirection = LEFT; kaede_left_move(); }
        else if (slideDirX > 0) { lastFacingDirection = RIGHT; kaede_right_move(); }
        else if (slideDirY < 0) { lastFacingDirection = UP; kaede_back_move(); }
        else if (slideDirY > 0) { lastFacingDirection = DOWN; kaede_front_move(); }

    } else {
        // 水平軸更新
        if (curA && curD) {
            velX = 0; // 同時押しは停止
        } else if (curA) {
            velX = -2;
            lastFacingDirection = LEFT;
            kaede_left_move();
        } else if (curD) {
            velX = 2;
            lastFacingDirection = RIGHT;
            kaede_right_move();
        } else {
            // どちらも押されていない & 前フレームで何か押されていた方向が残っているなら停止
            if ((prevA && !curA && velX < 0) || (prevD && !curD && velX > 0)) {
                velX = 0;
            }
        }

        // 垂直軸更新
        if (curW && curS) {
            velY = 0; // 同時押し停止
        } else if (curW) {
            velY = -2;
            lastFacingDirection = UP;
            kaede_back_move();
        } else if (curS) {
            velY = 2;
            lastFacingDirection = DOWN;
            kaede_front_move();
        } else {
            if ((prevW && !curW && velY < 0) || (prevS && !curS && velY > 0)) {
                velY = 0;
            }
        }

        // SHIFT解除時にフェイルセーフで速度をクリア（斜め＋スライド後の残留対策）
        if (prevShift && !curShift) {
            velX = 0;
            velY = 0;
        }
    }

    // 実際の移動（斜めは速度正規化）
    if (velX != 0 || velY != 0) {
        float moveX = (float)velX;
        float moveY = (float)velY;
        if (velX != 0 && velY != 0) {
            // 斜めは長さを1に正規化
            const float inv = 0.70710678f; // 1/sqrt(2)
            moveX *= inv;
            moveY *= inv;
        }
        player_x += (int)(moveX * normal_speed);
        player_y += (int)(moveY * normal_speed);
    } else {
        // 完全停止時はアニメカウンタと向きをニュートラルへ
        ani_cnt = 0;
        lastFacingDirection = NONE;
    }

    // 前フレーム状態更新
    prevA = curA; prevD = curD; prevW = curW; prevS = curS; prevShift = curShift;
}

// キャラクターを描画（常に描画し、最後の向きを使用）
void drawCharacter() {
    Bmp* imageToDraw = kaede_front[0];  // デフォルト画像（下向き）

    // 最後の向きに基づいて画像を選択
    switch (lastFacingDirection) {
        case LEFT:
            imageToDraw = kaede_left[ani_data[ani_cnt]];
            break;
        case RIGHT:
            imageToDraw = kaede_right[ani_data[ani_cnt]];
            break;
        case UP:
            imageToDraw = kaede_back[ani_data[ani_cnt]];
            break;
        case DOWN:
            imageToDraw = kaede_front[ani_data[ani_cnt]];
            break;
        default:
            // NONEの場合、デフォルトの正面を使用
            break;
    }

    // 常に描画
    DrawBmp(player_x, player_y, imageToDraw, true);
}

// 毎フレームの処理
void gameLoop() {
    updateCharacterMovement();  // 移動処理
    drawCharacter();            // 描画処理
}

void SpawnEnemy() {
    int enemyIndex = -1;

    // 非アクティブな敵を探して再利用
    for (int i = 0; i < 100; i++) {
        if (!enemy[i].active) {
            enemyIndex = i;
            break;
        }
    }

    // すべてのスロットが埋まっている場合は何もしない
    if (enemyIndex == -1) return;

    // 画面外のランダムな位置でスポーン
    int side = rand() % 4; // 0: 左, 1: 右, 2: 上, 3: 下
    int x, y, dx = 0, dy = 0,width,height;
    width = 128;
    height = 128;
    if (side == 0) {  // 左からスポーン
        x = -50;
        y = rand() % SCREEN_HEIGHT;
        dx = 10;
    }
    else if (side == 1) {  // 右からスポーン
        x = SCREEN_WIDTH + 50;
        y = rand() % SCREEN_HEIGHT;
        dx = 10;
    }
    else if (side == 2) {  // 上からスポーン
        x = rand() % SCREEN_WIDTH;
        y = -50;
        dy = 10;
    }
    else {  // 下からスポーン
        x = rand() % SCREEN_WIDTH;
        y = SCREEN_HEIGHT + 50;
        dy = 10;
    }
    // 敵を設定
    enemy[enemyIndex] = { x, y, dx, dy, true, enemyImages[enemyIndex % 100],width,height };
}

// 敵を動かす
void MoveEnemies() {
    for (int i = 0; i < 100; i++) {
        if (enemy[i].active) {
            enemy[i].x += enemy[i].dx;
            enemy[i].y += enemy[i].dy;

            // 画面外に出たら非アクティブ化
            if (enemy[i].x < -60 || enemy[i].x > SCREEN_WIDTH + 60 ||
                enemy[i].y < -60 || enemy[i].y > SCREEN_HEIGHT + 60) {
                enemy[i].active = false;
            }
        }
    }
}

void CheckPlayerCollision() {
    for (int i = 0; i < 100; i++) {
        if (enemy[i].active) {
            if (CheckCollision(player_x, player_y, playerWidth, playerHeight,
                enemy[i].x, enemy[i].y, enemy[i].width, enemy[i].height)) {
                // 衝突したらプレイヤーのライフを減らす or ゲームオーバー

                GameScene = 2;
                enemy[i].active = false;  // 敵を消す
               // MciStopSound(game_bgm);
                MciStopSound(title_bgm);
                MciPlaySound(rose, false);
            }
        }
    }
}


void CheckBomCollision() {
    for (int i = 0; i < 100; i++) {
        if (enemy[i].active) { // 敵が生きているなら
            for (int j = 0; j < MAX_BOM; j++) {
                if (fire_boms[j].drawn) { // **爆弾が存在しているなら**
                    if (CheckCollision(enemy[i].x, enemy[i].y, enemy[i].width, enemy[i].height,
                        fire_boms[j].x, fire_boms[j].y, 32, 32)) {
                        // **当たったら敵を消す**
                        DrawBmp(fire_boms[j].x, fire_boms[j].y, fire_bomb, true);
                        enemy[i].active = false;
                        fire_boms[j].drawn = false; // 爆弾も消える
                        enemy_kill += 1;
                        MciPlaySound(bom, false);
                    }
                }

                if (walter_boms[j].drawn) {
                    if (CheckCollision(enemy[i].x, enemy[i].y, enemy[i].width, enemy[i].height,
                        walter_boms[j].x, walter_boms[j].y, 32, 32)) {
                        DrawBmp(walter_boms[j].x, walter_boms[j].y, walter_bomb, true);
                        enemy[i].active = false;
                        walter_boms[j].drawn = false;
                        enemy_kill += 1;
                        MciPlaySound(bom, false);
                    }
                }

                if (green_boms[j].drawn) {
                    if (CheckCollision(enemy[i].x, enemy[i].y, enemy[i].width, enemy[i].height,
                        green_boms[j].x, green_boms[j].y, 32, 32)) {
                        DrawBmp(green_boms[j].x, green_boms[j].y, green_bomb, true);
                        enemy[i].active = false;
                        green_boms[j].drawn = false;
                        enemy_kill += 1;
                        MciPlaySound(bom, false);
                    }
                }

                if (white_boms[j].drawn) {
                    if (CheckCollision(enemy[i].x, enemy[i].y, enemy[i].width, enemy[i].height,
                        white_boms[j].x, white_boms[j].y, 32, 32)) {
                        DrawBmp(white_boms[j].x, white_boms[j].y, white_bomb, true);
                        enemy[i].active = false;
                        white_boms[j].drawn = false;
                        enemy_kill += 1;
                        MciPlaySound(bom, false);
                    }
                }
            }
        }
    }
}

void reset() {

   // プレイヤー位置
    player_x = 750;
    player_y = 300;

    // 入力状態リセット
    a_n = a_o = a_t = 0;
    d_n = d_o = d_t = 0;
    w_n = w_o = w_t = 0;
    s_n = s_o = s_t = 0;
    c_n = c_o = c_t = 0;

    // スライド状態リセット
    slide = 0;
    slide_change = 0;
    normal_speed = 5;

    // 向きもリセット
    isFacingLeft = isFacingRight = isFacingUp = isFacingDown = false;

    // **敵のリセット**
    for (int i = 0; i < 100; i++) {
        enemy[i].active = false;
    }

    // **爆弾のリセット**
    for (int i = 0; i < MAX_BOM; i++) {
        fire_boms[i].drawn = false;
        walter_boms[i].drawn = false;
        green_boms[i].drawn = false;
        white_boms[i].drawn = false;
    }

    // **カードの状態リセット**
    for (int i = 0; i < 8; i++) {
        isFlipped[i] = false;
    }

    // **デッキのリセット**
    for (int i = 0; i < 6; i++) {
        drawn[i] = false;         // 描画フラグをリセット
        imageType[i] = IMG_NONE;      // 画像の種類をリセット
    }

    // **ペア判定用の変数リセット**
    enterPressed = false;
    checkingPair = false;
    flippedCards[0] = -1;
    flippedCards[1] = -1;
    imagesRevealed = false;
    enemy_spawn_time = 0;
    //imagesを初期状態に戻す（シャッフル前の状態）
    for (int i = 0; i < 8; i++) {
        images[i] = originalImages[i];
    }

    for (int i = 0; i < 8; i++) {
        isFlipped[i] = false;  // すべてのカードを裏向きにする
    }

    // **タイマーリセット**
    startTime = time(NULL);

    // **デッキを再描画**
    drawImages();
}

// UpdateSlide: SHIFT を一定フレーム保持でスライド開始、スライド中は一定フレーム継続
void UpdateSlide()
{
    // 現在の SHIFT 押下状態
    int curShift = (InputKeyMouse(PK_SHIFT) == 1);

    if (!slidingActive) {
        // ホールドカウンタを増加 or リセット
        if (curShift) {
            slideHoldCounter++;
        } else {
            slideHoldCounter = 0;
        }

        // 一定時間押し続けたらスライド開始
        if (slideHoldCounter >= SLIDE_HOLD_FRAMES) {
            // 現在の方向を取得（直近の入力ベクトル）
            int dirX = velX;
            int dirY = velY;

            // 動いていないなら lastFacingDirection を使う
            if (dirX == 0 && dirY == 0) {
                switch (lastFacingDirection) {
                case LEFT: dirX = -1; dirY = 0; break;
                case RIGHT: dirX = 1; dirY = 0; break;
                case UP: dirX = 0; dirY = -1; break;
                case DOWN: dirX = 0; dirY = 1; break;
                default: break;
                }
            }

            // 方向が決まっていればスライド開始
            if (dirX != 0 || dirY != 0) {
                slidingActive = true;
                slideTimerGlobal = SLIDE_DURATION_FRAMES;
                slideDirX = dirX;
                slideDirY = dirY;
                // force velocity to slide direction
                velX = slideDirX;
                velY = slideDirY;
                slide_change = 1;
                // ホールドカウンタをリセットしておく
                slideHoldCounter = 0;
            } else {
                // 方向がない場合は開始しない
                slideHoldCounter = 0;
            }
        }
    } else {
        // スライド中のカウントダウン
        if (slideTimerGlobal > 0) {
            slideTimerGlobal--;
            // スライド中は方向を固定
            velX = slideDirX;
            velY = slideDirY;
            slide = SLIDE_DURATION_FRAMES - slideTimerGlobal; // 進行表示用
            if (slideTimerGlobal == 0) {
                slidingActive = false;
                slide_change = 0;
                // スライド終了時に速度をクリア
                velX = 0;
                velY = 0;
            }
        }
    }

    // スピードを適用
    ChangeSpeed();
}

// ChangeSpeed: set normal_speed depending on slide_change flag
void ChangeSpeed()
{
    if (slide_change == 1) {
        normal_speed = boost_speed; // スライド中は速くする
    }
    else {
        normal_speed = 5; // 通常速度に戻す
    }
}
