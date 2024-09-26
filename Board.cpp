/*
 * @Descripttion: Board函数实现
 * @version:
 * 五子棋第三天启发式搜索版本（启发式搜索+alpha-beta剪枝+极大极小值算法）
 * @Author: 周耀海 u201811260@hust.edu.cn
 * @LastEditTime: 2020-02-16 09:48:29
 */
#include "Board.h"

const int Board::dx[8] = {-1, -1, -1, 0, 1, 1, 1, 0};
const int Board::dy[8] = {-1, 0, 1, 1, 1, 0, -1, -1};

/**
 * @name: Board
 * @msg: 构造函数，用于初始化参数
 * @param none
 * @return: void
 */
Board::Board(sf::RenderWindow *window) : windowPtr(window) {
  int i, j;
  for (i = 1; i < BOARD_SIZE + 1; i++)
    for (j = 1; j < BOARD_SIZE + 1; j++)
      chess[i][j] = CellState::Empty; ///初始化空地
  for (i = 0; i < BOARD_SIZE + 2; i++) {
    chess[i][0] = CellState::WALL; ///初始化墙
    chess[i][BOARD_SIZE + 1] = CellState::WALL;
  }
  for (j = 0; j < BOARD_SIZE + 2; j++) {
    chess[0][j] = CellState::WALL;
    chess[BOARD_SIZE + 1][j] = CellState::WALL;
  }
  if (DEBUG) {
    ///测试用五连+活四
    chess[8][8] = CellState::Black;
    chess[9][8] = CellState::Black;
    chess[10][8] = CellState::Black;
    chess[11][8] = CellState::Black;
    chess[8][9] = CellState::White;
    chess[9][9] = CellState::White;
    chess[10][9] = CellState::White;
    chess[11][9] = CellState::White;
    ///测试用冲四
    chess[8][1] = CellState::Black;
    chess[9][1] = CellState::Black;
    chess[10][1] = CellState::Black;
    chess[7][1] = CellState::White;
    ///测试用活四
    chess[8][4] = CellState::Black;
    chess[9][4] = CellState::Black;
    chess[11][4] = CellState::Black;
    ///测试用五连
    chess[8][12] = CellState::Black;
    chess[9][12] = CellState::Black;
    chess[10][12] = CellState::Black;
    chess[11][12] = CellState::Black;
  }
  is_end = true;
  game_over = false;
  winner = Player::None;

  for (int i = 0; i < AI_MAX_CHOICE; ++i) {
    orderSort[i] = {0, 0, 0}; // Initialize all fields to 0
  }
}

void Board::setGameOver(Player winning_player) {
  game_over = true;
  winner = winning_player;
}

void Board::drawEndGameMessage(sf::RenderWindow &window) {
  // Create a semi-transparent background
  sf::RectangleShape background(sf::Vector2f(300, 100));
  background.setFillColor(sf::Color(0, 0, 0, 200)); // Black with 200/255 alpha
  background.setPosition(120, 220); // Centered on a 540x540 window

  // Create the message text using basic shapes
  sf::Text message;
  if (winner == Player::Human) {
    message.setString("Human Wins!");
    message.setFillColor(sf::Color::White);
  } else if (winner == Player::AI) {
    message.setString("AI Wins!");
    message.setFillColor(sf::Color::White);
  } else {
    message.setString("Game Over!");
    message.setFillColor(sf::Color::Yellow);
  }

  // Load the font
  sf::Font font;
  if (!font.loadFromFile("font/arial.ttf")) {
    // Handle font loading error
    std::cerr << "Error loading font" << std::endl;
    return;
  }

  // Set the font for the message
  message.setFont(font);
  message.setCharacterSize(24); // Set font size

  // Position the message
  message.setPosition(170, 250); // Adjust as needed

  // Draw the background and message
  window.draw(background);
  window.draw(message);
}

void Board::checkEnd() {
  for (int u = 0; u < 4; u++) {
    if ((sameSum(u, getRow(), getCol()) + sameSum(u + 4, getRow(), getCol())) >=
        4) {
      setGameOver(current_player);
      is_end = false;
      return;
    }
  }
}

/**
 * @name: printBoard
 * @msg: 用于打印棋盘
 * @param none
 * @return: void
 */
