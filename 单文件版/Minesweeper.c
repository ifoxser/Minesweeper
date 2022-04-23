#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <linux/fb.h>// 屏幕信息相关的头文件
#include <sys/mman.h>// mmap相关的头文件
#include <sys/ioctl.h>// ioctl相关的头文件   
#include <stdlib.h>// abs的头文件，用来求绝对值
#include <linux/input.h>
#include <time.h>


//全局变量
void * plcd=NULL;//帧缓冲的地址
struct fb_var_screeninfo fbinfo;//屏幕信息（结构体）

int address_mine[15]={0};//储存雷的地址（最多15个雷）
int NumForMine;//雷的数量（10~15个）
int num_block[10][10]={0};//所有方块的数字,特别的，雷标识为10。
int num_player[10][10]={0};//玩家所标识的数字,特别的，雷标识为10。
int address_attributes[10][10]={0};//指示方块的属性（0表示没翻开，1表示翻开了,2表示被标记）
int mine_boom=0;//0表示没炸，1表示炸了
char num_picture[8][10]={{"0.bmp"},{"1.bmp"},{"2.bmp"},{"3.bmp"},{"4.bmp"},{"5.bmp"},{"6.bmp"},{"7.bmp"}};//储存数字对应的bmp图


/*自定义函数*/

// LCD_Draw_Point：在指定坐标(x,y)的像素点处，显示color这种颜色
void LCD_Draw_Point(int x,int y,unsigned int color);

//LCD_Draw_Rectangle:在坐标(x,y)处，画一个宽为w，高为h，颜色为color的矩形
void LCD_Draw_Rectangle(int x,int y,int w,int h,int color);

//LCD_Draw_Bmp：在坐标(x,y)处，显示名为BmpName的bmp图片
int LCD_Draw_Bmp(const char *BmpName,int x,int y);

// 在位置为(x0,y0)处显示宽为w,高为h的汉字	
void LCD_Draw_Word(int x0,int y0,int w,int h,unsigned char *data,int color);

//屏幕初始化
int LCD_Init(void);

//屏幕反初始化
int LCD_DeInit(int fb);

//画背景
void Background();

//确定雷的地址
void Set_address_mine();

//为所有方块设置数字（雷区、数字区）
void Set_bolck();

//已知一个坐标，计算出周围地雷的数量
int num_mine_around(int x,int y);

//显示出二维数组的值(用于测试)
void show_array_test(int **array);

//根据生成的数据(二维数组)，显示出地雷和数字的图片，即明牌（用于测试）
void show_block_test();

//Get_InputValue:返回点击的牌的位置下标(0~15)，如果没有点中牌，返回-1
int Get_InputValue(void);

//根据点中的屏幕坐标，返回标识
int Judge_mine(int x,int y);

//递归显示连续的无雷区
int ShowSafeArea(int x,int y);

//退出的条件
int exit_conditions();

//判断是否胜利
int Judge_victory();

//判断点的位置，并且画出那个区域
void Draw_area(int num);

