/*五子棋
 *1.0：双人模式，采用eaxy.x库绘出棋盘画线，定坐标，依坐标画出定位框
 *     ，宏定义键值来操作移动，定义变量来计数下的步数，模2以判断是黑白子、当前手，
 *     空格键下子，判断模块：判断是否五子一线以定输赢，显示输赢
 * 
 *     四子连珠时高亮显示，五子连珠高亮 
 *2.0：菜单及设置，生成棋谱保存，悔棋功能，可重新玩，并计分
 *
 *
 *
 *3.0：电脑玩家的模块，可设定难度等级，可设置双人模式或电脑模式
 *     动画效果
 *
 *棋盘格子间距40，棋子半径15，14个格子
 *
 *
 *
 *
 */
#include <stdio.h>
#include <graphics.h>
#include <stdlib.h>
#include <conio.h>

#define DOWN 80
#define UP   72
#define LEFT 75
#define RIGHT 77
#define ESC 27
#define BLANK 32
#define DEBUG

typedef enum{EMPTY, BLACK_PLAYER, WHITE_PLAYER}  Winner;

enum point_value {E = 0, B = 1, W = 2};
struct Point
{
	point_value value;
	int i;
	int j;
};

void welcome(void);
void initial_chess(Point *chess);
void draw_board(void);
void draw_pieces(Point *cur, const int steps);
void draw_frame(Point *cur);
void clear_frame(Point *cur);
void play(Point *chess, int *steps);
void show_winner(Winner result);
#ifdef DEBUG
void warning(Point *cur);
#endif
Winner judge_winner(Point *cur);
BOOL is_fall(Point *cur);

int main(void)
{
	Point chess_piece[15][15];
   int steps = 0;
   
	initgraph(800, 600);
	initial_chess(chess_piece[0]);
   draw_board();
   draw_frame(&chess_piece[7][7]);
   play(&chess_piece[7][7], &steps);
   

	getch();
	closegraph();
	return 0;
}

/*
 *
 *
 *
 *
 */
void initial_chess(Point *chess)
{
	int i;
	int j;

	for(i = 0; i < 15; i++)
		for(j = 0; j < 15; j++)
		{
			( *( chess + 15 * i + j ) ).i = i;
         ( *( chess + 15 * i + j ) ).j = j;
			( *( chess + 15 * i + j ) ).value = E; 
		}
}

/*
 *
 *
 *
 *
 */
void welcome(void)
{
	setfont(50, 0, "微软雅黑");
	outtextxy(300, 200, "WELCOME!");
   setfont(20, 0, "宋体");
	outtextxy(300, 260, "载入中......");
	Sleep(3000);
}
/*
 *
 *
 *
 */
void draw_board(void)
{
	int i;

	welcome();

	setfillcolor(WHITE);
	solidrectangle(0, 0, 900, 600);

	setfillcolor(YELLOW);
	bar(0, 0, 600, 600);

	setfillcolor(LIGHTGRAY);
   solidrectangle(600, 0, 800, 600);
   
	setfillcolor(CYAN);
   solidrectangle(620, 20, 770, 80);

	setfont(50, 0, "隶书");
	settextcolor(BLUE);
	setbkmode(TRANSPARENT);
	outtextxy(620, 20, "五子棋");

   setfillcolor(BLUE);
	bar(620, 100, 680, 140);
   bar(620, 250, 680, 290);
	setfont(30, 0, "宋体");
	settextcolor(GREEN);
	outtextxy(620, 100, "黑方");
	setfillcolor(BLACK);
	solidcircle(650, 180, 15);
	outtextxy(620, 250, "白方");

	setfont(15, 0, "黑体");
	settextcolor(BLACK);
	outtextxy(602, 360, "操作说明：");
	outtextxy(602, 400, "↑↓←→键控制上下左右");
	outtextxy(602, 440, "ESC键退出游戏，空格键落子");

	setcolor(BLACK);
	setlinestyle(PS_SOLID, 2);

	for(i = 0; i < 15; i++)
	{
		line(20, 20 + 40 * i, 580, 20 + 40 * i);
		line(20 + 40 * i, 20, 20 + 40 * i, 580);
	}
}