void Board::printBoard() {
  sf::RenderWindow &window = *static_cast<sf::RenderWindow *>(windowPtr);
  window.clear(sf::Color(173, 92, 45));

  for (int i = 60; i < 510; i += 30) {
    sf::Vertex line[] = {sf::Vertex(sf::Vector2f(i, 60)),
                         sf::Vertex(sf::Vector2f(i, 480))};
    window.draw(line, 2, sf::Lines);

    sf::Vertex hline[] = {sf::Vertex(sf::Vector2f(60, i)),
                          sf::Vertex(sf::Vector2f(480, i))};
    window.draw(hline, 2, sf::Lines);
  }

  for (int i = 60; i < 510; i += 30) {
    for (int j = 60; j < 510; j += 30) {
      out(i, j);
    }
  }

  if (game_over) {
    drawEndGameMessage(window);
  }

  window.display();
}
/**
 * @name: out
 * @msg: 分类别打印棋子和格子
 * @param {int} i {int} j 代表对应点的行和列
 * @return: void
 */
void Board::out(int i, int j) {
  sf::RenderWindow &window = *static_cast<sf::RenderWindow *>(windowPtr);
  if (chess[i / 30 - 1][j / 30 - 1] == CellState::Black) {
    sf::CircleShape circle(15);
    circle.setPosition(i - 15, j - 15);
    circle.setFillColor(sf::Color::Black);
    window.draw(circle);
  }
  if (chess[i / 30 - 1][j / 30 - 1] == CellState::White) {
    sf::CircleShape circle(15);
    circle.setPosition(i - 15, j - 15);
    circle.setFillColor(sf::Color::White);
    window.draw(circle);
  }
}
/**
 * @name: player1
 * @msg: 玩家一下棋操作，先下棋再打印棋盘，判断是否胜利
 * @param none
 * @return: void
 */
void Board::player1() {
  if (game_over)
    return;
  current_player = Player::AI;
  if (FIRST) {
    getMouseLoc();
  } else {
    AI_1_MAX();
  }
  chess[row][col] = CellState::Black;
  printBoard();
  checkEnd();
}
/**
 * @name: player2
 * @msg: 玩家二下棋操作，先下棋再打印棋盘，判断是否胜利
 * @param none
 * @return: void
 */
void Board::player2() {
  if (game_over)
    return;
  current_player = Player::Human;
  if (FIRST) {
    AI_1_MAX();
  } else {
    getMouseLoc();
  }
  chess[row][col] = CellState::White;
  printBoard();
  checkEnd();
}
/**
 * @name: checkSame
 * @msg: 检查是否相同
 * @param {int} row {int} col 传入的行和列，非全局变量行和列
 * @return: bool 相同则返回true，不同返回false
 */
bool Board::checkSame(int row, int col) {
  return (chess[row][col] == static_cast<CellState>(current_player));
}
/**
 * @name: sameSum
 * @msg: 检查在一个方向上相同棋子个数
 * @param {int} u 传入方向 {int} row {int} col 传入的行和列
 * @return: {int} sum 总数
 */
int Board::sameSum(int u, int row, int col) {
  int sum = 0;
  int i;
  for (i = 1; checkSame(row + i * dx[u], col + i * dy[u]); i++) {
    sum++;
  }
  distance = i;
  return sum;
}
/**
 * @name: checkAvailable
 * @msg: 检查是否可落子
 * @param {int} row {int} col  传入的行和列
 * @return: bool 可落子为true，不可落子为false
 */
bool Board::checkAvailable(int row, int col) {
  return (row >= 1 && row <= 15 && col >= 1 && col <= 15 &&
          chess[row][col] == CellState::Empty);
}
/// sumMakeFive要领
///在8个方向上求成五点，对于每个方向，从当前位置往前延伸，
///第一个不同色的点是空格，跳过空格继续往前延伸直到第二个不同色的点，
///同时从当前位置往反方向延伸直到第一个不同色的点， 算上当前位置本身，
///如果同色点的数量一共是4，那么该方向就有成五点。
/**
 * @name: sumMakeFive
 * @msg:
 * 成五点总数，用来计算冲四的个数，成五点形式为YabcdeY，其中abcde中4个1,1个0，所以考虑可以跳过一个空格然后4个1。但是不可跳过对方棋子或者墙壁
 * @param {int} row {int} col  传入的行和列
 * @return: {int} sum 总数
 */
