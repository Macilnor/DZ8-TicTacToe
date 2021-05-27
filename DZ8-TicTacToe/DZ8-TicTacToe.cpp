// DZ8-TicTacToe.cpp 
//

#include <stdlib.h>
#include <iostream>
#include <chrono>
#include <random>

using namespace std;

enum cell;
enum state;
struct field;
struct coord;

void clrScr();
int32_t getRandom(int32_t min, int32_t max);
inline void checkInput();
void startGame(field& f);
void endGame(field& f);
void drawField(const field& f);
coord getPlayerCoord(field& f);
coord getAICoord(field& f);
state checkWiner(field& f);

//====================================================================================================

enum cell {
    EMPTY = ' ',
    CROSS = 'X',
    ZERO = 'O'
};

enum state {
    IN_PROGRESS,
    WON_PLAYER,
    WON_AI,
    DRAW
};

struct field {
    cell** ppField = nullptr;
    size_t size = 0;
    size_t winline = 0;
    size_t turn = 0;
    cell player = EMPTY;
    cell ai = EMPTY;
    state progress = IN_PROGRESS;
};

struct coord {
    size_t x;
    size_t y;
};

//====================================================================================================

void clrScr()
{
    system("cls");
}

int32_t getRandom(int32_t min, int32_t max) 
{
    const static auto seed = chrono::system_clock::now().time_since_epoch().count();
    static mt19937_64 gen(seed);
    uniform_int_distribution<int32_t> dis(min, max);
    return dis(gen);
}

inline void checkInput() 
{
    if (cin.fail())
    {
        cin.clear();
        cin.ignore(32767, '\n');
        cout << "Wrong input! ";
    }
    else
    {
        cin.ignore(32767, '\n');
    }
}

void startGame(field& f) 
{
    do
    {
        cout << "Enter field size (3-5): ";
        cin >> f.size;
        checkInput();

    } while (f.size < 3 || f.size > 5);

    do
    {
        cout << "Enter wining line size (3-5): ";
        cin >> f.winline;
        checkInput();

    } while (f.winline < 3 || f.winline > f.size);

    f.ppField = new cell * [f.size];
    for (size_t y = 0; y < f.size; y++)
    {
        f.ppField[y] = new cell[f.size];
    }

    for (size_t x = 0; x < f.size; x++)
    {
        for (size_t y = 0; y < f.size; y++)
        {
            f.ppField[x][y] = EMPTY;
        }
    }

    if (getRandom(0, 1000) > 500)
    {
        f.player = CROSS;
        f.ai = ZERO;
        f.turn = 0;
    }
    else
    {
        f.player = ZERO;
        f.ai = CROSS;
        f.turn = 1;
    }
}

void endGame(field& f) 
{
    for (size_t y = 0; y < f.size; y++)
    {
       delete[] f.ppField[y];
    }
    delete[] f.ppField;
}

void drawField(const field& f)
{
    cout << endl << "      ";
    for (size_t x = 0; x < f.size; x++)
    {
        cout << x + 1 << "    ";
    }
    cout << endl;
    for (size_t y = 0; y < f.size; y++)
    {
        cout << "  " << y + 1 << " [ ";
        for (size_t x = 0; x < f.size; x++)
        {
            string str = (x < f.size - 1) ? " ][ " : " ]";
            cout << static_cast<char>(f.ppField[y][x]) << str;
        }
        cout << endl;
    }
    cout << endl << "  Player : " << static_cast<char>(f.player) << endl << "  AI : " << static_cast<char>(f.ai) << endl;

}

coord getPlayerCoord(field& f)
{
    coord c{ 0 };
    do {
        cout << "Enter X coord: ";
        cin >> c.x;
        checkInput();
        cout << "Enter Y coord: ";
        cin >> c.y;
        checkInput();

    } while ((c.x < 1 || c.x > f.size) || (c.y < 1 || c.y > f.size) || (f.ppField[c.y - 1][c.x - 1] != EMPTY));

    c.x--;
    c.y--;

    return c;
}