/*
 *
 *
 *
 *
 */
void draw_pieces(Point *cur, const int steps)
{
	setlinestyle(PS_SOLID, 1);

	if(steps % 2 == 0)
	{
		setfillcolor(WHITE);
		cur->value = W;
	}
	else if(steps % 2 == 1)
	{
		setfillcolor(BLACK);
      cur->value = B;
	}
	else
	{
		fprintf(stderr, "Variable steps error!");
      exit(EXIT_FAILURE);
	}
 
	fillcircle(20 + 40 * cur->j, 20 + 40 * cur->i, 15);
}

/*
 *
 *
 *
 *
 */
void draw_frame(Point *cur)
{
	int x = 20 + 40 * cur->j;
	int y = 20 + 40 * cur->i;

   setcolor(BLUE);
   setlinestyle(PS_SOLID, 1);

	line(x - 15, y - 15, x - 5, y - 15);
	line(x - 15, y - 15, x - 15, y - 5);
	line(x + 5, y - 15, x + 15, y - 15);
	line(x + 15, y - 15, x + 15, y - 5);
	line(x - 15, y + 5, x - 15, y + 15);
	line(x - 15, y + 15, x - 5, y + 15);
	line(x + 5, y + 15, x + 15, y + 15);
	line(x + 15, y + 5, x + 15, y + 15);
}

/*
 *
 *
 *
 */
void clear_frame(Point *cur)
{
	int x = 20 + 40 * cur->j;
	int y = 20 + 40 * cur->i;
	
   setcolor(YELLOW);
   setlinestyle(PS_SOLID, 1);

	line(x - 15, y - 15, x - 5, y - 15);
	line(x - 15, y - 15, x - 15, y - 5);
	line(x + 5, y - 15, x + 15, y - 15);
	line(x + 15, y - 15, x + 15, y - 5);
	line(x - 15, y + 5, x - 15, y + 15);
	line(x - 15, y + 15, x - 5, y + 15);
	line(x + 5, y + 15, x + 15, y + 15);
	line(x + 15, y + 5, x + 15, y + 15);
}
/*
 *
 *
 *
 *
 */
void play(Point *chess, int *steps)
{
	int key_value;
   int shift_i = 0;
	int shift_j = 0;
	BOOL is_win = FALSE;
	Winner reslut;
   Point *cur = chess;

   while( ( key_value = getch() ) != 0 )	     
	{
		switch(key_value)
		{
			case UP:
				shift_i--;
				if(shift_i < -7)
				{
					shift_i = -7;
               break;
				}
				else
				{
					clear_frame(cur);
					cur = chess + 15 * shift_i + shift_j;
					draw_frame(cur);
					break;
				}
			case DOWN:
				shift_i++;
				if(shift_i > 7)
				{
					shift_i = 7;
               break;
				}
				else
				{
					clear_frame(cur);
					cur = chess + 15 * shift_i + shift_j;
					draw_frame(cur);
					break;
				}
			case LEFT:
				shift_j--;
				if(shift_j < -7)
				{
					shift_j = -7;
               break;
				}
				else
				{
					clear_frame(cur);
					cur = chess + 15 * shift_i + shift_j;
					draw_frame(cur);
					break;
				}
			case RIGHT:
				shift_j++;
				if(shift_j > 7)
				{
					shift_j = 7;
               break;
				}
				else
				{
					clear_frame(cur);
					cur = chess + 15 * shift_i + shift_j;
					draw_frame(cur);
					break;
				}
			case BLANK:
				if(is_win == FALSE)
				{
					if(is_fall(cur) == TRUE)
					{
						(*steps)++;
						draw_pieces(cur, *steps);

						setfont(30, 0, "宋体");
						settextcolor(GREEN);
						if(*steps % 2 == 0)
						{
							setfillcolor(LIGHTGRAY);
							solidcircle(650, 330, 15);
							setfillcolor(BLACK);
							solidcircle(650, 180, 15);
						}
						else
						{
							setfillcolor(LIGHTGRAY);
							solidcircle(650, 180, 15);
							setfillcolor(WHITE);
							solidcircle(650, 330, 15);
						}

						if(*steps > 6)
							warning();
						if(*steps > 8)
						{
							reslut = judge_winner(cur);
							if(reslut != EMPTY)
							{
								show_winner(reslut);
								is_win = TRUE;
							}
						}
					}
				}
				break;
			case ESC:
				exit(EXIT_SUCCESS);
			default:
				break;
		}
	}
}