int Board::sumMakeFive(int row, int col) {
  int sum = 0, i, u;

  for (u = 0; u < 8; u++) {
    bool flag = true;
    int sumk = 0;
    for (i = 1; checkSame(row + i * dx[u], col + i * dy[u]) || flag; i++) {
      if (!checkSame(row + i * dx[u], col + i * dy[u])) {
        if (chess[row + i * dx[u]][col + i * dy[u]] != CellState::Empty) {
          sumk -= 10;
        }
        flag = false;
      }
      sumk++;
    }
    for (i = -1; checkSame(row + i * dx[u], col + i * dy[u]); i--) {
      sumk++;
    }
    if (sumk == 4)
      sum++;
  }

  return sum;
}
/**
 * @name: sumLiveFour
 * @msg: 活四总数
 * @param {int} row {int} col  传入的行和列
 * @return: {int} sum 总数
 */
int Board::sumLiveFour(int row, int col) {
  int sum = 0, u;
  for (u = 0; u < 4; u++) {
    if ((sameSum(u + 4, row, col) + sameSum(u, row, col)) >= 3)
    ///记念一下，前后顺序不一样导致distance不一样
    ///(sameSum(u,row,col) + sameSum(u+4,row,col)
    ///原本写的是这样的，后面的函数后运行，使用的distance就是后面那个
    ///所以活四一直没有匹配上
    {
      if (chess[row + distance * dx[u]][col + distance * dy[u]] ==
              CellState::Empty &&
          chess[row + (distance - 5) * dx[u]][col + (distance - 5) * dy[u]] ==
              CellState::Empty)
        sum++;
    }
  }
  return sum;
}
/**
 * @name: sumMakeFour
 * @msg: 冲四总数=五点总数-2*活四总数
 * @param {int} row {int} col  传入的行和列
 * @return: {int} sum 总数
 */
int Board::sumMakeFour(int row, int col) {
  return sumMakeFive(row, col) - 2 * sumLiveFour(row, col);
}
/**
 * @name: sumLiveThree
 * @msg: 活三总数
 * @param {int} row {int} col  传入的行和列
 * @return: {int} sum 总数
 */
int Board::sumLiveThree(int row, int col) {
  int u, sum = 0;
  for (u = 0; u < 4; u++) {
    if ((sameSum(u + 4, row, col) + sameSum(u, row, col)) >= 2) {
      if ((chess[row + distance * dx[u]][col + distance * dy[u]] ==
               CellState::Empty &&
           chess[row + (distance - 4) * dx[u]][col + (distance - 4) * dy[u]] ==
               CellState::Empty &&
           chess[row + (distance - 5) * dx[u]][col + (distance - 5) * dy[u]] ==
               CellState::Empty) ||
          (chess[row + distance * dx[u]][col + distance * dy[u]] ==
               CellState::Empty &&
           chess[row + (distance + 1) * dx[u]][col + (distance + 1) * dy[u]] ==
               CellState::Empty &&
           chess[row + (distance - 4) * dx[u]][col + (distance - 4) * dy[u]] ==
               CellState::Empty))
        sum++;
    }
  }
  for (u = 0; u < 8; u++) {
    int sumk = 0, i;
    bool flag = true;
    for (i = 1; checkSame(row + i * dx[u], col + i * dy[u]) || flag; i++) {
      if (!checkSame(row + i * dx[u], col + i * dy[u])) {
        if (chess[row + i * dx[u]][col + i * dy[u]] != CellState::Empty) {
          sumk -= 10;
        }
        flag = false;
      }
      sumk++;
    }
    if (sumk == 3)
      sum++;
  }
  return sum;
}
/**
 * @name: calculate
 * @msg:
 * 计算分值，如果这个点有棋子记0分，如果五连则50000分，如果活四则4320分，如果活三或者冲四则720分
 * @param {int} row {int} col  传入的行和列
 * @return: {int} point 打分值
 */
