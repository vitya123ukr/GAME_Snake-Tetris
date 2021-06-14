#include <SFML/Graphics.hpp>
#include <time.h>
using namespace sf;

enum class Mode {
    SNAKE_MODE,
    TETRIS_MODE
};

Keyboard::Key snake_dir = Keyboard::Down;
Mode mode = Mode::SNAKE_MODE;

const int M = 20;
const int N = 10;

const int apple_dist = 10;

int field[M][N] = { 0 };

struct Point
{
    int x, y;
};

bool operator==(const Point& lhs, const Point& rhs)
{
    return (lhs.x == rhs.x && lhs.y == rhs.y);
}

Point a[4], b[4];
Point apple;

int colorFigure = 1;
int colorSnakeHead = 7;
int colorApple = 6;

int figures[7][4] =
{
    1,3,5,7, // I
    2,4,5,7, // Z
    3,5,4,6, // S
    3,5,4,7, // T
    2,3,5,7, // L
    3,5,7,6, // J
    2,3,4,5, // O
};

bool check_collision(Point p)
{
    if (p.x < 0 || p.x >= N || p.y < 0 || p.y >= M)
        return true;

    if (field[p.y][p.x])
        return true;

    return false;
}

bool check_collision()
{
    for (int i = 0; i < 4; i++)
        if (check_collision(a[i]))
            return true;

    return false;
};

void gameover()
{
    
    exit(0);
}

void generate_snake()
{
    // generate snake
    a[0].x = a[1].x = a[2].x = a[3].x = rand() % N;
    for (int i = 0; i < 4; ++i)
        a[i].y = -i;

    // generate apple
    int max_level = M - 1;
    for (; max_level >= 0; --max_level)
    {
        bool any_filled = false;
        for (int x = 0; x < N; ++x)
            if (field[max_level][x])
            {
                any_filled = true;
                break;
            }

        if (not any_filled)
            break;
    }

    max_level -= apple_dist;
    if (max_level < 0)
        max_level = 0;

    apple.x = rand() % N;
    apple.y = rand() % max_level;

    colorFigure = 1 + rand() % 5;
}

int main()
{
    srand(time(0));

    RenderWindow window(VideoMode(320, 480), "The Game Snake+Tetris");

    Texture t1, t2, t3;
    t1.loadFromFile("D:/Курсова/tiles.png");
    t2.loadFromFile("D:/Курсова/background.png");
    t3.loadFromFile("D:/Курсова/frame.png");

    Sprite s(t1), background(t2), frame(t3);

    int dx = 0; bool rotate = 0;
    float timer = 0, delay = 0.3;

    Clock clock;

    generate_snake();

    while (window.isOpen())
    {
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;

        if (Mode::SNAKE_MODE == mode)
        {
            Event e;
            while (window.pollEvent(e))
            {
                if (e.type == Event::Closed)
                    window.close();

                if (e.type == Event::KeyPressed &&
                    (e.key.code == Keyboard::Up || e.key.code == Keyboard::Right ||
                        e.key.code == Keyboard::Down || e.key.code == Keyboard::Left))
                {
                    if (snake_dir == e.key.code)
                        delay = 0.05;
                    else
                        snake_dir = e.key.code;
                }
            }

            ///////Tick//////
            if (timer > delay)
            {
                if (not (a[2] == a[0]))
                    a[3] = a[2];
                a[2] = a[1];
                a[1] = a[0];

                switch (snake_dir)
                {
                case Keyboard::Up:
                    --a[0].y; break;
                case Keyboard::Right:
                    ++a[0].x; break;
                case Keyboard::Down:
                    ++a[0].y; break;
                case Keyboard::Left:
                    --a[0].x; break;
                }

                if (a[0] == apple) // snake ate apple
                    mode = Mode::TETRIS_MODE;

                if (check_collision(a[0]))
                    gameover();

                timer = 0;
            }
        }
        else
        {
            Event e;
            while (window.pollEvent(e))
            {
                if (e.type == Event::Closed)
                    window.close();

                if (e.type == Event::KeyPressed)
                    if (e.key.code == Keyboard::Up) rotate = true;
                    else if (e.key.code == Keyboard::Left) dx = -1;
                    else if (e.key.code == Keyboard::Right) dx = 1;
            }

            if (Keyboard::isKeyPressed(Keyboard::Down)) delay = 0.05;

            //// <- Move -> ///
            for (int i = 0; i < 4; i++) { b[i] = a[i]; a[i].x += dx; }
            if (check_collision()) for (int i = 0; i < 4; i++) a[i] = b[i];

            //////Rotate//////
            if (rotate)
            {
                Point p = a[1]; //center of rotation
                for (int i = 0; i < 4; i++)
                {
                    int x = a[i].y - p.y;
                    int y = a[i].x - p.x;
                    a[i].x = p.x - x;
                    a[i].y = p.y + y;
                }
                if (check_collision()) for (int i = 0; i < 4; i++) a[i] = b[i];
            }

            ///////Tick//////
            if (timer > delay)
            {
                for (int i = 0; i < 4; i++) { b[i] = a[i]; a[i].y += 1; }

                if (check_collision())
                {
                    for (int i = 0; i < 4; i++)
                        field[b[i].y][b[i].x] = colorFigure;

                    mode = Mode::SNAKE_MODE;
                    snake_dir = Keyboard::Down;
                    generate_snake();
                }

                timer = 0;
            }
        }

        ///////check lines//////////
        int k = M - 1;
        for (int i = M - 1; i > 0; i--)
        {
            int count = 0;
            for (int j = 0; j < N; j++)
            {
                if (field[i][j]) count++;
                field[k][j] = field[i][j];
            }
            if (count < N) k--;
        }

        dx = 0; rotate = 0; delay = 0.3;

        /////////draw//////////
        window.clear(Color::White);
        window.draw(background);

        for (int i = 0; i < M; i++)
            for (int j = 0; j < N; j++)
            {
                if (field[i][j] == 0) continue;
                s.setTextureRect(IntRect(field[i][j] * 18, 0, 18, 18));
                s.setPosition(j * 18, i * 18);
                s.move(28, 31); //offset
                window.draw(s);
            }

        for (int i = 3; i >= 0; --i)
        {
            if (a[i].y >= 0)
            {
                int color = (Mode::SNAKE_MODE == mode && i == 0) ? colorSnakeHead : colorFigure;
                s.setTextureRect(IntRect(color * 18, 0, 18, 18));
                s.setPosition(a[i].x * 18, a[i].y * 18);
                s.move(28, 31); //offset
                window.draw(s);
            }
        }

        if (Mode::SNAKE_MODE == mode)
        {
            s.setTextureRect(IntRect(colorApple * 18, 0, 18, 18));
            s.setPosition(apple.x * 18, apple.y * 18);
            s.move(28, 31); //offset
            window.draw(s);
        }

        window.draw(frame);
        window.display();
    }

    return 0;
}
