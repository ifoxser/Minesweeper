#include "Minesweeper.h"
#include <unistd.h>
#include <stdio.h>

//ȫ�ֱ���(����ʽ����)
extern void *g_plcd;						 //֡����ĵ�ַ
extern struct fb_var_screeninfo g_fbinfo;	 //��Ļ��Ϣ���ṹ�壩
extern int g_addressMine[15];			 //�����׵ĵ�ַ�����15���ף�
extern int g_numMine;						 //�׵�������10~15����
extern int g_numBlock[10][10];			 //���з��������,�ر�ģ��ױ�ʶΪ10��
extern int g_numPlayer[10][10];		 //�������ʶ������,�ر�ģ��ױ�ʶΪ10��
extern int g_addressAttributes[10][10]; //ָʾ��������ԣ�0��ʾû������1��ʾ������,2��ʾ����ǣ�
extern int g_mineBoom;						 //0��ʾûը��1��ʾը��
extern char g_numPicture[9][10];		 //�������ֶ�Ӧ��bmpͼ

int main()
{
	int fb = LCD_Init(); //��Ļ�ĳ�ʼ��

	Background();			  //��ʾ����
	sleep(1);				  //��ͣһ��
	Set_addressMine(); //�����׵ĵ�ַ
	Set_bolck();			  //�����׵ĵ�ַ��Ϊ���з���������Ӧ����ֵ
	show_block_test();		  //����
	sleep(3);
	Background(); //�ָ�����

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
			//printf("��־��%d,%d\n",exit_conditions(),g_mineBoom);
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
		printf("%d��Ϊ0��ʾ׼�����㣩,%d��Ϊ1��ʾը�ˣ�\n", exit_conditions(), g_mineBoom);
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

	LCD_DeInit(fb); //��Ļ�ķ���ʼ��
	return 0;
}