int Board::calculate(int row, int col) ///
{
  if (chess[row][col] != CellState::Empty)
    return 0;
  int point = 0;
  for (int u = 0; u < 8; u++)
    if (chess[row + dx[u]][col + dy[u]] != CellState::Empty &&
        chess[row + dx[u]][col + dy[u]] != CellState::WALL)
      point++;
  if (sumFive(row, col))
    return 50000;
  point += 4320 * sumLiveFour(row, col) +
           720 * (sumMakeFive(row, col) + sumLiveThree(row, col));
  return point;
}
/**
 * @name: sumFive
 * @msg: 五连总数
 * @param {int} row {int} col  传入的行和列
 * @return: {int} sum 总数
 */
int Board::sumFive(int row, int col) {
  int sum = 0;
  for (int u = 0; u < 4; u++) {
    if ((sameSum(u, row, col) + sameSum(u + 4, row, col)) >= 4)
      sum++;
  }
  return sum;
}
/**
 * @name: getRow
 * @msg: 获取行
 * @param none
 * @return: {int} row 全局变量行
 */
int Board::getRow() { return row; }
/**
 * @name: getCol
 * @msg: 获取列
 * @param {type}
 * @return: {int} col 全局变量列
 */
int Board::getCol() { return col; }
/**
 * @name: getMouseLoc
 * @msg: Get mouse position and map it to the real chessboard. The chessboard
 * starts at (60, 60) and has lines every 30 pixels.
 * @param none
 * @return: void
 */
void Board::getMouseLoc() {
  sf::RenderWindow &window = *static_cast<sf::RenderWindow *>(windowPtr);
  sf::Vector2i mousePosition;
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::MouseButtonPressed) {
        mousePosition = sf::Mouse::getPosition(window);
        int mousex = mousePosition.x;
        int mousey = mousePosition.y;

        // Round to nearest grid point
        mousex = ((mousex - 60 + 15) / 30) * 30 + 60;
        mousey = ((mousey - 60 + 15) / 30) * 30 + 60;

        if (checkAvailable(mousex / 30 - 1, mousey / 30 - 1)) {
          row = mousex / 30 - 1;
          col = mousey / 30 - 1;
          return;
        }
      }
    }
  }
}
/**
 * @name: getScore
 * @msg:
 * 计算当前点打分值，但是是以某一方的当前点利益为参考的，其实更好的方法是scoreAI-scoreHuman得出对于AI最有利，对于人类最不利的点
 * @param {int} row {int} col  传入的行和列
 * @return: {int} calculate(row,col) 当前点打分值
 */
int Board::getScore(int row, int col) { return calculate(row, col); }
/**
 * @name: AI_1_MAX
 * @msg:
 * 第一层若采用启发式搜素，会对225个点进行一个排序，得分高的优先进行第二层的运算，相当于修改第二层的顺序，有利于剪枝
 * 若不用启发式搜索，就对225个点依次调用二层运算
 * 第一层代表AI的利益，所以取极大值
 * @param none
 * @return: void
 */
void Board::AI_1_MAX() {
  if (chess[8][8] == CellState::Empty) {
    row = 8;
    col = 8;
    return;
  }
  int i, j, score = -100000, temp = 0;

  if (HEURISTIC) {
    ///启发式搜索
    for (i = 1; i < BOARD_SIZE + 1; i++) {
      for (j = 1; j < BOARD_SIZE + 1; j++) {
        orderSort[(i - 1) * 15 + (j - 1)].orderi =
            i; ///给结构体成员赋值i/j/point
        orderSort[(i - 1) * 15 + (j - 1)].orderj = j;
        orderSort[(i - 1) * 15 + (j - 1)].orderpoint = getScore(i, j);
      }
    }
    std::sort(orderSort, orderSort + BOARD_SIZE * BOARD_SIZE, cmp);
    for (i = 0; i < AI_MAX_CHOICE; i++) {
      if (!checkAvailable(orderSort[i].orderi, orderSort[i].orderj))
        continue;
      if (orderSort[i].orderpoint == 0)
        continue;
      if (orderSort[i].orderpoint == 50000) {
        row = orderSort[i].orderi;
        col = orderSort[i].orderj;
        return;
      }
      chess[orderSort[i].orderi][orderSort[i].orderj] =
          static_cast<CellState>(current_player);
      temp = AI_2_MIN(score);
      chess[orderSort[i].orderi][orderSort[i].orderj] = CellState::Empty;
      if (temp > score) {
        score = temp;
        row = orderSort[i].orderi;
        col = orderSort[i].orderj;
      }
    }
  } else {
    for (i = 1; i < BOARD_SIZE + 1; i++) {
      for (j = 1; j < BOARD_SIZE + 1; j++) {
        if (!checkAvailable(i, j))
          continue; //此处可以直接加上checkAround
        temp = getScore(i, j);
        chess[i][j] = static_cast<CellState>(current_player);
        if (temp == 0) {
          chess[i][j] = CellState::Empty;
          continue;
        }
        if (temp == 50000) {
          row = i;
          col = j;
          return;
        }
        temp = AI_2_MIN(score);
        chess[i][j] = CellState::Empty;
        if (temp > score) {
          score = temp;
          row = i;
          col = j;
        } ///第一层取极大值
      }
    }
  }

  return;
}
/**
 * @name: AI_2_MIN
 * @msg: 模拟对手进行二层运算，取极小值
 * @param {int} score1
 * 从第一层下来的数据，这个数据维护第一层的极大值，所以如果第二层算出来的点比这个值小，说明就不需要再算了，因为AI不可能选这个方案了
 * @return: {int} score 维护第二层的极小值
 */
