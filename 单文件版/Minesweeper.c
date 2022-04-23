#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <linux/fb.h>// ��Ļ��Ϣ��ص�ͷ�ļ�
#include <sys/mman.h>// mmap��ص�ͷ�ļ�
#include <sys/ioctl.h>// ioctl��ص�ͷ�ļ�   
#include <stdlib.h>// abs��ͷ�ļ������������ֵ
#include <linux/input.h>
#include <time.h>


//ȫ�ֱ���
void * plcd=NULL;//֡����ĵ�ַ
struct fb_var_screeninfo fbinfo;//��Ļ��Ϣ���ṹ�壩

int address_mine[15]={0};//�����׵ĵ�ַ�����15���ף�
int NumForMine;//�׵�������10~15����
int num_block[10][10]={0};//���з��������,�ر�ģ��ױ�ʶΪ10��
int num_player[10][10]={0};//�������ʶ������,�ر�ģ��ױ�ʶΪ10��
int address_attributes[10][10]={0};//ָʾ��������ԣ�0��ʾû������1��ʾ������,2��ʾ����ǣ�
int mine_boom=0;//0��ʾûը��1��ʾը��
char num_picture[8][10]={{"0.bmp"},{"1.bmp"},{"2.bmp"},{"3.bmp"},{"4.bmp"},{"5.bmp"},{"6.bmp"},{"7.bmp"}};//�������ֶ�Ӧ��bmpͼ


/*�Զ��庯��*/

// LCD_Draw_Point����ָ������(x,y)�����ص㴦����ʾcolor������ɫ
void LCD_Draw_Point(int x,int y,unsigned int color);

//LCD_Draw_Rectangle:������(x,y)������һ����Ϊw����Ϊh����ɫΪcolor�ľ���
void LCD_Draw_Rectangle(int x,int y,int w,int h,int color);

//LCD_Draw_Bmp��������(x,y)������ʾ��ΪBmpName��bmpͼƬ
int LCD_Draw_Bmp(const char *BmpName,int x,int y);

// ��λ��Ϊ(x0,y0)����ʾ��Ϊw,��Ϊh�ĺ���	
void LCD_Draw_Word(int x0,int y0,int w,int h,unsigned char *data,int color);

//��Ļ��ʼ��
int LCD_Init(void);

//��Ļ����ʼ��
int LCD_DeInit(int fb);

//������
void Background();

//ȷ���׵ĵ�ַ
void Set_address_mine();

//Ϊ���з����������֣���������������
void Set_bolck();

//��֪һ�����꣬�������Χ���׵�����
int num_mine_around(int x,int y);

//��ʾ����ά�����ֵ(���ڲ���)
void show_array_test(int **array);

//�������ɵ�����(��ά����)����ʾ�����׺����ֵ�ͼƬ�������ƣ����ڲ��ԣ�
void show_block_test();

//Get_InputValue:���ص�����Ƶ�λ���±�(0~15)�����û�е����ƣ�����-1
int Get_InputValue(void);

//���ݵ��е���Ļ���꣬���ر�ʶ
int Judge_mine(int x,int y);

//�ݹ���ʾ������������
int ShowSafeArea(int x,int y);

//�˳�������
int exit_conditions();

//�ж��Ƿ�ʤ��
int Judge_victory();

//�жϵ��λ�ã����һ����Ǹ�����
void Draw_area(int num);

int main()
{
	int fb = LCD_Init();//��Ļ�ĳ�ʼ��

	Background();//��ʾ����
	sleep(1);//��ͣһ��
	Set_address_mine();//�����׵ĵ�ַ
	Set_bolck();//�����׵ĵ�ַ��Ϊ���з���������Ӧ����ֵ
	show_block_test();//����
	sleep(3);
	Background();//�ָ�����

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
				//printf("��־��%d,%d\n",exit_conditions(),mine_boom);
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
		printf("%d��Ϊ0��ʾ׼�����㣩,%d��Ϊ1��ʾը�ˣ�\n",exit_conditions(),mine_boom);
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
	

	LCD_DeInit(fb);//��Ļ�ķ���ʼ��
	return 0;
}

// LCD_Draw_Point����ָ������(x,y)�����ص㴦����ʾcolor������ɫ
void LCD_Draw_Point(int x,int y,unsigned int color)
{
	int *p = (int *)plcd;//��Ϊɫ����32λ������ÿ�����ص���Ҫ4�ֽڿռ�
	if(x>=0 && x<=799 && y>=0 && y<=479)
	{
		*(p+800*y+x) = color;
	}
}
//LCD_Draw_Rectangle:������(x,y)������һ����Ϊw����Ϊh����ɫΪcolor�ľ���
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

