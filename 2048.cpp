#include <iostream>
#include <time.h>
#define MAX    16  // 随机事件的最大数量
#define string std::string
// 生成随机数[a,b)
int random_number(int a, int b);

class Game
{
private:
    int *board;
    int max;            // 棋盘大小
    int winning_score;  // 获胜分数
    int rate;           // 生成2与4的比例
    bool rogue;         // Rogue模式
    int score;          // 当前分数
    int score_sub;      // 每回合移动扣的分，初始为1
    int left_round;  // 距离下一次随机事件回合，在min和max之间
    int min_round;   // 最小left_round，初始为9
    int max_round;   // 最大left_round，初始为15
    struct Issue
    {
        int no;
        string name;
        string description;
        bool is_trigger;  // 是否是瞬间执行的
        int duration;     // 持续时间
    };
    Issue issue[MAX];
    int num_of_issue;

    // 打印棋盘
    void print_board();

    // 初始化棋盘
    void initialize_board();

    // 随机生成2或4
    void generate_number();

    // 转置
    void transit();

    // 左右对称
    void mirror();

    // 单行向左
    void move_left(int *line);

    // 判断是否可以向左移动
    bool could_move_left();

    // 移动，返回false表示移动失败
    bool move();

    // 判断结束（0为未结束，1为成功，-1为失败）
    int check_end();

    // 随机事件初始化
    void issue_initialize(Issue *issue, int n);

    // 随机事件执行
    void roguelike();

    // 持续性随机事件生效
    void issue_working();

    // 打印随机事件信息
    void information(int i);
public:
    void game_process();
    Game(int max, int winning_score, int rate, bool rogue)
    {
        this->max = max;
        this->winning_score = winning_score;
        this->rate = rate;
        this->rogue = rogue;
    }
};

int main()
{
    std::cout << "Choose level: type e for EASY, n for NORMAL and so on"
              << std::endl
              << "EASY: size: 5*5, rate: 3, winning_score: 8192" << std::endl
              << "NORMAL:size: 4*4, rate: 3, winning_score: 2048" << std::endl
              << "HARD: size: 4*4, rate: 1, winning_score: 4096" << std::endl
              << "LUNATIC: size: 3*3, rate: 1, winning_score: 512" << std::endl
              << "ROGUE: A special mode with random issues" << std::endl;
    char choose_level;
    std::cin >> choose_level;
    int max, winning_score, rate, rogue;
    switch (choose_level)
    {
    case 'e':
        max = 5;
        rate = 3;
        winning_score = 8192;
        rogue = false;
        break;
    case 'n':
        max = 4;
        rate = 3;
        winning_score = 2048;
        rogue = false;
        break;
    case 'h':
        max = 4;
        rate = 1;
        winning_score = 4096;
        rogue = false;
        break;
    case 'l':
        max = 3;
        rate = 1;
        winning_score = 512;
        rogue = false;
        break;
    case 'r':
        max = 5;
        rate = 3;
        winning_score = 4096;
        rogue = true;
        break;
    default:
        max = 4;
        rate = 3;
        winning_score = 2048;
        rogue = false;
        break;
    }
    Game game(max, winning_score, rate, rogue);
    game.game_process();
    return 0;
}

void Game::game_process()
{
    initialize_board();
    if (rogue)
    {
        num_of_issue = 13;  // 标记：事件数量
        issue_initialize(issue, num_of_issue);
    }
    print_board();
    std::cout << "Current Score: " << score << std::endl;
    if (rogue)
        std::cout << "Before Next Issue: " << left_round << std::endl;
    while (true)
    {
        int count_of_0 = 0;
        for (int i = 0; i < max * max; i++)
            if (board[i] == 0)
                count_of_0++;
        if (count_of_0 == max * max)
        {
            std::cout << "No numbers, so generate one automatically."
                      << std::endl;
            generate_number();
            print_board();
        }            // 上面都是在防止没有数字的情况
        if (move())  // 成功移动
        {
            generate_number();
            left_round--;
        }
        system("cls");
        print_board();
        std::cout << "Current Score: " << score << std::endl;
        if (rogue)
        {
            std::cout << "Before Next Issue:" << left_round << std::endl;
            issue_working();
        }

        if (rogue && left_round <= 0)  // 随机事件发生
        {
            roguelike();
            left_round = random_number(min_round, max_round);
        }

        if (check_end() == 1)
        {
            std::cout << "Congratulations! you've succeeded! " << std::endl;
            std::cout << "Your final score is " << score << std::endl;
            break;
        } else if (check_end() == -1)
        {
            std::cout << "GG" << std::endl;
            std::cout << "Your final score is " << score << std::endl;
            break;
        }
    }
    delete[] board;
}