/*
 *
 *
 *
 *
 */
BOOL is_fall(Point *cur)
{
	if(cur->value == E)
		return TRUE;
	else
	   return FALSE;
}

/*
 *
 *
 *
 *
 */
Winner judge_winner(Point *cur)
{
   int part_1 = 0;
	int part_2 = 0;
	Winner result = EMPTY;
   Point *temp;

	if(cur->value != E)
	{
		if(cur->value == B)
			result = BLACK_PLAYER;
      else 
			result = WHITE_PLAYER;

		for(temp = cur; 
		    temp->value == cur->value && temp->i == cur->i; 
			 temp -= 1)
			part_1++;
		for(temp = cur; 
		    temp->value == cur->value && temp->i == cur->i;
			 temp += 1)
			part_2++;
		if(part_1 + part_2 == 6)
		   return result;

		part_1 = 0;
		part_2 = 0;
		for(temp = cur; 
		    temp->value == cur->value && temp->j >= 0 && temp->j <= 14;
			 temp -= 15)
			part_1++;
      for(temp = cur; 
		    temp->value == cur->value && temp->j >= 0 && temp->j <= 14;
			 temp += 15)
			part_2++;
		if(part_1 + part_2 == 6)
		   return result;

		part_1 = 0;
		part_2 = 0;
		for(temp = cur;
		    temp->value == cur->value && temp->i >= 0 && temp->i <= 14 &&        \
				 temp->j >= 0 && temp->j <= 14;
			 temp -= 16)
			part_1++;
      for(temp = cur;
		    temp->value == cur->value && temp->i >= 0 && temp->i <= 14 &&        \
				 temp->j >= 0 && temp->j <= 14;
			 temp += 16)
			part_2++;
		if(part_1 + part_2 == 6)
		   return result;

      part_1 = 0;
		part_2 = 0;
		for(temp = cur;
		    temp->value == cur->value && temp->i >= 0 && temp->i <= 14 &&        \
				 temp->j >= 0 && temp->j <= 14;
			 temp -= 14)
			part_1++;
		for(temp = cur;
		    temp->value == cur->value && temp->i >= 0 && temp->i <= 14 &&        \
				 temp->j >= 0 && temp->j <= 14;
			 temp += 14)
			part_2++;
		if(part_1 + part_2 == 6)
		   return result;
	}

	return EMPTY;
}

/*
 *
 *
 *
 *
 *
 */
#ifdef DEBUG
void warning(Point *cur)
{

}
#endif
/*
 *
 *
 *
 *
 *
 */
void show_winner(Winner result)
{
	setfont(40, 0, _T("幼圆"));
	settextcolor(LIGHTBLUE);

	switch(result)
	{
		case BLACK_PLAYER:
			setfillcolor(WHITE);
			solidrectangle(150, 300, 480, 350);
			setbkmode(TRANSPARENT);
			settextcolor(BLACK);
			outtextxy(160, 300, "恭喜黑方玩家获胜");
		   break;
		case WHITE_PLAYER:
			setfillcolor(WHITE);
			solidrectangle(150, 300, 480, 350);
			setbkmode(TRANSPARENT);
			settextcolor(BLACK);
			outtextxy(160, 300, "恭喜白方玩家获胜");
			break;
	   default:
			fprintf(stderr, "Result error!");
			exit(EXIT_FAILURE);
			break;
	}
}