//LCD_Draw_Bmp��������(x,y)������ʾ��ΪBmpName��bmpͼƬ
int LCD_Draw_Bmp(const char *BmpName,int x,int y)
{
	unsigned char dataBuf[4]={0};//������ȡ����
	int fd = open(BmpName,O_RDONLY);
	if(fd == -1)
	{
		perror("open bmp error!");
	}
	
	/*��ȡħ�����ж��ǲ���BMPͼƬ*/
	lseek(fd,0,SEEK_SET);//����ض�λ
	read(fd,dataBuf,2);
	if(dataBuf[0]!=0x42 || dataBuf[1]!=0x4D)
	{
		printf("the picture is not bmp!\n");
		close(fd);
		return -1;
	}
	
	/*��ȡ��������ĵ�ַƫ���� offset*/
	lseek(fd,0x0A,SEEK_SET);//����ض�λ
	read(fd,dataBuf,4);
	int offset = *((int *)dataBuf);//��ΪͼƬ��С��ģʽ�洢����ARMҲ��С��ģʽ������ֱ��ת��
	
	/*��ȡλͼ��� width*/
	lseek(fd,0x12,SEEK_SET);//����ض�λ
	read(fd,dataBuf,4);
	int width = *((int *)dataBuf);//��ΪͼƬ��С��ģʽ�洢����ARMҲ��С��ģʽ������ֱ��ת��
	
	/*��ȡλͼ�߶� height*/
	lseek(fd,0x16,SEEK_SET);//����ض�λ
	read(fd,dataBuf,4);
	int height = *((int *)dataBuf);//��ΪͼƬ��С��ģʽ�洢����ARMҲ��С��ģʽ������ֱ��ת��
	
	/*��ȡ����ɫ�� color_deepth*/
	lseek(fd,0x1C,SEEK_SET);//����ض�λ
	read(fd,dataBuf,2);
	int color_deepth = *((short *)dataBuf);//��ΪͼƬ��С��ģʽ�洢����ARMҲ��С��ģʽ������ֱ��ת��
	//printf("bmp: %ld * %ld *%d\n",width,height,color_deepth);
	
	/*��ȡ�������������*/
	unsigned char dataBuf2[width*height*color_deepth/8];
	int k=0;
	lseek(fd,offset,SEEK_SET);
	read(fd,dataBuf2,width*height*color_deepth/8);
	
	/*���û��㺯�������������鱣�����ɫֵ��������*/
	int i,j;
	unsigned char colorBuf[4];
	for(i=0;i<abs(height);i++)//i��������һ��,abs�������������ֵ
	{
		for(j=0;j<width;j++)//j��������һ��
		{
			colorBuf[0] = dataBuf2[k++];//b
			colorBuf[1] = dataBuf2[k++];//g
			colorBuf[2] = dataBuf2[k++];//r
			if(color_deepth == 32)//ͼƬ��32λ��
			{
				colorBuf[3] = dataBuf2[k++];//a
			}
			else if(color_deepth == 24)//ͼƬ��24λ��
			{
				colorBuf[3] = 0;//a
			}
			LCD_Draw_Point(x+j,y+(height>0?(height-i-1):i),*((int*)colorBuf));//�ڶ�Ӧ�����괦����
		}
		k += (4-color_deepth/8*width%4)%4;//��������4�ֽڶ������Ч����
	}
	
	close(fd);
}

// ��λ��Ϊ(x0,y0)����ʾ��Ϊw,��Ϊh�ĺ���	
void LCD_Draw_Word(int x0,int y0,int w,int h,unsigned char *data,int color)
{
	int i,k;
	
	for(i=0;i<w*h/8;i++)//i����ڼ����ֽ�
	{
		for(k=0;k<8;k++)
		{
			if((data[i]<<k )&0x80)//ÿ���ֽڴӸ�λ��ʼ�ж���0����1
			{
				LCD_Draw_Point(x0+(i*8+k)%w,y0+i/(w/8),color);
			}
		}			
	}
}
/*

		type:3 code:0     value:274 //������x����Ϊ274
		type:3 code:1     value:134 //������y����Ϊ134
		type:1 code:330   value:1   //�����¼�����
		type:0 code:0     value:0   //��Ϣͬ��
		type:1 code:330   value:0   //�����¼�����
		type:0 code:0     value:0   //��Ϣͬ��
*/