int random_number(int a, int b)
{
    srand(time(0));
    return (rand() % (b - a)) + a;
}

void Game::print_board()
{
    for (int i = 0; i < max; i++)
    {
        for (int j = 0; j < max; j++)
        {
            int number = *(board + max * i + j);
            int digit = 1;
            if (number > 0)
            {
                digit = 0;
                while (number > 0)
                {
                    digit++;
                    number /= 10;
                }
            }
            std::cout << *(board + max * i + j);
            if (j < max - 1)
            {
                for (int i = 0; i < 5 - digit; i++)
                    std::cout << ' ';
            }
        }
        std::cout << std::endl;
    }
}

void Game::initialize_board()
{
    board = new int[max * max];
    score = 0;
    score_sub = 1;
    min_round = 9;
    max_round = 15;
    left_round = random_number(min_round, max_round);

    for (int i = 0; i < max * max; i++)
    {
        board[i] = 0;
    }
    // 随机数0-15
    int randnumber = random_number(0, 16);
    switch (randnumber / 4)
    {
    case 0: board[0] = 2; break;
    case 1: *(board + max * (max - 1)) = 2; break;
    case 2: board[max - 1] = 2; break;
    case 3: *(board + max - 1 + max * (max - 1)) = 2; break;
    default: break;
    }
    do
    {
        randnumber = random_number(0, max * max);
    }
    while (board[randnumber] == 2);
    board[randnumber] = 2;
}

void Game::generate_number()
{
    // 生成2或4(比例为 rate)
    int rand_number = random_number(1, 101);
    int number2or4 = 2;
    float border = 100.0 / (rate + 1);
    if (rand_number <= border)
        number2or4 = 4;
    // 寻找可以生成数的位置
    int *location = new int[2 * max * max];  // 偶数位为行，奇数位为列
    int count_of_zero = 0;
    for (int i = 0; i < max; i++)
    {
        for (int j = 0; j < max; j++)
        {
            if (*(board + max * i + j) == 0)
            {
                count_of_zero++;
                location[2 * count_of_zero - 2] = i;
                location[2 * count_of_zero - 1] = j;
            }
        }
    }
    if (count_of_zero == 0)
        return;
    rand_number = random_number(1, count_of_zero + 1);
    *(board + max * location[2 * rand_number - 2] +
      location[2 * rand_number - 1]) = number2or4;
    delete[] location;
}

void Game::transit()
{
    for (int i = 0; i < max; i++)
    {
        for (int j = i; j < max; j++)
        {
            int temp = *(board + max * i + j);
            *(board + max * i + j) = *(board + i + max * j);
            *(board + i + max * j) = temp;
        }
    }
}

void Game::mirror()
{
    for (int i = 0; i < max; i++)
    {
        for (int j = 0; j < max / 2; j++)
        {
            int temp = *(board + max * i + j);
            *(board + max * i + j) = *(board + max * i + (max - 1 - j));
            *(board + max * i + (max - 1 - j)) = temp;
        }
    }
}

void Game::move_left(int *line)
{
    int index;
    int value;
    // “合并同类项”
    //从第二个数开始到第四个数
    for (int i = 1; i < max; i++)
    {
        value = line[i];
        if (value == 0)
            continue;//找到第一个不是0的数
        index = i - 1;
        while (index >= 0)//从这个数往前一直找
        {
            if (line[index] > 0)
            {
                if (line[index] != value)
                    break;//找到不为0的数但不能合并就停止
                else
                {
                    line[index] = 2 * value;//找到可以合并的数就合并
                    line[i] = 0;//然后把这个数归零
                    score += value;
                }
            } else
                index--;
        }
    }
    // 把0移到右边
    int n = max;
    bool exchange;
    while (n > 1)
    {
        exchange = false;
        for (int i = 0; i < n - 1; i++)
        {
            if (line[i] == 0)
            {
                int temp = line[i];
                line[i] = line[i + 1];
                line[i + 1] = temp;
                exchange = true;
            }
        }
        if (!exchange)
            break;
        n--;
    }
}

bool Game::could_move_left()
{
    int outside_sum = 0;
    for (int i = 0; i < max; i++)
    {
        int *temp1 = new int[max];
        int *temp2 = new int[max];
        for (int j = 0; j < max; j++)
        {
            temp1[j] = *(board + max * i + j);
            temp2[j] = temp1[j];
        }
        move_left(temp1);
        int inside_sum = 0;
        for (int i = 0; i < max; i++)
        {
            if (temp1[i] == temp2[i])
                inside_sum++;
        }
        if (inside_sum < max)
            outside_sum++;  // 这一行可以移动
        delete[] temp1;
        delete[] temp2;
    }
    if (outside_sum == 0)
        return false;
    else
        return true;
}

