//全局变量(引用式声明)
extern void *g_plcd;						 //帧缓冲的地址
extern struct fb_var_screeninfo g_fbinfo;	 //屏幕信息（结构体）
extern int g_addressMine[15];			 //储存雷的地址（最多15个雷）
extern int g_numMine;						 //雷的数量（10~15个）
extern int g_numBlock[10][10];			 //所有方块的数字,特别的，雷标识为10。
extern int g_numPlayer[10][10];		 //玩家所标识的数字,特别的，雷标识为10。
extern int g_addressAttributes[10][10]; //指示方块的属性（0表示没翻开，1表示翻开了,2表示被标记）
extern int g_mineBoom;						 //0表示没炸，1表示炸了
extern char g_numPicture[9][10];		 //储存数字对应的bmp图

/*自定义函数*/

// LCD_Draw_Point：在指定坐标(x,y)的像素点处，显示color这种颜色
void LCD_Draw_Point(int x, int y, unsigned int color);

//LCD_Draw_Rectangle:在坐标(x,y)处，画一个宽为w，高为h，颜色为color的矩形
void LCD_Draw_Rectangle(int x, int y, int w, int h, int color);

//LCD_Draw_Bmp：在坐标(x,y)处，显示名为BmpName的bmp图片
int LCD_Draw_Bmp(const char *BmpName, int x, int y);

// 在位置为(x0,y0)处显示宽为w,高为h的汉字
void LCD_Draw_Word(int x0, int y0, int w, int h, unsigned char *data, int color);

//屏幕初始化
int LCD_Init(void);

//屏幕反初始化
int LCD_DeInit(int fb);

//画背景
void Background();

//确定雷的地址
void Set_addressMine();

//为所有方块设置数字（雷区、数字区）
void Set_bolck();

//已知一个坐标，计算出周围地雷的数量
int num_mineAround(int x, int y);

//显示出二维数组的值(用于测试)
void show_array_test(int **array);

//根据生成的数据(二维数组)，显示出地雷和数字的图片，即明牌（用于测试）
void show_block_test();

//Get_InputValue:返回点击的牌的位置下标(0~15)，如果没有点中牌，返回-1
int Get_InputValue(void);

//根据点中的屏幕坐标，返回标识
int Judge_mine(int x, int y);

//递归显示连续的无雷区
int ShowSafeArea(int x, int y);

//退出的条件
int exit_conditions();

//判断是否胜利
int Judge_victory();

//判断点的位置，并且画出那个区域
void Draw_area(int num);