#include <termios.h>
#include <unistd.h>
#include <cstdio>
#include <iostream>
#include<ctime>
#include<vector>
#include<list>

using namespace std;

int getch()
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}

class snake 
{
    
    unsigned height, width;
    char** field; // зберігає розмір поля гри
    long int eaten_apples;
    unsigned apple_coor_x, apple_coor_y;

    list<vector<int>> coordinates;

    void show_field(); // Виводить ігрове поле
    void new_frame(int rows, int cols); // Сюди я перекидував всі дії для підготовки консолі до виведення нового "кадру"
    void game_over(int rows, int cols);// Виконує усі дії пов'язані з програшом
    void snake_move(int &rows, int &cols, char &movement);//Відповідає за рух змійки
    void make_apple(int rows, int cols);// Знаходить оптимальне місце для створення ябука і створює його
    bool is_apple_eaten();// Перевіряє чи є на полі якесь яблуко
    void snakes_tail(int rows, int cols);// Записує координати попереднього перебування "голови" змійки. Ніж більше значення eaten_apples тим більше координат воно записує

public:
    snake();
    ~snake(){delete[] field;}

    
    void play_game(); // функція яка відповідає за ігровий процес
    

};

void snake::snakes_tail(int rows, int cols)
{
    static unsigned int prev_score = 0;

    if(eaten_apples != 0)
    {
        coordinates.push_back({rows, cols});
        
        if (prev_score < eaten_apples)
        {
            prev_score = eaten_apples;
            return;
        }
        

        field[coordinates.front()[0]][coordinates.front()[1]] = ' ';
        coordinates.pop_front();
    }
    else
    {
        // зачищається попередня *
        field[rows][cols] = ' ';
    }
    
}

snake::snake()
{
    int size; // Відповідає за розмірність поля гри

    eaten_apples = -1; // Тут -1 тому що після початку гри яблук не буде, і алгоритм зразу додасть до цієї змінної 1
    apple_coor_y = 0;
    apple_coor_x = 1;// Тут одиниця щоб яблуко не появилося на місці створення змійки
    // Тут створюється поле вказаного розміру
    cout << "Введіть розмірність поля: " << endl;
    cin >> size;

    height = size/2;//Тут size/2 для того щоб поле було семетричним 
    width = size;

    field = new char*[height];
    for (int i = 0; i < height; i++)
    {
        field[i] = new char[width];
        for (int j = 0; j < width; j++)
        {
            field[i][j] = ' ';
        }
        
    }
}

void snake::show_field()
{
    //Верхня межа поля
    for (int i = 0; i < width+2; i++)
    {
        cout << '~';
    }
    cout << endl;
    

    for (int  i = 0; i < height; i++)
    {
        cout << '|';// Ліва межа поля
        for (int j = 0; j < width; j++)
        {
            cout << field[i][j];
        }
        cout << '|';// Права межа поля
        cout << endl;
    }

    // Нижня межа поля
    for (int i = 0; i < width+2; i++)
    {
        cout << '~';
    }
    cout << endl;

    // Виводить '*' на місця колишніх координат змійки для створення іллюзії повзання
    if (coordinates.size() != 0)
    {
        auto iter = coordinates.begin();

        for (int i = 0; i < coordinates.size(); i++)
        {
            field[(*iter)[0]][(*iter)[1]] = '*';
        }
    }
}

void snake::new_frame(int rows, int cols)
{
    // Тут всі необхіжні дії для створення нового кадру
    system("clear");
}

void snake::snake_move(int &rows, int &cols, char &movement)
{
    // Відбувається проста перевірка на те, який символ був введений
    if (tolower(movement) == 'w')
    {
        if(rows != 0)
        {
            snakes_tail(rows, cols);
            rows--;
        }
        else
        {
            snakes_tail(rows, cols);
            rows = height-1;
        }
        
    }
    else if (tolower(movement) == 's')
    {
        if(rows != height-1)
        {   
            snakes_tail(rows, cols);
            rows++;
        }
        else
        {
            snakes_tail(rows, cols);
            rows = 0;
        }
    }
    else if (tolower(movement) == 'a')
    {
        if(cols != 0)
        {
            snakes_tail(rows, cols);
            cols--;
        }
        else
        {
            snakes_tail(rows, cols);
            cols = width-1;
        }
  
    }
    else if (tolower(movement) == 'd')
    {
        if(cols != width-1)
        {
            snakes_tail(rows, cols);
            cols++;
        }
        else
        {
            snakes_tail(rows, cols);
            cols = 0;
        }
    }
}

bool snake::is_apple_eaten()
{
    if(field[apple_coor_y][apple_coor_x] == 'A')
    {
        return false;
    }
    else
    {
        eaten_apples++;
        return true;
    }
    
}

void snake::make_apple(int rows, int cols)
{
    // Перевіряється чи ще існує яблуко, якщо ні, то ствовюється нове, у протилежному випадку нічого не відбувається
    if (is_apple_eaten())
    {
        to_initialization:    apple_coor_x = (unsigned)(rand() % width-1); // goto знаходиться в наступних двох if
                              apple_coor_y = (unsigned)(rand() % height-1);

        if (apple_coor_x > width || apple_coor_y > height) // Перевіряє чи не виходять х і у за межі поля, якщо так, то їм присвоюється нове значення
        {
            goto to_initialization;
        }

        if (field[apple_coor_y][apple_coor_x] == '*')
        {
            goto to_initialization;
        }

        field[apple_coor_y][apple_coor_x] = 'A';
    }
    
}

void snake::play_game()
{
    srand(time(NULL));
    char movement = 'a';

    int rows = 0, cols = 0;
    while(movement != '0')
    {
        if (field[rows][cols] != '*' || movement == '\n')
        {
            field[rows][cols] = '*';
        }
        else
        {
            game_over(rows, cols);
            return;
        }
        
        
        
        make_apple(rows, cols);

        cout << "Score: " << eaten_apples << endl;

        show_field();
        movement = getch();

        
        new_frame(rows, cols);
        snake_move(rows, cols, movement);

    }
}

void snake::game_over(int rows, int cols)
{
    cout << "Game Over!\a\n";
    cout << "Score: " << eaten_apples << endl;

    field[rows][cols] = 'X';
    show_field();

    cout << "Нажміть любу кнопку щоб закінчити...\n";

    char ch;
    ch = getch();    

    system("clear");
    
        
}

int main()
{
    snake snake_game;
    snake_game.play_game();
    

    return 0;
}