int main()
{
	int fb = LCD_Init();//屏幕的初始化

	Background();//显示背景
	sleep(1);//暂停一秒
	Set_address_mine();//设置雷的地址
	Set_bolck();//根据雷的地址，为所有方块设置相应的数值
	show_block_test();//明牌
	sleep(3);
	Background();//恢复背景

	int Index,Num=0,area,flag,num;
	do
	{
		if(Num!=100&&Num!=101&&Num!=102)
		Num=Get_InputValue();
		Draw_area(Num);		
		switch (Num)
		{
			case 100:
			{
				do
				{
					Num=Get_InputValue();
					if (Num>=0&&Num<=99)
					{
						if(num_block[Num%10][Num/10]!=10)
						{
							address_attributes[Num%10][Num/10]=1;
							num_player[Num%10][Num/10]=num_block[Num%10][Num/10];
							ShowSafeArea(Num%10,Num/10);
						}
						else if(address_attributes[Num%10][Num/10]==0)
						{
							mine_boom=1;
							break;
						}
						else 
						LCD_Draw_Bmp("flag.bmp",250+(Num%10)*45,15+(Num/10)*45);						/* code */
					}
					

				}while(Num>=0&&Num<=99&&exit_conditions());
			}
			break;

			case 101:
			{		
				do
				{
					Num=Get_InputValue();
					if(address_attributes[Num%10][Num/10]==0&&Num>=0&&Num<=99)
					{
						LCD_Draw_Bmp("flag.bmp",250+(Num%10)*45,15+(Num/10)*45);
						address_attributes[Num%10][Num/10]=2;
						num_player[Num%10][Num/10]=10;					
					}	
					else if(address_attributes[Num%10][Num/10]!=0&&Num>=0&&Num<=99)
					continue;										
						
				}while(Num>=0&&Num<=99&&exit_conditions());
				//Num=Get_InputValue();
				//printf("标志的%d,%d\n",exit_conditions(),mine_boom);
			}
			break;

			case 102:
			{
							
				do
				{
					Num=Get_InputValue();
					if(address_attributes[Num%10][Num/10]==2&&Num>=0&&Num<=99)
					{
						LCD_Draw_Bmp("block.bmp",250+(Num%10)*45,15+(Num/10)*45);
						address_attributes[Num%10][Num/10]=0;
						num_player[Num%10][Num/10]=0;						
					}			
				}while(Num>=0&&Num<=99&&exit_conditions());
			}
			break;

			default:			
			break;	

		}
		Draw_area(Num);	
		printf("%d（为0表示准备结算）,%d（为1表示炸了）\n",exit_conditions(),mine_boom);
	}while(exit_conditions()&&mine_boom==0);	

	


    if (Judge_victory())
	{
		LCD_Draw_Bmp("victory.bmp",0,165);	
		LCD_Draw_Rectangle(725, 0,50,100,0xbdff9f);
	}

	else 
	{
		LCD_Draw_Rectangle(725, 0,50,100,0xbdff9f);  				
		LCD_Draw_Bmp("default.bmp",0,165);
		for(int x=0;x<10;x++)
		{
			for(int y=0;y<10;y++)
			{
				if(address_attributes[x][y]==0)
				{
					if(num_block[x][y]==10)
					LCD_Draw_Bmp("mine.bmp",250+x*45,15+y*45);
				}
				if(address_attributes[x][y]==2)
				{
					if(num_player[x][y]==10&&num_block[x][y]!=10)
					LCD_Draw_Bmp("ErrorFlag.bmp",250+x*45,15+y*45);					
				}
				
			}
		}
	}
	

	LCD_DeInit(fb);//屏幕的反初始化
	return 0;
}

// LCD_Draw_Point：在指定坐标(x,y)的像素点处，显示color这种颜色
void LCD_Draw_Point(int x,int y,unsigned int color)
{
	int *p = (int *)plcd;//因为色深是32位，所以每个像素点需要4字节空间
	if(x>=0 && x<=799 && y>=0 && y<=479)
	{
		*(p+800*y+x) = color;
	}
}
//LCD_Draw_Rectangle:在坐标(x,y)处，画一个宽为w，高为h，颜色为color的矩形
void LCD_Draw_Rectangle(int x,int y,int w,int h,int color)
{
	int i,j;
	for(i=x;i<x+w;i++)
	{
		for(j=y;j<y+h;j++)
		{
			LCD_Draw_Point(i,j,color);
		}
	}
}

