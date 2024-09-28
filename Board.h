/*
 * @Descripttion: Board头文件，宏定义，调用库，定义成员函数和变量
 * @version:
 * 五子棋第三天启发式搜索版本（启发式搜索+alpha-beta剪枝+极大极小值算法）
 * @Author: 周耀海 u201811260@hust.edu.cn
 * @LastEditTime: 2020-02-16 09:07:01
 */
#ifndef BOARD_H_INCLUDED
#define BOARD_H_INCLUDED
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <array>
#include <iostream>
#include <vector>
///下棋模式
/// 1为开启debug，0为关闭debug，开启debug会在棋局最后显示棋盘各点的评估值
#define DEBUG 0
/// 1为人类执黑先手，0为AI执黑先手
#define FIRST 0
/// 1为打开二层剪枝操作，0为关闭
#define TURN_ON_CUT2 1
/// 1为打开三层剪枝操作，0为关闭，建议两层剪枝都打开
#define TURN_ON_CUT3 1
/// 1为打开启发式搜索，0为关闭，视情况打开
#define HEURISTIC 1
/// AI Intelligence
#define AI_MAX_CHOICE 20 /// AI在启发式搜索时每一步考虑的最大位置数量

///棋盘样式
static constexpr int BOARD_SIZE = 15;
enum class CellState { Empty = 0, Black = 1, White = 2, WALL = 3 };
enum class Player { None = 0, AI = 1, Human = 2 };

class Board {
public:
  explicit Board(sf::RenderWindow *window);
  ~Board() = default;
  void play();
  void checkEnd();
  bool isGameOver() { return game_over; }
  void setGameOver(Player winning_player);

  void AI_1_MAX();
  int AI_2_MIN(int alpha);
  int AI_3_MAX(int alpha, int beta);

  void printBoard();
  void drawEndGameMessage(sf::RenderWindow &window);

  int getRow();
  int getCol();
  void out(int, int);
  void player1();
  void player2();

private:
  struct Cell {
    int row;
    int col;
    int score;

    bool operator<(const Cell &other) {
      return score > other.score; // For descending order
    }
  };

  std::array<std::array<CellState, BOARD_SIZE + 2>, BOARD_SIZE + 2> chess;
  typedef struct order {
    int orderi;
    int orderj;
    int orderpoint;
  } Order;
  Order orderSort[BOARD_SIZE * BOARD_SIZE];

  static bool cmp(Order, Order);

  int row, col;
  Player current_player;
  bool is_end = true;
  bool game_over;
  int distance;
  Player winner;
  sf::RenderWindow *windowPtr;

  static const int dx[8];
  static const int dy[8];

  bool checkAvailable(int x, int y);
  bool checkSame(int x, int y);
  int sameSum(int x, int y, int direction);
  int calculate(int x, int y);
  int getScore(int x, int y);
  void getMouseLoc();

  int sumMakeFive(int x, int y);
  int sumLiveFour(int x, int y);
  int sumLiveThree(int x, int y);
  int sumMakeFour(int x, int y);
  int sumFive(int x, int y);

  std::vector<Cell> getTopMoves(int num_moves);
};

#endif // BOARD_H_INCLUDED
