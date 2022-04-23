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

/*�Զ��庯��*/

// LCD_Draw_Point����ָ������(x,y)�����ص㴦����ʾcolor������ɫ
void LCD_Draw_Point(int x, int y, unsigned int color);

//LCD_Draw_Rectangle:������(x,y)������һ����Ϊw����Ϊh����ɫΪcolor�ľ���
void LCD_Draw_Rectangle(int x, int y, int w, int h, int color);

//LCD_Draw_Bmp��������(x,y)������ʾ��ΪBmpName��bmpͼƬ
int LCD_Draw_Bmp(const char *BmpName, int x, int y);

// ��λ��Ϊ(x0,y0)����ʾ��Ϊw,��Ϊh�ĺ���
void LCD_Draw_Word(int x0, int y0, int w, int h, unsigned char *data, int color);

//��Ļ��ʼ��
int LCD_Init(void);

//��Ļ����ʼ��
int LCD_DeInit(int fb);

//������
void Background();

//ȷ���׵ĵ�ַ
void Set_addressMine();

//Ϊ���з����������֣���������������
void Set_bolck();

//��֪һ�����꣬�������Χ���׵�����
int num_mineAround(int x, int y);

//��ʾ����ά�����ֵ(���ڲ���)
void show_array_test(int **array);

//�������ɵ�����(��ά����)����ʾ�����׺����ֵ�ͼƬ�������ƣ����ڲ��ԣ�
void show_block_test();

//Get_InputValue:���ص�����Ƶ�λ���±�(0~15)�����û�е����ƣ�����-1
int Get_InputValue(void);

//���ݵ��е���Ļ���꣬���ر�ʶ
int Judge_mine(int x, int y);

//�ݹ���ʾ������������
int ShowSafeArea(int x, int y);

//�˳�������
int exit_conditions();

//�ж��Ƿ�ʤ��
int Judge_victory();

//�жϵ��λ�ã����һ����Ǹ�����
void Draw_area(int num);