/*五子棋
 *1.0：双人模式，采用eaxy.x库绘出棋盘画线，定坐标，依坐标画出定位框
 *     ，宏定义键值来操作移动，定义变量来计数下的步数，模2以判断是黑白子、当前手，
 *     空格键下子，判断模块：判断是否五子一线以定输赢，显示输赢
 *
 *     五子连珠
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
#include <math.h>
#pragma   comment(lib, "Winmm.lib ")

#define NEAREST(x) ( floor( (x) + 0.5 ) )
#define DEBUG

typedef enum {EMPTY, BLACK_PLAYER, WHITE_PLAYER, COMPUTER}  Winner;
typedef enum {FALL, WIN, LOSE} Sound;
typedef enum {HH, HC, DATA} Mode;

enum point_value {E, B, W};
struct Point
{
    point_value value;
    int i;
    int j;
};
Point *g_chess;

bool is_fall(Point *cur);
bool ExtractResource(LPCTSTR strDstFile, LPCTSTR strResType, LPCTSTR strResName);
void music(Sound cur);
void initial_chess(Point *chess);
void draw_board(void);
void draw_pieces(Point *cur, const int steps);
void play(Point *chess, Mode *game_mode);
void computer(Point *chess, Point **cur);
int evaluation(Point *cur);
int alphabeta(int depth, int alpha, int beta, int *pi, int *pj);
int score (int m, int n, int k, Point *chess);
void show_winner(Winner result, Mode a);
Winner judge_winner(Point *cur, Mode a);

int main(void)
{
    Point chess_piece[15][15];
    g_chess = &chess_piece[0][0];
    Mode game_mode = HH;
    IMAGE board;

    initgraph(800, 600);
    initial_chess(&chess_piece[0][0]);
//	 loadimage(&board, "JPG", "BOARD");
//	 putimage(0, 0, &board);
    draw_board();
    play(&chess_piece[0][0], &game_mode);
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
    int chess_i;
    int chess_j;

    for(chess_i = 0; chess_i < 15; chess_i++)
        for(chess_j = 0; chess_j < 15; chess_j++)
        {
            ( *( chess + 15 * chess_i + chess_j ) ).i = chess_i;
            ( *( chess + 15 * chess_i + chess_j ) ).j = chess_j;
            ( *( chess + 15 * chess_i + chess_j ) ).value = E;
        }
}

/*
 *
 *
 *
 */