int LCD_Init(void)
{
	/*1.������Ļ�豸�ļ�  /dev/fb0 */
	int fb = open("/dev/fb0",O_RDWR);
	if(fb == -1)
	{
		perror("open /dev/fb0 error");
		return -1;
	}
	/*2.����ȡ��Ļ��Ϣ */
	ioctl(fb, FBIOGET_VSCREENINFO, &fbinfo);
	//printf("%d * %d * %d\n",fbinfo.xres,fbinfo.yres,fbinfo.bits_per_pixel);
	/*3.��ӳ��  mmap */
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
	/*5.��ӳ�� munmap */
	munmap(plcd, fbinfo.xres*fbinfo.yres*fbinfo.bits_per_pixel / 8);

	/*6.�ر���Ļ�豸�ļ� */
	close(fb);
}

void Background()
{
	LCD_Draw_Rectangle(0,0,800,480,0xbdff9f);//������ɫ	
	for(int y=0;y<10;y++)//ȷ���������������ַ��ͼƬ
	{
		for(int x=0;x<10;x++)
		{
			LCD_Draw_Bmp("block.bmp",250+x*45,15+y*45);
		}
	}	
	LCD_Draw_Bmp("game.bmp",0,165);//��Ϸ��־
	LCD_Draw_Bmp("shovel.bmp",730,150);//����
	LCD_Draw_Bmp("flag.bmp",730,210);//���
	LCD_Draw_Bmp("deflag.bmp",730,270);//ȡ�����
}

int Judge_repeat(int *Array,int n)//�ж��Ƿ����ظ���
{
	int i,j,count;
	int ary[n];
	for(count=0;count<n;count++)//����������������
	{
		ary[count]=Array[count];
	}
	for(i=0;i<(n-1);i++) //��ѡ������
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
	for(i=0;i<n-1;i++) //�ж����ڵ�����Ԫ���Ƿ�����ȵ�
	{
		if(ary[i] == ary[i+1])
		{
			return 1; //���ظ�
		}
	}
	return 0;//���ظ�
}

//ȷ���׵ĵ�ַ
void Set_address_mine()
{	
	srand(time(NULL));
	NumForMine=rand()%6+10;
	//NumForMine=10;
	for(int i=0;i<NumForMine;i++)
	{
		address_mine[i]=rand()%100;//�����׵ĵ�ַ(0~99)		
		while (Judge_repeat(address_mine,(i+1)))
		address_mine[i]=rand()%100;//�����׵ĵ�ַ(0~99)		
	}

	for(int m=0;m<(NumForMine-1);m++) //��ѡ������
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

//Ϊ���з����������֣���������������
void Set_bolck()
{
	//�����ú�����
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
	//��Ϊ�������鰴�չ��������ڵ�λ�ã�������Ӧ������
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

//��֪һ�����꣨��ά���飩���������Χ���׵�����
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

//��ʾ����ά����(10*10)��ֵ(���ڲ���)
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

//�������ɵ����ݣ���ʾ�����׺����ֵ�ͼƬ�������ƣ����ڲ��ԣ�
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


//Get_InputValue:���ص��������
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
		if(r == sizeof(et))//��ȡ��������
		{
			printf("type:%d code:%d value:%ld\n",et.type,et.code,et.value);
			if(et.type==EV_ABS && et.code==ABS_X)//valueΪ������
			{
				x = et.value;
			}
			else if(et.type==EV_ABS && et.code==ABS_Y)//valueΪ������
			{
				y = et.value;
			}
			else if(et.type==EV_KEY && et.code==BTN_TOUCH && et.value==0)//�����¼�����
			{
				x=x/1.28;
				y=y/1.24;
				printf("=========(%d,%d)=======\n",x,y);
				close(fd);
				if(x>=730 && x<=775 && y>=150 && y<=195)
				{
					printf("=========�����ˡ����ס���ͼ�꣨100)=======\n");
					return 100;//����
				}
				else if(x>=730 && x<=775 && y>=210 && y<=255)
				{
					printf("=========�����ˡ�����ס���ͼ��(101)=======\n");
					return 101;//����
				}
				else if(x>=730 && x<=775 && y>=270 && y<=315)
				{
					printf("=========�����ˡ�ȡ�����ס���ͼ��(101)=======\n");
					return 102;//ȡ������
				}				
				else if(x>=250 && x<=700 && y>=15 && y<=465)
				{
					temp=Judge_mine(x, y);
					printf("=========(��������Ϸ����λ�ú�Ϊ��%d)=======\n",temp);
					return temp;//������
				}
				else 
				{
					printf("=========(���˸���į)=======\n");
					return -1;//��������
				}
					
	
			}
			
		}
	}
	close(fd);
}

//���ݵ��е���Ļ���꣬���������±�
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
	if(num!=0)//���е�������
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


//�˳�������֮һ��Ϊ1��ʾ�������Ƽ��ϱ�ǹ�����Ϊ100
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

//�ж��Ƿ�ʤ����0��ʾ���ˣ�1��ʾӮ�ˣ�
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


//�жϵ��λ�ã����һ����Ǹ�����
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