//LCD_Draw_Bmp：在坐标(x,y)处，显示名为BmpName的bmp图片
int LCD_Draw_Bmp(const char *BmpName,int x,int y)
{
	unsigned char dataBuf[4]={0};//用来读取数据
	int fd = open(BmpName,O_RDONLY);
	if(fd == -1)
	{
		perror("open bmp error!");
	}
	
	/*读取魔数，判断是不是BMP图片*/
	lseek(fd,0,SEEK_SET);//光标重定位
	read(fd,dataBuf,2);
	if(dataBuf[0]!=0x42 || dataBuf[1]!=0x4D)
	{
		printf("the picture is not bmp!\n");
		close(fd);
		return -1;
	}
	
	/*读取像素素组的地址偏移量 offset*/
	lseek(fd,0x0A,SEEK_SET);//光标重定位
	read(fd,dataBuf,4);
	int offset = *((int *)dataBuf);//因为图片是小端模式存储，而ARM也是小端模式，所以直接转换
	
	/*读取位图宽度 width*/
	lseek(fd,0x12,SEEK_SET);//光标重定位
	read(fd,dataBuf,4);
	int width = *((int *)dataBuf);//因为图片是小端模式存储，而ARM也是小端模式，所以直接转换
	
	/*读取位图高度 height*/
	lseek(fd,0x16,SEEK_SET);//光标重定位
	read(fd,dataBuf,4);
	int height = *((int *)dataBuf);//因为图片是小端模式存储，而ARM也是小端模式，所以直接转换
	
	/*读取像素色深 color_deepth*/
	lseek(fd,0x1C,SEEK_SET);//光标重定位
	read(fd,dataBuf,2);
	int color_deepth = *((short *)dataBuf);//因为图片是小端模式存储，而ARM也是小端模式，所以直接转换
	//printf("bmp: %ld * %ld *%d\n",width,height,color_deepth);
	
	/*读取像素数组的数据*/
	unsigned char dataBuf2[width*height*color_deepth/8];
	int k=0;
	lseek(fd,offset,SEEK_SET);
	read(fd,dataBuf2,width*height*color_deepth/8);
	
	/*利用画点函数，把像素数组保存的颜色值，画出来*/
	int i,j;
	unsigned char colorBuf[4];
	for(i=0;i<abs(height);i++)//i决定画哪一行,abs函数用来求绝对值
	{
		for(j=0;j<width;j++)//j决定画哪一列
		{
			colorBuf[0] = dataBuf2[k++];//b
			colorBuf[1] = dataBuf2[k++];//g
			colorBuf[2] = dataBuf2[k++];//r
			if(color_deepth == 32)//图片是32位的
			{
				colorBuf[3] = dataBuf2[k++];//a
			}
			else if(color_deepth == 24)//图片是24位的
			{
				colorBuf[3] = 0;//a
			}
			LCD_Draw_Point(x+j,y+(height>0?(height-i-1):i),*((int*)colorBuf));//在对应的坐标处画点
		}
		k += (4-color_deepth/8*width%4)%4;//跳过用来4字节对齐的无效数据
	}
	
	close(fd);
}

// 在位置为(x0,y0)处显示宽为w,高为h的汉字	
void LCD_Draw_Word(int x0,int y0,int w,int h,unsigned char *data,int color)
{
	int i,k;
	
	for(i=0;i<w*h/8;i++)//i代表第几个字节
	{
		for(k=0;k<8;k++)
		{
			if((data[i]<<k )&0x80)//每个字节从高位开始判断是0还是1
			{
				LCD_Draw_Point(x0+(i*8+k)%w,y0+i/(w/8),color);
			}
		}			
	}
}
/*

		type:3 code:0     value:274 //触摸的x坐标为274
		type:3 code:1     value:134 //触摸的y坐标为134
		type:1 code:330   value:1   //触摸事件发生
		type:0 code:0     value:0   //信息同步
		type:1 code:330   value:0   //触摸事件结束
		type:0 code:0     value:0   //信息同步
*/



int LCD_Init(void)
{
	/*1.　打开屏幕设备文件  /dev/fb0 */
	int fb = open("/dev/fb0",O_RDWR);
	if(fb == -1)
	{
		perror("open /dev/fb0 error");
		return -1;
	}
	/*2.　获取屏幕信息 */
	ioctl(fb, FBIOGET_VSCREENINFO, &fbinfo);
	//printf("%d * %d * %d\n",fbinfo.xres,fbinfo.yres,fbinfo.bits_per_pixel);
	/*3.　映射  mmap */
	plcd = mmap(NULL, fbinfo.xres*fbinfo.yres*fbinfo.bits_per_pixel / 8, 
	       PROT_READ|PROT_WRITE,  MAP_SHARED, fb, 0);
	if(plcd == MAP_FAILED)
	{
		perror("mmap error");
		return -1;
	}
	return fb;
}

int LCD_DeInit(int fb)
{
	/*5.解映射 munmap */
	munmap(plcd, fbinfo.xres*fbinfo.yres*fbinfo.bits_per_pixel / 8);

	/*6.关闭屏幕设备文件 */
	close(fb);
}

void Background()
{
	LCD_Draw_Rectangle(0,0,800,480,0xbdff9f);//背景颜色	
	for(int y=0;y<10;y++)//确定方块的数量、地址、图片
	{
		for(int x=0;x<10;x++)
		{
			LCD_Draw_Bmp("block.bmp",250+x*45,15+y*45);
		}
	}	
	LCD_Draw_Bmp("game.bmp",0,165);//游戏标志
	LCD_Draw_Bmp("shovel.bmp",730,150);//挖雷
	LCD_Draw_Bmp("flag.bmp",730,210);//标记
	LCD_Draw_Bmp("deflag.bmp",730,270);//取消标记
}