coord getAICoord(field& f)
{
    coord buf[25];
    size_t num = 0;
    for (size_t y = 0; y < f.size; y++)
    {
        for (size_t x = 0; x < f.size; x++)
        {
            if (f.ppField[y][x] == EMPTY)
            {
                buf[num] = { x, y };
                num++;
            }
        }
    }
    if (num != 0)
    {
        for (size_t i = 0; i < num; i++)
        {
            f.ppField[buf[i].y][buf[i].x] = f.ai;
            if (checkWiner(f) == WON_AI)
            {
                f.ppField[buf[i].y][buf[i].x] = EMPTY;
                return buf[i];
            }
            f.ppField[buf[i].y][buf[i].x] = EMPTY;
        }
        
        for (size_t i = 0; i < num; i++)
        {
            f.ppField[buf[i].y][buf[i].x] = f.player;
            if (checkWiner(f) == WON_PLAYER)
            {
                f.ppField[buf[i].y][buf[i].x] = EMPTY;
                return buf[i];
            }
            f.ppField[buf[i].y][buf[i].x] = EMPTY;
        }
        size_t i = getRandom(0, 1000) % num;
        return buf[i];
    }
}

state checkWiner(field& f)
{
    size_t playerScore = 0;
    size_t aiScore = 0;
    cell prev = EMPTY;

    //Проверяем столбцы
    for (size_t y = 0; y < f.size; y++)
    {
        playerScore = 0;
        aiScore = 0;
        prev = EMPTY;
        for (size_t x = 0; x < f.size; x++)
        {
            if (f.ppField[y][x] == f.player && (prev == EMPTY || prev == f.player))
            {
                prev = f.player;
                playerScore++;
                if (playerScore >= f.winline)
                    return WON_PLAYER;
            }
            else if (f.ppField[y][x] == f.ai && (prev == EMPTY || prev == f.ai))
            {
                prev = f.ai;
                aiScore++;
                if (aiScore >= f.winline)
                    return WON_AI;
            }
            else if (f.ppField[y][x] == f.player && prev == f.ai)
            {
                prev = f.player;
                playerScore++;
                aiScore = 0;
            }
            else if (f.ppField[y][x] == f.ai && prev == f.player)
            {
                prev = f.ai;
                playerScore = 0;
                aiScore++;
            }
            else
            {
                prev = EMPTY;
                aiScore = 0;
                playerScore = 0;
            }
        }
    }

    //Проверяем строки
    for (size_t x = 0; x < f.size; x++)
    {
        playerScore = 0;
        aiScore = 0;
        prev = EMPTY;
        for (size_t y = 0; y < f.size; y++)
        {
            if (f.ppField[y][x] == f.player && (prev == EMPTY || prev == f.player))
            {
                prev = f.player;
                playerScore++;
                if (playerScore >= f.winline)
                    return WON_PLAYER;
            }
            else if (f.ppField[y][x] == f.ai && (prev == EMPTY || prev == f.ai))
            {
                prev = f.ai;
                aiScore++;
                if (aiScore >= f.winline)
                    return WON_AI;
            }
            else if (f.ppField[y][x] == f.player && prev == f.ai)
            {
                prev = f.player;
                playerScore++;
                aiScore = 0;
            }
            else if (f.ppField[y][x] == f.ai && prev == f.player)
            {
                prev = f.ai;
                playerScore = 0;
                aiScore++;
            }
            else
            {
                prev = EMPTY;
                aiScore = 0;
                playerScore = 0;
            }
        }
    }

    //Проверяем диагональ 1

    playerScore = 0;
    aiScore = 0;
    prev = EMPTY;
    for (size_t y = 0; y < f.size; y++)
    {
        if (f.ppField[y][y] == f.player && (prev == EMPTY || prev == f.player))
        {
            prev = f.player;
            playerScore++;
            if (playerScore >= f.winline)
                return WON_PLAYER;
        }
        else if (f.ppField[y][y] == f.ai && (prev == EMPTY || prev == f.ai))
        {
            prev = f.ai;
            aiScore++;
            if (aiScore >= f.winline)
                return WON_AI;
        }
        else if (f.ppField[y][y] == f.player && prev == f.ai)
        {
            prev = f.player;
            playerScore++;
            aiScore = 0;
        }
        else if (f.ppField[y][y] == f.ai && prev == f.player)
        {
            prev = f.ai;
            playerScore = 0;
            aiScore++;
        }
        else
        {
            prev = EMPTY;
            aiScore = 0;
            playerScore = 0;
        }
    }

    //Проверяем диагональ 2
    
    playerScore = 0;
    aiScore = 0;
    prev = EMPTY;
    for (size_t y = 0; y < f.size; y++)
    {
        if (f.ppField[y][f.size - y - 1] == f.player && (prev == EMPTY || prev == f.player))
        {
            prev = f.player;
            playerScore++;
            if (playerScore >= f.winline)
                return WON_PLAYER;
        }
        else if (f.ppField[y][f.size - y - 1] == f.ai && (prev == EMPTY || prev == f.ai))
        {
            prev = f.ai;
            aiScore++;
                if (aiScore >= f.winline)
                    return WON_AI;
        }
        else if (f.ppField[y][f.size - y - 1] == f.player && prev == f.ai)
        {
            prev = f.player;
            playerScore++;
            aiScore = 0;
        }
        else if (f.ppField[y][f.size - y - 1] == f.ai && prev == f.player)
        {
                prev = f.ai;
                playerScore = 0;
                aiScore++;
        }
        else
        {
            prev = EMPTY;
            aiScore = 0;
            playerScore = 0;
        }
    }
    
    //Проверяем доп. диагонали
    if (f.size - f.winline >= 1)
    {
        playerScore = 0;
        aiScore = 0;
        prev = EMPTY;
        for (size_t y = 1; y < f.size; y++)
        {
            if (f.ppField[y-1][y] == f.player && (prev == EMPTY || prev == f.player))
            {
                prev = f.player;
                playerScore++;
                if (playerScore >= f.winline)
                    return WON_PLAYER;
            }
            else if (f.ppField[y-1][y] == f.ai && (prev == EMPTY || prev == f.ai))
            {
                prev = f.ai;
                aiScore++;
                if (aiScore >= f.winline)
                    return WON_AI;
            }
            else if (f.ppField[y-1][y] == f.player && prev == f.ai)
            {
                prev = f.player;
                playerScore++;
                aiScore = 0;
            }
            else if (f.ppField[y-1][y] == f.ai && prev == f.player)
            {
                prev = f.ai;
                playerScore = 0;
                aiScore++;
            }
            else
            {
                prev = EMPTY;
                aiScore = 0;
                playerScore = 0;
            }
        }

        prev = EMPTY;
        aiScore = 0;
        playerScore = 0;
        for (size_t y = 1; y < f.size; y++)
        {
            if (f.ppField[y][y - 1] == f.player && (prev == EMPTY || prev == f.player))
            {
                prev = f.player;
                playerScore++;
                if (playerScore >= f.winline)
                    return WON_PLAYER;
            }
            else if (f.ppField[y][y - 1] == f.ai && (prev == EMPTY || prev == f.ai))
            {
                prev = f.ai;
                aiScore++;
                if (aiScore >= f.winline)
                    return WON_AI;
            }
            else if (f.ppField[y][y - 1] == f.player && prev == f.ai)
            {
                prev = f.player;
                playerScore++;
                aiScore = 0;
            }
            else if (f.ppField[y][y - 1] == f.ai && prev == f.player)
            {
                prev = f.ai;
                playerScore = 0;
                aiScore++;
            }
            else
            {
                prev = EMPTY;
                aiScore = 0;
                playerScore = 0;
            }
        }
        
        playerScore = 0;
        aiScore = 0;
        prev = EMPTY;
        for (size_t y = 0; y < f.size - 1; y++)
        {
            if (f.ppField[y][f.size - y - 2] == f.player && (prev == EMPTY || prev == f.player))
            {
                prev = f.player;
                playerScore++;
                if (playerScore >= f.winline)
                    return WON_PLAYER;
            }
            else if (f.ppField[y][f.size - y - 2] == f.ai && (prev == EMPTY || prev == f.ai))
            {
                prev = f.ai;
                aiScore++;
                if (aiScore >= f.winline)
                    return WON_AI;
            }
            else if (f.ppField[y][f.size - y - 2] == f.player && prev == f.ai)
            {
                prev = f.player;
                playerScore++;
                aiScore = 0;
            }
            else if (f.ppField[y][f.size - y - 2] == f.ai && prev == f.player)
            {
                prev = f.ai;
                playerScore = 0;
                aiScore++;
            }
            else
            {
                prev = EMPTY;
                aiScore = 0;
                playerScore = 0;
            }
        }

        playerScore = 0;
        aiScore = 0;
        prev = EMPTY;
        for (size_t y = 0; y < f.size - 1; y++)
        {
            if (f.ppField[y + 1][f.size - y - 1] == f.player && (prev == EMPTY || prev == f.player))
            {
                prev = f.player;
                playerScore++;
                if (playerScore >= f.winline)
                    return WON_PLAYER;
            }
            else if (f.ppField[y + 1][f.size - y - 1] == f.ai && (prev == EMPTY || prev == f.ai))
            {
                prev = f.ai;
                aiScore++;
                if (aiScore >= f.winline)
                    return WON_AI;
            }
            else if (f.ppField[y + 1][f.size - y - 1] == f.player && prev == f.ai)
            {
                prev = f.player;
                playerScore++;
                aiScore = 0;
            }
            else if (f.ppField[y + 1][f.size - y - 1] == f.ai && prev == f.player)
            {
                prev = f.ai;
                playerScore = 0;
                aiScore++;
            }
            else
            {
                prev = EMPTY;
                aiScore = 0;
                playerScore = 0;
            }
        }
    }

    if (f.size - f.winline >= 2)
    {
        playerScore = 0;
        aiScore = 0;
        prev = EMPTY;
        for (size_t y = 2; y < f.size; y++)
        {
            if (f.ppField[y - 2][y] == f.player && (prev == EMPTY || prev == f.player))
            {
                prev = f.player;
                playerScore++;
                if (playerScore >= f.winline)
                    return WON_PLAYER;
            }
            else if (f.ppField[y - 2][y] == f.ai && (prev == EMPTY || prev == f.ai))
            {
                prev = f.ai;
                aiScore++;
                if (aiScore >= f.winline)
                    return WON_AI;
            }
            else if (f.ppField[y - 2][y] == f.player && prev == f.ai)
            {
                prev = f.player;
                playerScore++;
                aiScore = 0;
            }
            else if (f.ppField[y - 2][y] == f.ai && prev == f.player)
            {
                prev = f.ai;
                playerScore = 0;
                aiScore++;
            }
            else
            {
                prev = EMPTY;
                aiScore = 0;
                playerScore = 0;
            }
        }

        prev = EMPTY;
        aiScore = 0;
        playerScore = 0;
        for (size_t y = 2; y < f.size; y++)
        {
            if (f.ppField[y][y - 2] == f.player && (prev == EMPTY || prev == f.player))
            {
                prev = f.player;
                playerScore++;
                if (playerScore >= f.winline)
                    return WON_PLAYER;
            }
            else if (f.ppField[y][y - 2] == f.ai && (prev == EMPTY || prev == f.ai))
            {
                prev = f.ai;
                aiScore++;
                if (aiScore >= f.winline)
                    return WON_AI;
            }
            else if (f.ppField[y][y - 2] == f.player && prev == f.ai)
            {
                prev = f.player;
                playerScore++;
                aiScore = 0;
            }
            else if (f.ppField[y][y - 2] == f.ai && prev == f.player)
            {
                prev = f.ai;
                playerScore = 0;
                aiScore++;
            }
            else
            {
                prev = EMPTY;
                aiScore = 0;
                playerScore = 0;
            }
        }

        playerScore = 0;
        aiScore = 0;
        prev = EMPTY;
        for (size_t y = 0; y < f.size - 2; y++)
        {
            if (f.ppField[y][f.size - y - 3] == f.player && (prev == EMPTY || prev == f.player))
            {
                prev = f.player;
                playerScore++;
                if (playerScore >= f.winline)
                    return WON_PLAYER;
            }
            else if (f.ppField[y][f.size - y - 3] == f.ai && (prev == EMPTY || prev == f.ai))
            {
                prev = f.ai;
                aiScore++;
                if (aiScore >= f.winline)
                    return WON_AI;
            }
            else if (f.ppField[y][f.size - y - 3] == f.player && prev == f.ai)
            {
                prev = f.player;
                playerScore++;
                aiScore = 0;
            }
            else if (f.ppField[y][f.size - y - 3] == f.ai && prev == f.player)
            {
                prev = f.ai;
                playerScore = 0;
                aiScore++;
            }
            else
            {
                prev = EMPTY;
                aiScore = 0;
                playerScore = 0;
            }
        }

        playerScore = 0;
        aiScore = 0;
        prev = EMPTY;
        for (size_t y = 0; y < f.size - 2; y++)
        {
            if (f.ppField[y + 2][f.size - y - 1] == f.player && (prev == EMPTY || prev == f.player))
            {
                prev = f.player;
                playerScore++;
                if (playerScore >= f.winline)
                    return WON_PLAYER;
            }
            else if (f.ppField[y + 2][f.size - y - 1] == f.ai && (prev == EMPTY || prev == f.ai))
            {
                prev = f.ai;
                aiScore++;
                if (aiScore >= f.winline)
                    return WON_AI;
            }
            else if (f.ppField[y + 2][f.size - y - 1] == f.player && prev == f.ai)
            {
                prev = f.player;
                playerScore++;
                aiScore = 0;
            }
            else if (f.ppField[y + 2][f.size - y - 1] == f.ai && prev == f.player)
            {
                prev = f.ai;
                playerScore = 0;
                aiScore++;
            }
            else
            {
                prev = EMPTY;
                aiScore = 0;
                playerScore = 0;
            }
        }
    }

    bool draw = true;
    for (size_t y = 0; y < f.size; y++)
    {
        for (size_t x = 0; x < f.size; x++)
        {
            if (f.ppField[y][x] == EMPTY)
            {
                draw = false;
                break;
            }
        }
        if (!draw)
            break;
    }
    if (draw)
        return DRAW;

    return IN_PROGRESS;
}

//====================================================================================================

int main()
{
    field f;
    startGame(f);

    do {
        clrScr();
        drawField(f);
        if (f.turn % 2 == 0)
        {
            coord c = getPlayerCoord(f);
            f.ppField[c.y][c.x] = f.player;
        }
        else
        {
            coord c = getAICoord(f);
            f.ppField[c.y][c.x] = f.ai;
        }

        f.turn++;
        f.progress = checkWiner(f);
    } while (f.progress == IN_PROGRESS);
    clrScr();
    drawField(f);
    if (f.progress == WON_PLAYER)
        cout << "You win! Congrats!";
    else if (f.progress == WON_AI)
        cout << "AI win! Game over.";
    else if (f.progress == DRAW)
        cout << "Draw! Game over.";
    endGame(f);
    return 0;
}