bool Game::move()
{
    char direction = 'w';
    std::cout << "Choose Direction (wasd), or type '!' to end game: ";
    std::cin >> direction;
    switch (direction)
    {
    case 'w':
        transit();
        if (!could_move_left())
        {
            transit();
            return false;
        }
        for (int i = 0; i < max; i++)
            move_left(board + max * i);
        score -= score_sub;
        transit();
        break;
    case 's':
        transit();
        mirror();
        if (!could_move_left())
        {
            mirror();
            transit();
            return false;
        }
        for (int i = 0; i < max; i++)
            move_left(board + max * i);
        score -= score_sub;
        mirror();
        transit();
        break;
    case 'a':
        if (!could_move_left())
            return false;
        for (int i = 0; i < max; i++)
            move_left(board + max * i);
        score -= score_sub;
        break;
    case 'd':
        mirror();
        if (!could_move_left())
        {
            mirror();
            return false;
        }
        for (int i = 0; i < max; i++)
            move_left(board + max * i);
        score -= score_sub;
        mirror();
        break;
    case '!':
        std::cout << "Game end, your final score is " << score << std::endl;
        delete[] board;
        exit(0);
        break;
    default:
        std::cout << "Invalid direction, please repeat" << std::endl;
        return false;
    }
    return true;
}

int Game::check_end()
{
    // 找winning_score
    for (int i = 0; i < max; i++)
    {
        for (int j = 0; j < max; j++)
        {
            if (*(board + max * i + j) == winning_score)
                return 1;
        }
    }
    for (int i = 0; i < max; i++)
    {
        for (int j = 0; j < max; j++)
        {
            if (*(board + max * i + j) == 0)
                return 0;
        }
    }
    // 判断不能移动
    bool left = could_move_left();
    mirror();
    bool right = could_move_left();
    transit();
    bool up = could_move_left();
    mirror();
    bool down = could_move_left();
    mirror();
    transit();
    mirror();
    if (!left && !right && !up && !down)
        return -1;
    return 0;
}

void Game::issue_initialize(Issue *issue, int n)
{
    for (int i = 0; i < n; i++)
    {
        issue[i].no = i + 1;
        issue[i].duration = 0;
        switch (i)
        {
        case 0:
            issue[i].name = "SHAME ON JINGKANG";
            issue[i].description =
                "The half of Song Dynasty was conquered, and so was your numbers.";
            issue[i].is_trigger = true;
            break;
        case 1:
            issue[i].name = "FRENCH REVOLUTION";
            issue[i].description =
                "Your max number was cut in half, like the Louis XVI on the guillotine.";
            issue[i].is_trigger = true;
            break;
        case 2:
            issue[i].name = "INDUSTRIAL REVOLUTION";
            issue[i].description =
                "Productivity has been improved.Within 50 turns, it generates 2 numbers by 1 turn.";
            issue[i].is_trigger = false;
            break;
        case 3:
            issue[i].name = "DIOCLETANUS";
            issue[i].description =
                "Roman was divided into two parts, and you've lost your numbers on the right half part.";
            issue[i].is_trigger = true;
            break;
        case 4:
            issue[i].name = "CONCESSION CITY";
            issue[i].description = "Your board was semi-colonized with a 1.";
            issue[i].is_trigger = true;
            break;
        case 5:
            issue[i].name = "SIANT SOPHIA";
            issue[i].description =
                "Byzantines built the great church with few years, but it brought conscription and taxation.\nWhithin 50 turns, you'll loose 4 scores per move";
            issue[i].is_trigger = false;
            break;
        case 6:
            issue[i].name = "CENTRALIZATION";
            issue[i].description =
                "The greatest number stands in the middle of your board like a emperor.";
            issue[i].is_trigger = true;
            break;
        case 7:
            issue[i].name = "GREAT VOYAGE";
            issue[i].description =
                "Magellan's fleet returned to Eerope, and your board rotated 180 degrees.";
            issue[i].is_trigger = true;
            break;
        case 8:
            issue[i].name = "BLACK THURSDAY";
            issue[i].description =
                "The stock crashed, the workers were unemployed, and all your number 2 turned to 0.";
            issue[i].is_trigger = true;
            break;
        case 9:
            issue[i].name = "PROMISED LAND";
            issue[i].description =
                "Jews declared this place home, so you are surrounded by them.";
            issue[i].is_trigger = true;
            break;
        case 10:
            issue[i].name = "WORLD WAR";
            issue[i].description =
                "The horrible war has swallowed every human. Your board was ruined.";
            issue[i].is_trigger = true;
            break;
        case 11:
            issue[i].name = "DARK AGE";
            issue[i].description =
                "The world was never in peace. Within 50 turns, the issue happens more frequently.";
            issue[i].is_trigger = false;
            break;
        case 12:
            issue[i].name = "BERLIN WALL";
            issue[i].description = "It was divided physically, by a war of 1";
            issue[i].is_trigger = true;
            break;
        default:
            issue[i].name = "NOTHING HAPPENED";
            issue[i].description = "NONE";
            issue[i].is_trigger = true;
            break;
        }
    }
}