int Judge_repeat(int *Array,int n)//判断是否有重复的
{
	int i,j,count;
	int ary[n];
	for(count=0;count<n;count++)//保护传进来的数组
	{
		ary[count]=Array[count];
	}
	for(i=0;i<(n-1);i++) //用选择法排序
	{
		for(j=i+1;j<n;j++)
		{
			if(ary[i] > ary[j])
		   	{
				int t = ary[i];
				ary[i] = ary[j];
				ary[j] = t;
			}
		}
	}
	for(i=0;i<n-1;i++) //判断相邻的两个元素是否有相等的
	{
		if(ary[i] == ary[i+1])
		{
			return 1; //有重复
		}
	}
	return 0;//无重复
}

//确定雷的地址
void Set_address_mine()
{	
	srand(time(NULL));
	NumForMine=rand()%6+10;
	//NumForMine=10;
	for(int i=0;i<NumForMine;i++)
	{
		address_mine[i]=rand()%100;//设置雷的地址(0~99)		
		while (Judge_repeat(address_mine,(i+1)))
		address_mine[i]=rand()%100;//设置雷的地址(0~99)		
	}

	for(int m=0;m<(NumForMine-1);m++) //用选择法排序
	{
		for(int n=m+1;n<NumForMine;n++)
		{
			if(address_mine[m] > address_mine[n])
			{
				int t = address_mine[m];
				address_mine[m] = address_mine[n];
				address_mine[n] = t;
			}
		}
	}	
}

//为所有方块设置数字（雷区、数字区）
void Set_bolck()
{
	//先设置好雷区
	int count=0,x,y;
	for(y=0;y<10;y++)
	{
		for(x=0;x<10;x++)
		{
			if(((x+10*y)==address_mine[count])&&count<=NumForMine)
			{
				num_block[x][y]=10;
				count++;
			}
			
		}
	}
	//再为其他方块按照规则（雷所在的位置）设置相应的数字
	for(y=0;y<10;y++)
	{
		for(x=0;x<10;x++)
		{
			if(num_block[x][y]==10)
			continue;
			num_block[x][y]=num_mine_around(x,y);
		}
	}
}

//已知一个坐标（二维数组），计算出周围地雷的数量
int num_mine_around(int x,int y)
{
	int count=0;

	if( ((x-1)>=0)&&((y-1)>=0)&&num_block[x-1][y-1]==10)
	count++;
	if( ((y-1)>=0)&&num_block[x][y-1]==10)
	count++;
	if( ((x+1)<=9)&&((y-1)>=0)&&num_block[x+1][y-1]==10)
	count++;
	if( ((x-1)>=0)&&num_block[x-1][y]==10)
	count++;
	if( ((x+1)<=9)&&num_block[x+1][y]==10)
	count++;
	if( ((x-1)>=0)&&((y+1)<=9)&&num_block[x-1][y+1]==10)
	count++;
	if( ((y+1)<=9)&&num_block[x][y+1]==10)
	count++;
	if( ((x+1)<=9)&&((y+1)<=9)&&num_block[x+1][y+1]==10)
	count++;	

	return count;	
}

//显示出二维数组(10*10)的值(用于测试)
void show_array_test(int** array)
{
	int count=0;
	for (int m=0; m<10;m++)
	{
		for (int n=0;n<10;n++)
		{
			printf("%2d ",array[m][n]);
			count++;
			if(count%10==0)
			printf("\n");
		}
		
	}
			
}

//根据生成的数据，显示出地雷和数字的图片，即明牌（用于测试）
void show_block_test()
{
	int num;
	for(int y=0;y<10;y++)
	{
		for(int x=0;x<10;x++)
		{
			if(num_block[x][y]==10)
			LCD_Draw_Bmp("mine.bmp",250+x*45,15+y*45);
			else 
			{
				num=num_block[x][y];
				LCD_Draw_Bmp(num_picture[num],250+x*45,15+y*45);
			}
		}
	}
}


