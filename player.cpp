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

class Board {
   public:
    Board& operator=(Board& objBoard) {
        this->board = objBoard.board;
        this->discCnt = objBoard.discCnt;
        this->curPlayer = objBoard.curPlayer;
        return (*this);
    }
    constexpr const std::array<int, SIZE> operator[](int idx) const {
        return this->board[idx];
    }

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
    std::array<std::array<int, SIZE>, SIZE> board;

   public:
    Board() {
        std::fill(this->discCnt.begin(), this->discCnt.end(), 0);
    }
    ~Board() = default;
    int get_next_player(int player) const {
        return 3 - player;
    }
    bool is_spot_on_board(Point p) const {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const {
        return board[p.x][p.y];
    }
    void set_disc(Point p, int disc) {
        board[p.x][p.y] = disc;
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
class AIMethod {
   protected:
    int curPlayer;
    Board curBoard;
    std::vector<Point> nxtSpots;

   public:
    AIMethod() {}
    virtual ~AIMethod() {}
    virtual void __init__(int& player, Board& board, std::vector<Point>& next_spots) {
        this->curPlayer = player;
        this->curBoard = board;
        this->nxtSpots = next_spots;
    }
    virtual Point solve() { return Point(0, 0); }
};

class AIRandom : public AIMethod {
   public:
    Point solve() {
        int n_valid_spots = nxtSpots.size();
        srand(time(NULL));
        int index = (rand() % n_valid_spots);
        return nxtSpots[index];
    }
};

class AIStateValueFunction : public AIMethod {
    const std::array<std::array<int, SIZE>, SIZE> STATEVAL = {{
        {10, 1, 5, 5, 5, 5, 1, 10},
        {1, 1, 2, 2, 2, 2, 1, 1},
        {5, 2, 3, 3, 3, 3, 2, 5},
        {5, 2, 3, 3, 3, 3, 2, 5},
        {5, 2, 3, 3, 3, 3, 2, 5},
        {5, 2, 3, 3, 3, 3, 2, 5},
        {1, 1, 2, 2, 2, 2, 1, 1},
        {10, 1, 5, 5, 5, 5, 1, 10},
    }};

   public:
    void solve() override{
        queue<Board> tasks;
        
    }
};

class Player {
   private:
    int curPlayer;
    Board curBoard;
    std::vector<Point> nxtSpots;
    Point finSpot;
    int argc;
    char** argv;

   public:
    Player(int argc, char** argv) : argc(argc), argv(argv) {}

    void start(AIMethod* aiMethod) {
        std::ifstream fin(this->argv[1]);   // Input file stream
        std::ofstream fout(this->argv[2]);  // Output file stream
        this->read_board(fin);              // Read the board from fin
        this->read_valid_spots(fin);        // Read Valid inputs from fin

        if (aiMethod) {
            aiMethod->__init__(curPlayer, curBoard, nxtSpots);
            this->finSpot = aiMethod->solve();
            delete aiMethod;
        }

        this->write_valid_spot(fout);  // Output My Move
        fin.close();
        fout.close();
    }

    void read_board(std::ifstream& fin) {
        fin >> curPlayer;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                fin >> this->curBoard.board[i][j];
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

    void write_valid_spot(std::ofstream& fout) {
        // Remember to flush the output to ensure the last action is written to file.
        fout << finSpot.x << " " << finSpot.y << std::endl;
        fout.flush();
    }
};

int main(int argc, char** argv) {
    Player player(argc, argv);
    player.start(new AIRandom());
    return 0;
}