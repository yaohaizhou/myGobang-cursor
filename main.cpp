/*
 * @Descripttion: main����������Boardͷ�ļ���������Board����ĳ�Ա��������
 * @version: ���������������ʽ�����汾������ʽ����+alpha-beta��֦+����Сֵ�㷨��
 * @Author: ��ҫ�� u201811260@hust.edu.cn
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