//Get_InputValue:返回点击的区域。
int Get_InputValue(void)
{
	struct input_event et;
	int x,y,temp;
	int fd = open("/dev/input/event0",O_RDONLY);
	if(fd == -1)
	{
		perror("open event0 error");
		return -1;
	}
	while(1)
	{
		int r = read(fd,&et,sizeof(et));
		if(r == sizeof(et))//读取数据完整
		{
			printf("type:%d code:%d value:%ld\n",et.type,et.code,et.value);
			if(et.type==EV_ABS && et.code==ABS_X)//value为横坐标
			{
				x = et.value;
			}
			else if(et.type==EV_ABS && et.code==ABS_Y)//value为纵坐标
			{
				y = et.value;
			}
			else if(et.type==EV_KEY && et.code==BTN_TOUCH && et.value==0)//触摸事件结束
			{
				x=x/1.28;
				y=y/1.24;
				printf("=========(%d,%d)=======\n",x,y);
				close(fd);
				if(x>=730 && x<=775 && y>=150 && y<=195)
				{
					printf("=========点中了“挖雷”的图标（100)=======\n");
					return 100;//挖雷
				}
				else if(x>=730 && x<=775 && y>=210 && y<=255)
				{
					printf("=========点中了“标记雷”的图标(101)=======\n");
					return 101;//标雷
				}
				else if(x>=730 && x<=775 && y>=270 && y<=315)
				{
					printf("=========点中了“取消标雷”的图标(101)=======\n");
					return 102;//取消标雷
				}				
				else if(x>=250 && x<=700 && y>=15 && y<=465)
				{
					temp=Judge_mine(x, y);
					printf("=========(点中了游戏区，位置号为：%d)=======\n",temp);
					return temp;//方块内
				}
				else 
				{
					printf("=========(点了个寂寞)=======\n");
					return -1;//其他区域
				}
					
	
			}
			
		}
	}
	close(fd);
}

//根据点中的屏幕坐标，返回数组下标
int Judge_mine(int x,int y)
{
	int Index_x,Index_y,num;
	Index_x=(x-250)/45;
	Index_y=(y-15)/45;
	return num=(Index_x+Index_y*10);
}

int ShowSafeArea(int x,int y)
{
	
	int num=num_block[x][y];
	num_player[x][y]=num;
	address_attributes[x][y]=1;
	if(num!=0)//点中的是数字
	{
		LCD_Draw_Bmp(num_picture[num],250+x*45,15+y*45);
		return 0;
	}
	else
	LCD_Draw_Bmp(num_picture[0],250+x*45,15+y*45);
	
	
	if( ((x-1)>=0)&&((y-1)>=0)&&num_block[x-1][y-1]!=10&&address_attributes[x-1][y-1]==0)
	{
		num=num_block[x-1][y-1];
		num_player[x-1][y-1]=num;
		if(num!=0)
		{
			address_attributes[x-1][y-1]=1;
			LCD_Draw_Bmp(num_picture[num],250+(x-1)*45,15+(y-1)*45);
		}
		
		else
		{
			address_attributes[x-1][y-1]=1;
			ShowSafeArea(x-1,y-1);			
		}
				
	}
	
	if( ((y-1)>=0)&&num_block[x][y-1]!=10&&address_attributes[x][y-1]==0)
	{
		num=num_block[x][y-1];
		num_player[x][y-1]=num;
		if(num!=0)
		{
			LCD_Draw_Bmp(num_picture[num],250+x*45,15+(y-1)*45);
			address_attributes[x][y-1]=1;
		}
		
		else
		{
			address_attributes[x][y-1]=1;
			ShowSafeArea(x,y-1);			
		}		
	}

	if( ((x+1)<=9)&&((y-1)>=0)&&num_block[x+1][y-1]!=10&&address_attributes[x+1][y-1]==0)
	{
		num=num_block[x+1][y-1];
		num_player[x+1][y-1]=num;
		if(num!=0)
		{
			LCD_Draw_Bmp(num_picture[num],250+(x+1)*45,15+(y-1)*45);
			address_attributes[x+1][y-1]=1;
		}
		
		else
		{
			address_attributes[x+1][y-1]=1;
			ShowSafeArea(x+1,y-1);			
		}		
	}

	if( ((x-1)>=0)&&num_block[x-1][y]!=10&&address_attributes[x-1][y]==0)
	{
		num=num_block[x-1][y];
		num_player[x-1][y]=num;
		if(num!=0)
		{
			LCD_Draw_Bmp(num_picture[num],250+(x-1)*45,15+y*45);
			address_attributes[x-1][y]=1;
		}
		
		else
		{
			address_attributes[x-1][y]=1;
			ShowSafeArea(x-1,y);			
		}		
	}

	if( ((x+1)<=9)&&num_block[x+1][y]!=10&&address_attributes[x+1][y]==0)
	{
		num=num_block[x+1][y];
		num_player[x+1][y]=num;
		if(num!=0)
		{
			LCD_Draw_Bmp(num_picture[num],250+(x+1)*45,15+y*45);
			address_attributes[x+1][y]=1;
		}
		
		else
		{
			address_attributes[x+1][y]=1;
			ShowSafeArea(x+1,y);			
		}		
	}

	if( ((x-1)>=0)&&((y+1)<=9)&&num_block[x-1][y+1]!=10&&address_attributes[x-1][y+1]==0)
	{
		num=num_block[x-1][y+1];
		num_player[x-1][y+1]=num;
		if(num!=0)
		{
			LCD_Draw_Bmp(num_picture[num],250+(x-1)*45,15+(y+1)*45);
			address_attributes[x-1][y+1]=1;
		}
		
		else
		{
			address_attributes[x-1][y+1]=1;
			ShowSafeArea(x-1,y+1);			
		}		
	}

	if( ((y+1)<=9)&&num_block[x][y+1]!=10&&address_attributes[x][y+1]==0)
	{
		num=num_block[x][y+1];
		num_player[x][y+1]=num;
		if(num!=0)
		{
			LCD_Draw_Bmp(num_picture[num],250+x*45,15+(y+1)*45);
			address_attributes[x][y+1]=1;
		}
		
		else
		{
			address_attributes[x][y+1]=1;
			ShowSafeArea(x,y+1);			
		}		
	}

	if( ((x+1)<=9)&&((y+1)<=9)&&num_block[x+1][y+1]!=10&&address_attributes[x+1][y+1]==0)
	{
		num=num_block[x+1][y+1];
		num_player[x+1][y+1]=num;
		if(num!=0)
		{
			LCD_Draw_Bmp(num_picture[num],250+(x+1)*45,15+(y+1)*45);
			address_attributes[x+1][y+1]=1;
		}
		
		else
		{
			address_attributes[x+1][y+1]=1;
			ShowSafeArea(x+1,y+1);			
		}	
	}
	return 0;
}


