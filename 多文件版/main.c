#include "Minesweeper.h"
#include <unistd.h>
#include <stdio.h>

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

int main()
{
	int fb = LCD_Init(); //屏幕的初始化

	Background();			  //显示背景
	sleep(1);				  //暂停一秒
	Set_addressMine(); //设置雷的地址
	Set_bolck();			  //根据雷的地址，为所有方块设置相应的数值
	show_block_test();		  //明牌
	sleep(3);
	Background(); //恢复背景

	int Index, Num = 0, area, flag, num;
	do
	{
		if (Num != 100 && Num != 101 && Num != 102)
			Num = Get_InputValue();
		Draw_area(Num);
		switch (Num)
		{
		case 100:
		{
			do
			{
				Num = Get_InputValue();
				if (Num >= 0 && Num <= 99)
				{
					if (g_numBlock[Num % 10][Num / 10] != 10)
					{
						g_addressAttributes[Num % 10][Num / 10] = 1;
						g_numPlayer[Num % 10][Num / 10] = g_numBlock[Num % 10][Num / 10];
						ShowSafeArea(Num % 10, Num / 10);
					}
					else if (g_addressAttributes[Num % 10][Num / 10] == 0)
					{
						g_mineBoom = 1;
						break;
					}
					else
						LCD_Draw_Bmp("flag.bmp", 250 + (Num % 10) * 45, 15 + (Num / 10) * 45); /* code */
				}

			} while (Num >= 0 && Num <= 99 && exit_conditions());
		}
		break;

		case 101:
		{
			do
			{
				Num = Get_InputValue();
				if (g_addressAttributes[Num % 10][Num / 10] == 0 && Num >= 0 && Num <= 99)
				{
					LCD_Draw_Bmp("flag.bmp", 250 + (Num % 10) * 45, 15 + (Num / 10) * 45);
					g_addressAttributes[Num % 10][Num / 10] = 2;
					g_numPlayer[Num % 10][Num / 10] = 10;
				}
				else if (g_addressAttributes[Num % 10][Num / 10] != 0 && Num >= 0 && Num <= 99)
					continue;

			} while (Num >= 0 && Num <= 99 && exit_conditions());
			//Num=Get_InputValue();
			//printf("标志的%d,%d\n",exit_conditions(),g_mineBoom);
		}
		break;

		case 102:
		{

			do
			{
				Num = Get_InputValue();
				if (g_addressAttributes[Num % 10][Num / 10] == 2 && Num >= 0 && Num <= 99)
				{
					LCD_Draw_Bmp("block.bmp", 250 + (Num % 10) * 45, 15 + (Num / 10) * 45);
					g_addressAttributes[Num % 10][Num / 10] = 0;
					g_numPlayer[Num % 10][Num / 10] = 0;
				}
			} while (Num >= 0 && Num <= 99 && exit_conditions());
		}
		break;

		default:
			break;
		}
		Draw_area(Num);
		printf("%d（为0表示准备结算）,%d（为1表示炸了）\n", exit_conditions(), g_mineBoom);
	} while (exit_conditions() && g_mineBoom == 0);

	if (Judge_victory())
	{
		LCD_Draw_Bmp("victory.bmp", 0, 165);
		LCD_Draw_Rectangle(725, 0, 50, 100, 0xbdff9f);
	}

	else
	{
		LCD_Draw_Rectangle(725, 0, 50, 100, 0xbdff9f);
		LCD_Draw_Bmp("default.bmp", 0, 165);
		for (int x = 0; x < 10; x++)
		{
			for (int y = 0; y < 10; y++)
			{
				if (g_addressAttributes[x][y] == 0)
				{
					if (g_numBlock[x][y] == 10)
						LCD_Draw_Bmp("mine.bmp", 250 + x * 45, 15 + y * 45);
				}
				if (g_addressAttributes[x][y] == 2)
				{
					if (g_numPlayer[x][y] == 10 && g_numBlock[x][y] != 10)
						LCD_Draw_Bmp("ErrorFlag.bmp", 250 + x * 45, 15 + y * 45);
				}
			}
		}
	}

	LCD_DeInit(fb); //屏幕的反初始化
	return 0;
}

