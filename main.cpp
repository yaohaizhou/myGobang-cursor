/*
 * @Descripttion: main函数，调用Board头文件，程序以Board类里的成员函数进行
 * @version: 五子棋第三天启发式搜索版本（启发式搜索+alpha-beta剪枝+极大极小值算法）
 * @Author: 周耀海 u201811260@hust.edu.cn
 * @LastEditTime: 2020-02-16 08:58:47
 */
#include "Board.h"
using namespace std;

int main()
{
    sf::RenderWindow window(sf::VideoMode(1000, 500), "Gomoku");
    Board board(&window);
    board.printBoard();

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        board.player1();
        board.player2();
    }

    return 0;
}