//退出的条件之一，为1表示翻开的牌加上标记过的牌为100
int exit_conditions()
{
	int count=0;
	for (int i = 0; i < 100; i++)
	{
		if (address_attributes[i%10][i/10]>0)
		count++;		
	}

	if (count==100)
	return 0;

	return 1;
}

//判断是否胜利（0表示输了，1表示赢了）
int Judge_victory()
{
	int i,flag=0;
	if( mine_boom==1 )
	return 0;

	for (int i = 0; i < 100; i++)
	{
		if(num_player[i%10][i/10]==num_block[i%10][i/10])
		flag++;	
	}
	if (flag==100)
	return 1;
	else 
	return 0;	
}


//判断点的位置，并且画出那个区域
void Draw_area(int num)
{
	if(num==100)
	{
		LCD_Draw_Rectangle(730,195,45,10,0xCD55555);
		LCD_Draw_Rectangle(730,255,45,10,0xbdff9f);
		LCD_Draw_Rectangle(730,315,45,10,0xbdff9f); 
		LCD_Draw_Rectangle(725, 0,50,100,0xbdff9f);         
	}
	else if(num==101)
	{
		LCD_Draw_Rectangle(730,255,45,10,0xCD55555);
		LCD_Draw_Rectangle(730,195,45,10,0xbdff9f);
		LCD_Draw_Rectangle(730,315,45,10,0xbdff9f); 
		LCD_Draw_Rectangle(725, 0,50,100,0xbdff9f);
	}
	else if(num==102)
	{
		LCD_Draw_Rectangle(730,315,45,10,0xCD55555);
		LCD_Draw_Rectangle(730,195,45,10,0xbdff9f);
		LCD_Draw_Rectangle(730,255,45,10,0xbdff9f);
		LCD_Draw_Rectangle(725, 0,50,100,0xbdff9f); 			
	}
	else
	{
		LCD_Draw_Bmp("arrow.bmp",725,0);
		LCD_Draw_Rectangle(730,315,45,10,0xbdff9f);
		LCD_Draw_Rectangle(730,195,45,10,0xbdff9f);
		LCD_Draw_Rectangle(730,255,45,10,0xbdff9f); 			
	};		
}