int Board::AI_2_MIN(int score1) {
  int score = 100000, temp = 0;
  for (int i = 1; i < BOARD_SIZE + 1; i++) {
    bool cut1 = false;
    for (int j = 1; j < BOARD_SIZE + 1; j++) {
      if (!checkAvailable(i, j))
        continue;
      int originalTurn = static_cast<int>(current_player);
      current_player = static_cast<Player>(3 - originalTurn);
      temp = getScore(i, j);
      current_player = static_cast<Player>(originalTurn);
      chess[i][j] = static_cast<CellState>(
          3 - static_cast<int>(current_player)); ///模拟对手下棋
      if (temp == 0) {
        chess[i][j] = CellState::Empty;
        continue;
      }
      if (temp == 50000) {
        chess[i][j] = CellState::Empty;
        return -50000;
      }
      temp = AI_3_MAX(score, temp);
      chess[i][j] = CellState::Empty;
      if (temp < score)
        score = temp; ///第二层取极小值
      if (TURN_ON_CUT2) {
        if (score1 > temp) {
          cut1 = true;
          break;
        }
      }
    }
    if (TURN_ON_CUT2) {
      if (cut1)
        break;
    }
  }
  return score;
}
/**
 * @name: AI_3_MAX
 * @msg:
 * 第三层是AI利益，取极大。如果第三层算出来的点比第二层的极小值要大，说明第三层这个点不可能使得第二层的极小值变小了，不需要再算了。
 * 其实极大极小值算法是上下两层数据斜对角的关系
 * @param {int} score2 从第二层传下来的维护极小值 {int} tempp
 * 第二层当前计算值，用于提高难度/速度
 * @return: {int} score 从第三层返回极大值
 */
int Board::AI_3_MAX(int score2, int tempp) {
  int score = -100000, temp = 0;
  for (int i = 1; i < BOARD_SIZE + 1; i++) {
    bool cut2 = false;
    for (int j = 1; j < BOARD_SIZE + 1; j++) {
      if (!checkAvailable(i, j))
        continue;
      temp = getScore(i, j);
      chess[i][j] = static_cast<CellState>(current_player);
      if (temp == 0) {
        chess[i][j] = CellState::Empty;
        continue;
      }
      if (temp == 50000) {
        chess[i][j] = CellState::Empty;
        return 50000;
      }
      chess[i][j] = CellState::Empty;
      if (temp - tempp * 3 > score)
        score = temp - tempp * 3;
      if (TURN_ON_CUT3) {
        if (temp > score2) {
          cut2 = true;
          break;
        }
      }
    }
    if (TURN_ON_CUT3) {
      if (cut2)
        break;
    }
  }
  return score;
}
/**
 * @name: cmp
 * @msg: > 为降序 < 为升序，结构体数组按照成员orderpoint降序排列
 * @param {Order} x {Order} y 两个结构体
 * @return: bool
 */
bool Board::cmp(Order x, Order y) { return (x.orderpoint) > (y.orderpoint); }