void Game::information(int i)
{
    std::cout << "NO." << issue[i].no << ": " << issue[i].name << std::endl
              << issue[i].description << std::endl;
}

void Game::issue_working()
{
    if (issue[2].duration > 0)
    {
        generate_number();
        std::cout << "Before industrial revolution ending: "
                  << issue[2].duration << "turns" << std::endl;
        issue[2].duration--;
    }
    if (issue[5].duration > 0)
    {
        std::cout << "Before taxation ending: " << issue[5].duration << "turns"
                  << std::endl;
        issue[5].duration--;
    }
    if (issue[11].duration > 0)
    {
        std::cout << "Before dark age ending: " << issue[11].duration << "turns"
                  << std::endl;
        issue[11].duration--;
    } else
    {
        min_round = 9;
        max_round = 15;
    }
}

void Game::roguelike()
{
    int i = random_number(0, num_of_issue);
    // int i = 12;
    int temp = 0;
    int m_index = 0;
    int count_of_zero = 0;
    int rand_number = random_number(0, max * max);
    int *location = NULL;
    switch (i)
    {
    case 0:
        for (int i = 0; i < max / 2; i++)
        {
            for (int j = 0; j < max; j++)
            {
                *(board + max * i + j) = 0;
            }
        }
        information(i);
        print_board();
        break;
    case 1:
        m_index = 0;
        for (int i = 0; i < max * max; i++)
        {
            if (board[m_index] < board[i])
                m_index = i;
        }
        board[m_index] /= 2;
        information(i);
        print_board();
        break;
    case 2:
        issue[i].duration = 50;
        information(i);
        break;
    case 3:
        for (int i = 0; i < max; i++)
        {
            for (int j = (max - 1) / 2; j < max; j++)
            {
                *(board + max * i + j) = 0;
            }
        }
        information(i);
        print_board();
        break;
    case 4:
        information(i);
        location = new int[2 * max * max];  // 偶数位为行，奇数位为列
        count_of_zero = 0;
        for (int i = 0; i < max; i++)
        {
            for (int j = 0; j < max; j++)
            {
                if (*(board + max * i + j) == 0)
                {
                    count_of_zero++;
                    location[2 * count_of_zero - 2] = i;
                    location[2 * count_of_zero - 1] = j;
                }
            }
        }
        if (count_of_zero == 0)
        {
            std::cout << "No place to put" << std::endl;
            return;
        }
        rand_number = random_number(1, count_of_zero + 1);
        *(board + max * location[2 * rand_number - 2] +
          location[2 * rand_number - 1]) = 1;
        delete[] location;
        print_board();
        break;
    case 5:
        information(i);
        score_sub = 4;
        issue[i].duration = 50;
        break;
    case 6:
        m_index = 0;
        for (int i = 0; i < max * max; i++)
        {
            if (board[m_index] < board[i])
                m_index = i;
        }
        temp = board[m_index];
        board[m_index] = board[max * max / 2];
        board[max * max / 2] = temp;
        information(i);
        print_board();
        break;
    case 7:
        transit();
        mirror();
        transit();
        mirror();
        information(i);
        print_board();
        break;
    case 8:
        for (int i = 0; i < max * max; i++)
        {
            if (board[i] == 2)
                board[i] = 0;
        }
        information(i);
        print_board();
        break;
    case 9:
        for (int i = 0; i < max * max; i++)
        {
            if (i < max || i % max == 0 || i % max == max - 1 ||
                i >= max * (max - 1))
            {
                board[i] = 1;
            }
        }
        information(i);
        print_board();
        break;
    case 10:
        for (int j = 0; j < max * max; j++)
        {
            std::swap(board[j], board[random_number(0, max * max)]);
        }
        information(i);
        print_board();
        break;
    case 11:
        information(i);
        min_round = 5;
        max_round = 11;
        issue[i].duration = 50;
        break;
    case 12:
        for (int i = 0; i < max * max; i++)
        {
            if (i % max == max / 2)
                board[i] = 1;
        }
        information(i);
        print_board();
        break;
    default: break;
    }
}