void draw_board(void)
{
    int i;

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
    bar(720, 100, 780, 140);
    setfont(30, 0, "宋体");
    settextcolor(GREEN);
    outtextxy(620, 100, "黑方");
    setfillcolor(BLACK);
    solidcircle(650, 160, 15);
    outtextxy(720, 100, "白方");

    setfont(15, 0, "黑体");
    settextcolor(BLACK);
    outtextxy(602, 560, "操作说明：");
    outtextxy(602, 580, "左击落子，右击退出");

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
void play(Point *chess, Mode *game_mode)
{
    MOUSEMSG my_mouse;
    Winner reslut;
    Point *cur = chess;
    bool is_win = FALSE;
    HWND wnd = GetHWnd();
    int mouse_i;
    int mouse_j;
    int steps = 0;

    if (MessageBox(wnd, "愚蠢的人类！你想与本电脑对战吗？\n<(︶︿︶)>我可以考虑让你下先手~", \
                   "游戏模式", MB_YESNO | MB_ICONQUESTION) == IDYES)
        *game_mode = HC;
    else
        *game_mode = HH;

    if(*game_mode == HC)
    {
        setfillcolor(BLUE);
        bar(620, 100, 680, 140);
        bar(720, 100, 780, 140);
        setfont(30, 0, "宋体");
        settextcolor(GREEN);
        outtextxy(620, 100, "玩家");
        outtextxy(720, 100, "电脑");
    }

    FlushMouseMsgBuffer();
    while(TRUE)
    {
        while(MouseHit() == TRUE)
        {
            my_mouse = GetMouseMsg();
            switch(my_mouse.uMsg)
            {
            case WM_LBUTTONDOWN:
                if(is_win == TRUE)
                    break;
                if((my_mouse.x - 20) % 40 >= 10 &&
                   (my_mouse.x - 20) % 40 <= 30 &&
                   (my_mouse.y - 20) % 40 >= 10 &&
                   (my_mouse.y - 20) % 40 <= 30 )
                   break;
                mouse_j = (int)NEAREST( (double)(my_mouse.x - 20) / (double)40 );
                mouse_i = (int)NEAREST( (double)(my_mouse.y - 20) / (double)40 );

                if(mouse_i >= 0 && mouse_i <= 14 && mouse_j >= 0 && mouse_j <= 14)
                {
                    cur = chess + 15 * mouse_i + mouse_j;
                    if(is_fall(cur) == TRUE)
                    {
                        steps++;
                        draw_pieces(cur, steps);
                        music(FALL);
                        if(steps % 2 == 0)
                        {
                            setfillcolor(LIGHTGRAY);
                            solidcircle(750, 160, 15);
                            setfillcolor(BLACK);
                            solidcircle(650, 160, 15);
                        }
                        else
                        {
                            setfillcolor(LIGHTGRAY);
                            solidcircle(650, 160, 15);
                            setfillcolor(WHITE);
                            solidcircle(750, 160, 15);
                        }

                        if(steps > 8)
                        {
                            reslut = judge_winner(cur, *game_mode);
                            if(reslut != EMPTY)
                            {
                                show_winner(reslut, *game_mode);
                                music(WIN);
                                is_win = TRUE;
                            }
                        }
                        if(*game_mode == HC && is_win == FALSE)
                        {
                            computer(chess, &cur);
                            steps++;
                            draw_pieces(cur, steps);
                            setfillcolor(LIGHTGRAY);
                            solidcircle(750, 160, 15);
                            setfillcolor(BLACK);
                            solidcircle(650, 160, 15);
                            if(steps > 8)
                            {
                                reslut = judge_winner(cur, *game_mode);
                                if(reslut == COMPUTER)
                                {
                                    music(LOSE);
                                    show_winner(reslut, *game_mode);
                                    is_win = TRUE;
                                }
                            }
                        }
                    }
                }
                break;
            case WM_RBUTTONDOWN:
                closegraph();
                exit(EXIT_SUCCESS);
            default:
                break;
            }
        }
    }
}

/*估值函数的完善
 *
 *
 *
 *
 */
void computer(Point *chess, Point **cur_ptr)
{
 /*
    int i, j;
      int score_c[15][15];
    int score_p[15][15];
      int x, s, t, max;

    // 极大极小值搜索
    // 初始化分数score_c[i][j]:电脑分数 score_p[i][j]:玩家分数
      for(i = 0; i < 15; i++)
      {
         for(j = 0; j < 15; j++)
         {
            score_c[i][j] = 0;
            score_p[i][j] = 0;
         }
      }
      for(i = 0; i < 15; i++)
      {
         for(j = 0; j < 15; j++)
         {
            if((chess + 15 * i + j)->value == E)
            {
               (chess + 15 * i + j)->value = B;
               x = score(i, j, 1, chess);
               score_p[i][j] += x; // 记录如果玩家下在此处，得到多少分
               (chess + 15 * i + j)->value = W;
               x = score(i, j, -1, chess);
               score_c[i][j] += x; // 记录如果电脑下在此处，得到多少分
               (chess + 15 * i + j)->value = E;
             }
         }
      }
      // 找到能下棋的空位置中，假设电脑和人下在此处，得到分数中最大值
      for(s = t = i = 0, max = score_c[0][0]; i < 15; i++)
      {
         for(j = 0; j < 15; j++)
         {
            if(score_c[i][j] > max)
            {
               max = score_c[i][j];
               s = i;
               t = j;
            }
         }
      }
      for(i = 0; i < 15; i++)
      {
         for(j = 0; j < 15; j++)
         {
            if(score_p[i][j] > max)
            {
               max = score_p[i][j];
               s = i;
               t = j;
            }
         }
      }
    *cur_ptr = (chess + 15 * s + t);
      (chess + 15 * s + t)->value = W; // 在最高分处落棋
*/
#ifdef DEBUG
   int temp;
   int i = 4, j = 4;

        temp = alphabeta(3, -9999, 9999, &i, &j);
        *cur_ptr = (g_chess + 15 * i + j);
        (*cur_ptr)->value = W;
#endif
         music(FALL);
    setfillcolor(LIGHTGRAY);
    solidcircle(650, 160, 15);
    setfillcolor(WHITE);
    solidcircle(750, 160, 15);
}

/*
 *
 *
 *
 */
int alphabeta(int depth, int alpha, int beta, int *pi, int *pj)
{
        /*********************************************************************
    int value;
    int i, j;
    int x, y;
    int max = -9001;
    int max_i, max_j;

         if(depth <= 0)
       return score(*pi, *pj, 1, g_chess);//胜负已分或到达叶子,返回估值
      if(depth % 2 == 0)//此句用于判断当前节点是何种节点
                {  //是取极小值的节点
       for(i = 0; i < 15; i++)
       {
          for(j = 0; j < 15; j++)
          {
             if((g_chess + 15 * i + j)->value == E)
             {
                                         Point *temp;

                temp = g_chess + 15 * i + j;//生成新节点
                temp->value = B;
//               value = alphabeta(depth - 1, alpha, alpha + 1, pi, pj);//递归搜索子节点
//               if (value > alpha)
//                   value = alphabeta(depth - 1, value, beta, pi, pj);
                                         *pi = i;
                                         *pj = j;
                value = -alphabeta(depth - 1, -beta, -alpha, pi, pj);
                                         temp->value = E; //撤销搜索过的节点
                if(value < beta)
                {
                   beta = value;//取极小值
                   if(alpha >= beta)
                      return alpha;//剪枝,抛弃后继节点
                }
             }
                         }
        }
        return beta;//返回最小值
         }
    else
    {   //取极大值的节点
       for(i = 0; i < 15; i++)
                 {
           for(j = 0; j < 15; j++)
           {
              if((g_chess + 15 * i + j)->value == E)
              {
                 Point *temp;

                  temp = g_chess + 15 * i + j;//生成新节点
                  temp->value = W;
//                  value = alphabeta(depth - 1, beta - 1, beta, pi, pj);//递归搜索子节点
//                 if (value > alpha)
//                     value = alphabeta(depth - 1, alpha, value, pi, pj);
                                                *pi = i;
                                                *pj = j;
                  value = alphabeta(depth - 1, alpha, beta, pi, pj);
                                                temp->value = E; //撤销搜索过的节点
                                                if(depth == 3)
                                                {
                                                        if(value > max)
                                                        {
                                                                max = value;
                                                                max_i = i;
                                                                max_j = j;
                                                        }
                                                        *pi = max_i;
                                                        *pj = max_j;
                                                }
                 if(value > alpha)
                 {
                    alpha = value;//取极大值
                    if(alpha >= beta)
                       return beta;//剪枝,抛弃后继节点
                                                }
                                  }
                          }
        }
        return alpha;//返回最大值
    }
         *************************************************************/
   int value;
        int i, j;
   int max = -10000;
   int max_i = 5, max_j = 5;

        if(depth <= 0)
      return score(*pi, *pj, 1, g_chess);//胜负已分或到达叶子,返回估值
        for(i = 0; i < 15; i++)
        {
                for(j = 0; j < 15; j++)
                {
                        if((g_chess + 15 * i + j)->value == E)
                        {
                                Point *temp = g_chess + 15 + i + j;
                                temp->value = (depth % 2 == 0) ? B : W;
                                *pi = i;
                                *pj = j;
                                value = -alphabeta(depth - 1, -beta, -alpha, pi, pj);
                                *pi = i;
                                *pj = j;
                                temp->value = E;
                                /*if(depth == 3)
                                {
                                        if(value > max)
                                        {
                                                max = value;
                                           max_i = i;
                                           max_j = j;
                                        }
                                }*/
            if(value > max)
                                {
                                        max = value;
                                        if(depth == 3)
                                        {
                                                max_i = i;
                                                max_j = j;
                                        }
                                }
                                if(max > alpha)
               alpha = max;
                                if(max >= beta)
                                        break;
                        }
                }
        }
        if(depth == 3)
        {
                *pi = max_i;
                *pj = max_j;
        }
        return max;
}

/*
 *
 *
 *
 *
 *
int evaluation(Point *cur)
{
}*/
/*贪心算法
 *
 *
 */
int score (int m, int n, int k, Point *chess)
{
    int i, j, p = 0, q = 0, b[4] = {0}, x = 0, shu, heng, zuoxie, youxie;
    point_value s;

    if (k == 1)
        s = B;
    else
        s = W;
    for(i = m; i < m + 5, i < 15; i++)
    {
        if((chess + 15 * i + n)->value != s)
        {
            if((chess + 15 * i + n)->value == 0)
            {
                b[0]++;
            }
            break;
        }
        else
            p++;
    }
    for(i = m - 1; i > m - 5, i >= 0; i--)
    {
        if((chess + 15 * i + n)->value != s)
        {
            if((chess + 15 * i + n)->value == 0)
            {
                b[0]++;
            }
            break;
        }
        else
            q++;
    }
    heng = p + q;
    for(j = n, p = 0; j < n + 5, j < 15; j++)
    {
        if((chess + 15 * m + j)->value != s)
        {
            if((chess + 15 * m + j)->value == 0)
            {
                b[1]++;
            }
            break;
        }
        else
            p++;
    }
    for(j = n - 1, q = 0; j > n - 5, j >= 0; j--)
    {
        if((chess + 15 * m + j)->value != s)
        {
            if((chess + 15 * i + n)->value == 0)
            {
                b[1]++;
            }
            break;
        }
        else
            q++;
    }
    shu = p + q;
    for(i = m, j = n, p = 0; i < 15, i < m + 5, j < 15; i++, j++)
    {
        if((chess + 15 * i + j)->value != s)
        {
            if((chess + 15 * i + j)->value == 0)
            {
                b[2]++;
            }
            break;
        }
        else
            p++;
    }
    for(i = m - 1, j = n - 1, q = 0; i >= 0, i > m - 5, j >= 0; i--, j--)
    {
        if((chess + 15 * i + j)->value != s)
        {
            if((chess + 15 * i + j)->value == 0)
            {
                b[2]++;
            }
            break;
        }
        else
            q++;
    }
    zuoxie = p + q;
    for(i = m, j = n, p = 0; i > 0, i > m - 5, j < 15; i--, j++)
    {
        if((chess + 15 * i + j)->value != s)
        {
            if((chess + 15 * i + j)->value == 0)
            {
                b[3]++;
            }
            break;
        }
        else
            p++;
    }
    for(i = m + 1, j = n - 1, q = 0; i < 15, i < m + 5, j >= 0; i++, j--)
    {
        if((chess + 15 * i + j)->value != s)
        {
            if((chess + 15 * i + j)->value == 0)
            {
                b[3]++;
            }
            break;
        }
        else
            q++;
    }
    youxie = p + q;
    if(heng > 4 || shu > 4 || zuoxie > 4 || youxie > 4)
    {
        x = 100;
    }
    else
    {
        for(i = 0; i < 4; i++)
        {
            if(b[i] == 0)
            {
                b[i] = -20;
            }
        }
        x = heng + b[0];
        if(shu + b[1] > x)
            x = shu + b[1];
        if(zuoxie + b[2] > x)
            x = zuoxie + b[2];
        if(youxie + b[3] > x)
            x = youxie + b[3];
    }
    return x;
}

/*
 *
 *
 *
 *
 */
bool is_fall(Point *cur)
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
Winner judge_winner(Point *cur, Mode a)
{
    int part_1 = 0;
    int part_2 = 0;
    Winner result = EMPTY;
    Point *temp;

    if(a == HC)
    {
        if(cur->value == B)
            result = BLACK_PLAYER;
        else if(cur->value == W)
            result = COMPUTER;
        else
        {
            fprintf(stderr, "\nReslut error in judge_winner function!");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        if(cur->value == B)
            result = BLACK_PLAYER;
        else if(cur->value == W)
            result = WHITE_PLAYER;
        else
        {
            fprintf(stderr, "\nReslut error in judge_winner function!");
            exit(EXIT_FAILURE);
        }
    }

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
            temp->value == cur->value && temp->i >= 0 && temp->i <= 14 && temp->j >= 0 && temp->j <= 14;
            temp -= 16)
        part_1++;
    for(temp = cur;
            temp->value == cur->value && temp->i >= 0 && temp->i <= 14 && temp->j >= 0 && temp->j <= 14;
            temp += 16)
        part_2++;
    if(part_1 + part_2 == 6)
        return result;

    part_1 = 0;
    part_2 = 0;
    for(temp = cur;
            temp->value == cur->value && temp->i >= 0 && temp->i <= 14 && temp->j >= 0 && temp->j <= 14;
            temp -= 14)
        part_1++;
    for(temp = cur;
            temp->value == cur->value && temp->i >= 0 && temp->i <= 14 && temp->j >= 0 && temp->j <= 14;
            temp += 14)
        part_2++;
    if(part_1 + part_2 == 6)
        return result;

    return EMPTY;
}

/*
 *
 *
 *
 *
 *
 */
void show_winner(Winner result, Mode a)
{
    HWND wnd = GetHWnd();

    switch(result)
    {
    case BLACK_PLAYER:
        if(a == HH)
            MessageBox(wnd, "恭喜黑方玩家获胜！", "本局结果", MB_OK | MB_ICONWARNING);
        else
            MessageBox(wnd, "居然能下赢我！\n我一定会回来的！", "本局结果", MB_OK | MB_ICONWARNING);
        break;
    case WHITE_PLAYER:
        MessageBox(wnd, "恭喜白方玩家获胜！", "本局结果", MB_OK | MB_ICONWARNING);
        break;
    case COMPUTER:
        MessageBox(wnd, "愚蠢的人类\n简直不堪一击！", "本局结果", MB_OK | MB_ICONWARNING);
    default:
        fprintf(stderr, "Result error!");
        exit(EXIT_FAILURE);
        break;
    }
}

//////////////////////////////////////////////////////////////////
// 提取指定模块中的资源文件
// 参数：
//		strDstFile:		目标文件名。提取的资源将保存在这里；
//		strResType:		资源类型；
//		strResName:		资源名称；
// 返回值：
//		true: 执行成功；
//		false: 执行失败。
bool ExtractResource(LPCTSTR strDstFile, LPCTSTR strResType, LPCTSTR strResName)
{
    // 创建文件
    HANDLE hFile = ::CreateFile(strDstFile, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return false;

    // 查找资源文件中、加载资源到内存、得到资源大小
    HRSRC	hRes	= ::FindResource(NULL, strResName, strResType);
    HGLOBAL	hMem	= ::LoadResource(NULL, hRes);
    DWORD	dwSize	= ::SizeofResource(NULL, hRes);

    // 写入文件
    DWORD dwWrite = 0;  	// 返回写入字节
    ::WriteFile(hFile, hMem, dwSize, &dwWrite, NULL);
    ::CloseHandle(hFile);

    return true;
}

void music(Sound cur)
{
    // 产生临时文件的文件名
    TCHAR tmpmp3[_MAX_PATH];
    ::GetTempPath(_MAX_PATH, tmpmp3);
    _tcscat(tmpmp3, _T("testapp_background.mp3"));		// VC2008、VC2010 请用 _tcscat_s 函数
    // 将 MP3 资源提取为临时文件，标签名带引号，且名称与资源名一致，不带后缀

    switch(cur)
    {
    case FALL:
        ExtractResource(tmpmp3, _T("MP3"), _T("fall"));
        // 注：如果 MP3 资源的 ID 为宏 IDR_BACKGROUND，这样做：
        // ExtractResource(tmpmp3, _T("MP3"), MAKEINTRESOURCE(IDR_BACKGROUND));
        break;
    case WIN:
        ExtractResource(tmpmp3, _T("MP3"), _T("win"));
        break;
    case LOSE:
        ExtractResource(tmpmp3, _T("MP3"), _T("lose"));
        break;
   default:
        fprintf(stderr, "sound command error!");
        exit(EXIT_FAILURE);
    }

    // 打开音乐
    TCHAR mcicmd[300];
    _stprintf(mcicmd, _T("open \"%s\" alias mymusic"), tmpmp3);		// VC2008、VC2010 请用 _stprintf_s 函数
    mciSendString(mcicmd, NULL, 0, NULL);
    // 播放音乐
    mciSendString(_T("play mymusic"), NULL, 0, NULL);
    if(cur == FALL)
        Sleep(300);
    else if(cur == WIN)
        Sleep(4000);
    else
        Sleep(1500);
    mciSendString(_T("close mymusic"), NULL, 0, NULL);
    // 删除临时文件
    DeleteFile(tmpmp3);
}
