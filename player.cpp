#include <bits/stdc++.h>

const int SIZE = 8;
struct Point {
    int x, y;
    Point() : Point(0, 0) {}
    Point(int x, int y) : x(x), y(y) {}
    bool operator==(const Point& rhs) const {
        return x == rhs.x && y == rhs.y;
    }
    bool operator!=(const Point& rhs) const {
        return !operator==(rhs);
    }
    Point operator+(const Point& rhs) const {
        return Point(x + rhs.x, y + rhs.y);
    }
    Point operator-(const Point& rhs) const {
        return Point(x - rhs.x, y - rhs.y);
    }
};

class Board : public std::array<std::array<int, SIZE>, SIZE> {
   public:
    enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
    const std::array<Point, 8> directions{{Point(-1, -1), Point(-1, 0), Point(-1, 1),
                                           Point(0, -1), /*{0, 0}, */ Point(0, 1),
                                           Point(1, -1), Point(1, 0), Point(1, 1)}};
    std::array<int, 3> discCnt;
    int curPlayer;

   public:
    Board() {
        std::array<std::array<int, SIZE>, SIZE>();
        std::fill(this->discCnt.begin(), this->discCnt.end(), 0);
    };
    ~Board() = default;
    int get_next_player(int player) const {
        return 3 - player;
    }
    bool is_spot_on_board(Point p) const {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const {
        return (*this)[p.x][p.y];
    }
    void set_disc(Point p, int disc) {
        (*this)[p.x][p.y] = disc;
    }
    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center) const {
        if (get_disc(center) != EMPTY)
            return false;
        for (Point dir : directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(curPlayer)))
                continue;
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, curPlayer))
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
    void flip_discs(Point center) {
        for (Point dir : directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(curPlayer)))
                continue;
            std::vector<Point> discs({p});
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, curPlayer)) {
                    for (Point s : discs) {
                        set_disc(s, curPlayer);
                    }
                    discCnt[curPlayer] += discs.size();
                    discCnt[get_next_player(curPlayer)] -= discs.size();
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
    }
};

class Player {
   public:
    enum AI_TYPE {
        RANDOM = 0,
        MANUAL = 1,
        STATE_VALUE_FUNCTION = 2,
        MINIMAX = 3,
        ALPHA_BETA_PRUNING = 4
    };

   private:
    int curPlayer;
    Board curBoard;
    std::vector<Point> nxtSpots;
    Point finSpot;
    int argc;
    char** argv;

   public:
    Player(int argc, char** argv) : argc(argc), argv(argv) {}

    void start(AI_TYPE option = RANDOM) {
        std::ifstream fin(this->argv[1]);
        std::ofstream fout(this->argv[2]);
        this->read_board(fin);
        this->read_valid_spots(fin);
        std::cout << "I'm player: " << curPlayer << std::endl;
        switch (option) {
            case RANDOM:
                this->move_random();
                break;
            case MANUAL:
                this->move_manual();
                break;
            case STATE_VALUE_FUNCTION:
                this->move_state_value_function();
                break;
            case MINIMAX:
                this->move_minimax();
                break;
            case ALPHA_BETA_PRUNING:
                this->move_alpha_beta_pruning();
                break;
            default:
                this->move_random();
                break;
        }

        this->write_valid_spot(fout);
        fin.close();
        fout.close();
    }

    void read_board(std::ifstream& fin) {
        fin >> curPlayer;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                fin >> this->curBoard[i][j];
            }
        }
    }

    void read_valid_spots(std::ifstream& fin) {
        int n_valid_spots;
        fin >> n_valid_spots;
        int x, y;
        for (int i = 0; i < n_valid_spots; i++) {
            fin >> x >> y;
            this->nxtSpots.push_back({x, y});
        }
    }

    void move_random() {
        int n_valid_spots = nxtSpots.size();
        srand(time(NULL));
        int index = (rand() % n_valid_spots);
        finSpot = nxtSpots[index];
    }
    void move_manual() {
        int index;
        std::cin >> index;
        finSpot = nxtSpots[index];
    }
    void move_state_value_function() {
    }
    void move_minimax() {
    }
    void move_alpha_beta_pruning() {
    }

    void write_valid_spot(std::ofstream& fout) {
        // Remember to flush the output to ensure the last action is written to file.
        fout << finSpot.x << " " << finSpot.y << std::endl;
        fout.flush();
    }
};

int main(int argc, char** argv) {
    Player player(argc, argv);
    player.start(Player::RANDOM);
    return 0;
}