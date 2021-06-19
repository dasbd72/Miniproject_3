#include <bits/stdc++.h>
#define DEBUG 1
#define MIN -0x7FFFFFFF
#define MAX 0x7FFFFFFF
const int SIZE = 8;
using State = std::array<std::array<int, SIZE>, SIZE>;
struct LOCALTIME {
    static std::chrono::_V2::system_clock::time_point start;
    static std::chrono::duration<double> get_duration() {
        return std::chrono::system_clock::now() - start;
    }
};
std::chrono::_V2::system_clock::time_point LOCALTIME::start;

/**
 * @brief Call the class as cout, ex: LOG() << "Hello World";
 */
class LOG {
   public:
    static std::ofstream fout;
    LOG() {}
    ~LOG() {}
    template <class T>
    LOG& operator<<(const T& msg) {
        if (DEBUG) {
            fout << msg;
            LOG::fout.flush();
        }
        return *this;
    }
};
std::ofstream LOG::fout;

/**
 * @brief Stores: x, y.
 */
struct Point {
   public:
    // Operators
    bool operator==(const Point& rhs) const {
        return x == rhs.x && y == rhs.y;
    }
    bool operator!=(const Point& rhs) const {
        return !operator==(rhs);
    }
    bool operator<(const Point& rhs) const {
        return x < rhs.x || (x == rhs.x && y < rhs.y);
    }
    bool operator>(const Point& rhs) const {
        return operator!=(rhs) && !operator<(rhs);
    }
    Point operator+(const Point& rhs) const {
        return Point(x + rhs.x, y + rhs.y);
    }
    Point operator-(const Point& rhs) const {
        return Point(x - rhs.x, y - rhs.y);
    }
    Point& operator=(const Point& rhs) {
        x = rhs.x, y = rhs.y;
        return (*this);
    }
    friend std::ostream& operator<<(std::ostream&, const Point&);

    int x, y;
    Point() : Point(0, 0) {}
    Point(int x, int y) : x(x), y(y) {}
    Point(const Point& pt) : x(pt.x), y(pt.y) {}
};

/**
 * @brief Board class. Stores: board, curPlayer, value
 */
class Board {
   public:
    // Operators
    friend class Engine;
    friend std::ostream& operator<<(std::ostream&, const Board&);
    bool operator==(const Board& rhs) const {
        return this->curPlayer == rhs.curPlayer && this->board == rhs.board;
    }
    bool operator!=(const Board& rhs) const {
        return !operator==(rhs);
    }
    bool operator<(const Board& rhs) const {
        return this->curPlayer < rhs.curPlayer;
    }
    bool operator>(const Board& rhs) const {
        return operator!=(rhs) && !operator<(rhs);
    }
    Board& operator=(Board& rhs) {
        this->board = rhs.board;
        this->curPlayer = rhs.curPlayer;
        this->discsCnt = rhs.discsCnt;
        return *this;
    }
    constexpr std::array<int, SIZE> operator[](int idx) const {
        return this->board[idx];
    }
    const std::array<Point, 8> directions{{Point(-1, -1), Point(-1, 0), Point(-1, 1),
                                           Point(0, -1), /*{0, 0}, */ Point(0, 1),
                                           Point(1, -1), Point(1, 0), Point(1, 1)}};
    enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };

   private:
    State board;
    int curPlayer;
    int value;
    std::array<int, 3> discsCnt;

   public:
    Board() {
        value = 0;
        discsCnt = {0, 0, 0};
    }
    Board(const Board& other) {
        Board();
        this->board = other.board;
        this->curPlayer = other.curPlayer;
        this->discsCnt = other.discsCnt;
    }
    ~Board() = default;
    void initialize(int player) {
        this->curPlayer = player;

        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                discsCnt[board[i][j]]++;
            }
        }
    }

    void flip_player() {
        this->curPlayer = this->get_next_player();
    }
    int get_player() const {
        return this->curPlayer;
    }
    int get_next_player() const {
        return 3 - this->curPlayer;
    }
    bool is_spot_on_board(Point p) const {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const {
        return board[p.x][p.y];
    }
    void set_disc(Point p) {
        set_disc(p, this->curPlayer);
    }
    void set_disc(Point p, int disc) {
        discsCnt[get_disc(p)]--;
        discsCnt[disc]++;
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
            if (!is_disc_at(p, get_next_player()))
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
            if (!is_disc_at(p, get_next_player()))
                continue;
            std::vector<Point> discs({p});
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, curPlayer)) {
                    for (Point s : discs) {
                        set_disc(s, curPlayer);
                    }
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
    }
    std::set<Point> get_valid_spots() const {
        std::set<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.insert(p);
            }
        }
        return valid_spots;
    }
    bool is_terminal() const {
        int cnt = discsCnt[1] + discsCnt[2];
        if (cnt == 64) return true;
        return cnt == 64;
    }
    int calc_value() {
        this->value = discsCnt[curPlayer] - discsCnt[get_next_player()];
        return this->value;
    }
    int calc_value(const State& weight) {
        int val = 0;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (board[i][j] == curPlayer)
                    val += weight[i][j];
                else if (board[i][j] == get_next_player())
                    val -= weight[i][j];
            }
        }
        return this->value = val;
    }
    int get_value() const {
        return this->value;
    }
};

std::ostream& operator<<(std::ostream& os, const Point& rhs) {
    os << "(" << rhs.x << "," << rhs.y << ")";
    return os;
}
std::ostream& operator<<(std::ostream& os, const Board& rhs) {
    os << "+---------------+\n";
    for (int i = 0; i < SIZE; i++) {
        os << "|";
        for (int j = 0; j < SIZE; j++) {
            switch (rhs[i][j]) {
                case 1:
                    os << "O";
                    break;
                case 2:
                    os << "X";
                    break;
                default:
                    os << " ";
                    break;
            }
            if (j != SIZE - 1) os << " ";
        }
        os << "|\n";
    }
    os << "+---------------+\n";
    return os;
}

/**
 * @brief Input output dealer
 * 
 */
struct Engine {
    static std::ifstream fin;
    static std::ofstream fout;
    static int curPlayer;
    static Board curBoard;
    static std::set<Point> nxtSpots;

    static void read_board() {
        fin >> curPlayer;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                fin >> curBoard.board[i][j];
            }
        }
    }
    static void read_spots() {
        int n_valid_spots;
        fin >> n_valid_spots;
        int x, y;
        for (int i = 0; i < n_valid_spots; i++) {
            fin >> x >> y;
            nxtSpots.insert({x, y});
        }
    }
    static void write_spot(Point pt) {
        fout << pt.x << " " << pt.y << std::endl;
        LOG() << "Moved: " << pt << "\n";
        fout.flush();
    }
};
std::ifstream Engine::fin;
std::ofstream Engine::fout;
int Engine::curPlayer;
Board Engine::curBoard;
std::set<Point> Engine::nxtSpots;

/**
 * @brief Virtual Base Class Of the methods, Stores: curPlayer, curBoard, nxtSpots
 * 
 */
class AIMethod {
   protected:
    const State WEIGHT = {{
        {100, 5, 80, 60, 60, 80, 5, 100},
        {5, 0, 10, 10, 10, 10, 0, 5},
        {80, 10, 20, 20, 20, 20, 10, 80},
        {60, 10, 20, 20, 20, 20, 10, 60},
        {60, 10, 20, 20, 20, 20, 10, 60},
        {80, 10, 20, 20, 20, 20, 10, 80},
        {5, 0, 10, 10, 10, 10, 0, 5},
        {100, 5, 80, 60, 60, 80, 5, 100},
    }};
    const State PT_WEIGHT = {{
        {1100, 0, 750, 750, 750, 750, 0, 1100},
        {0, 0, 250, 250, 250, 250, 0, 0},
        {750, 250, 500, 500, 500, 500, 250, 750},
        {750, 250, 500, 500, 500, 500, 250, 750},
        {750, 250, 500, 500, 500, 500, 250, 750},
        {750, 250, 500, 500, 500, 500, 250, 750},
        {0, 0, 250, 250, 250, 250, 0, 0},
        {1100, 0, 750, 750, 750, 750, 0, 1100},
    }};
    int curPlayer;
    Board curBoard;
    std::set<Point> nxtSpots;

   public:
    AIMethod() {
        this->curPlayer = Engine::curPlayer;
        this->curBoard = Engine::curBoard;
        this->nxtSpots = Engine::nxtSpots;
    }
    virtual ~AIMethod() {}
    virtual void solve() = 0;
};

class AIRandom : public AIMethod {
   public:
    AIRandom() {
        LOG() << "Method: Random\n";
    }
    void solve() {
        int n_valid_spots = nxtSpots.size();
        srand(time(NULL));
        int index = (rand() % n_valid_spots);
        for (auto it = nxtSpots.begin(); it != nxtSpots.end(); it++) {
            if (index == 0) Engine::write_spot(*it);
            index--;
        }
    }
};
class AIMinimaxV0 : public AIMethod {  // determine with naive value + win or lose
   public:
    AIMinimaxV0() {
        LOG() << "Method: Minimax v01\n";
    }
    void solve() override {
        this->getMinimaxVal(this->curBoard, 0);
    }

   private:
    const int MAXDEPTH = 5;
    int getMinimaxVal(Board curBoard, int depth) const {
        if (depth >= MAXDEPTH) {
            return curBoard.calc_value();
        } else if (curBoard.is_terminal()) {
            curBoard.calc_value();
            if (curBoard.get_value() > 0) return MAX;
            if (curBoard.get_value() < 0) return MIN;
            if (curBoard.get_value() == 0) return 0;
        }

        std::set<Point> nxtSpots = (depth == 0 ? this->nxtSpots : curBoard.get_valid_spots());
        int maxVal = MIN;
        for (auto spot : nxtSpots) {
            Board nxtBoard = curBoard;
            int nxtVal;
            nxtBoard.set_disc(spot);
            nxtBoard.flip_discs(spot);
            nxtBoard.flip_player();
            nxtVal = -this->getMinimaxVal(nxtBoard, depth + 1);
            if (nxtVal > maxVal) {
                if (depth == 0) {
                    Engine::write_spot(spot);
                    LOG() << "Value of: " << spot << ": " << nxtVal << "\n";
                }
                maxVal = nxtVal;
            }
        }
        return maxVal;
    }
};
class AIMinimaxV1 : public AIMethod {  // determine with target spot + win or lose
   public:
    AIMinimaxV1() {
        LOG() << "Method: Minimax Enhanced v1\n";
    }
    void solve() override {
        this->getMinimaxVal(this->curBoard, 0);
    }

   private:
    const int MAXDEPTH = 5;
    int getMinimaxVal(Board curBoard, int depth) const {
        if (curBoard.is_terminal()) {
            curBoard.calc_value();
            if (curBoard.get_value() > 0) return MAX;
            if (curBoard.get_value() < 0) return MIN;
            if (curBoard.get_value() == 0) return 0;
        } else if (depth >= MAXDEPTH) {
            return 0;
        }

        std::set<Point> nxtSpots = (depth == 0 ? this->nxtSpots : curBoard.get_valid_spots());
        int maxVal = MIN;
        for (auto spot : nxtSpots) {
            Board nxtBoard = curBoard;
            int nxtVal;
            nxtBoard.set_disc(spot);
            nxtBoard.flip_discs(spot);
            nxtBoard.flip_player();
            nxtVal = -this->getMinimaxVal(nxtBoard, depth + 1);
            nxtVal = std::max(nxtVal, nxtVal + PT_WEIGHT[spot.x][spot.y]);
            if (nxtVal > maxVal) {
                if (depth == 0) {
                    Engine::write_spot(spot);
                    LOG() << "Value of: " << spot << ": " << nxtVal << "\n";
                }
                maxVal = nxtVal;
            }
        }
        return maxVal;
    }
};
class AIMinimaxV2 : public AIMethod {  // determine with target spot + board weight + win or lose
   public:
    AIMinimaxV2() {
        LOG() << "Method: Minimax Enhanced v2\n";
    }
    void solve() override {
        this->getMinimaxVal(this->curBoard, 0);
    }

   private:
    // 5 is max
    const int MAXDEPTH = 5;
    int getMinimaxVal(Board curBoard, int depth) const {
        int action;  // 0:normal 1:terminal 2:depthmax
        std::set<Point> nxtSpots;
        std::set<Point> nxt2Spots;
        Board nxtBoard;
        int maxVal, nxtVal;

        action = 0;
        if (curBoard.is_terminal()) {
            action = 1;
        } else if (depth >= MAXDEPTH) {
            action = 2;
        }

        maxVal = MIN;
        if (action == 0) {
            nxtSpots = (depth == 0 ? this->nxtSpots : curBoard.get_valid_spots());
            if (nxtSpots.empty()) {
                nxtBoard = curBoard;
                nxtBoard.flip_player();
                nxt2Spots = nxtBoard.get_valid_spots();
                if (nxt2Spots.empty())
                    action = 1;
            }
        }

        if (action == 0) {
            for (auto spot : nxtSpots) {
                nxtBoard = curBoard;

                nxtBoard.set_disc(spot);
                nxtBoard.flip_discs(spot);
                nxtBoard.flip_player();
                nxtVal = -this->getMinimaxVal(nxtBoard, depth + 1);
                if (nxtVal > maxVal) {
                    if (depth == 0) {
                        Engine::write_spot(spot);
                        LOG() << "Value of " << curBoard.get_player() << "-" << nxtBoard.get_player() << " " << spot << " is " << nxtVal << "\n";
                    }
                    maxVal = nxtVal;
                }
            }
        }

        if (action == 0) {
            if (maxVal < -0x7ffffff) LOG() << "!!! Death End !!!\n";
            return maxVal;
        } else if (action == 1) {
            curBoard.calc_value();
            if (curBoard.get_value() > 0) return MAX;
            if (curBoard.get_value() < 0) return MIN;
            if (curBoard.get_value() == 0) return 0;
        } else if (action == 2) {
            return curBoard.calc_value(WEIGHT);
        }
        return 0;
    }
};
class AIAlphaBetaPruning : public AIMethod {
   public:
    AIAlphaBetaPruning() {
        LOG() << "Method: Alpha-Beta Pruning v0.1\n";
    }
    void solve() override {
        this->getAlphaBetaVal(this->curBoard, 0, MIN, MAX);
        // int maxVal = MIN;
        // int alpha = MIN;
        // int beta = MAX;
        // for (auto spot : nxtSpots) {  // Initialize BFS tasks
        //     Board nxtBoard = curBoard;
        //     int nxtVal;
        //     nxtBoard.set_disc(spot);
        //     nxtBoard.flip_discs(spot);
        //     nxtBoard.calc_value(WEIGHT);
        //     nxtBoard.flip_player();
        //     nxtVal = this->getAlphaBetaVal(nxtBoard, 1, -beta, -alpha);
        //     if (nxtVal == MIN)
        //         nxtVal = MAX;
        //     else if (nxtVal == MAX)
        //         nxtVal = MIN;
        //     else
        //         nxtVal = nxtBoard.get_value() + PT_WEIGHT[spot.x][spot.y] - nxtVal;
        //     if (nxtVal >= maxVal) {
        //         maxVal = nxtVal;
        //     }
        //     LOG() << "Value of " << spot << ": " << nxtVal << "\n";
        //     alpha = std::max(alpha, maxVal);
        //     LOG() << "alpha: " << alpha << ", beta: " << beta << "\n";
        //     if (beta <= alpha)
        //         break;
        // }
    }

   private:
    const int MAXDEPTH = 6;
    int getAlphaBetaVal(Board curBoard, int depth, int alpha, int beta) const {
        if (depth > MAXDEPTH) return 0;

        std::set<Point> nxtSpots = (depth == 0 ? this->nxtSpots : curBoard.get_valid_spots());
        int maxVal = MIN;
        for (auto spot : nxtSpots) {
            Board nxtBoard = curBoard;
            int nxtVal;
            nxtBoard.set_disc(spot);
            nxtBoard.flip_discs(spot);
            nxtBoard.calc_value(WEIGHT);
            nxtBoard.flip_player();
            nxtVal = this->getAlphaBetaVal(nxtBoard, depth + 1, -beta, -alpha);
            if (nxtVal == MIN)
                nxtVal = MAX;
            else if (nxtVal == MAX)
                nxtVal = MIN;
            else
                nxtVal = nxtBoard.get_value() + PT_WEIGHT[spot.x][spot.y] - nxtVal;

            if (nxtVal > maxVal) {
                LOG() << "Depth: " << depth << ",Value of: " << spot << " tot: " << nxtVal << ", board: " << nxtBoard.get_value() << ", point: " << PT_WEIGHT[spot.x][spot.y] << "\n";
                if (depth == 0) {
                    Engine::write_spot(spot);
                }
                maxVal = nxtVal;
                alpha = std::max(alpha, maxVal);
                if (beta <= alpha)
                    break;
            }
        }
        maxVal = std::max(maxVal, maxVal + int(nxtSpots.size()) * 1000);

        return maxVal;
    }
};

int main(int, char** argv) {
    if (DEBUG) {
        LOCALTIME::start = std::chrono::system_clock::now();
        LOG::fout.open("debuglog.txt", std::ofstream::ate | std::ofstream::app);
        LOG() << "╰(*°▽°*)╯(❁´◡`❁)(●'◡'●)\n";
    }

    Engine::fin.open(argv[1]);                      // Input file stream
    Engine::fout.open(argv[2]);                     // Output file stream
    Engine::read_board();                           // Read the board from fin
    Engine::read_spots();                           // Read Valid inputs from fin
    Engine::write_spot(*Engine::nxtSpots.begin());  // Output once first to prevent running too long
    Engine::curBoard.initialize(Engine::curPlayer);

    if (DEBUG) {
        LOG() << "Current Player: " << (Engine::curPlayer == 1 ? "O" : "X") << "\n";
        LOG() << "Current Board:\n";
        LOG() << Engine::curBoard;
        LOG() << "Valid moves: ";
        for (auto it = Engine::nxtSpots.begin(); it != Engine::nxtSpots.end(); it++) {
            auto nxtit = it;
            LOG() << (*it) << (++nxtit != Engine::nxtSpots.end() ? ", " : "\n");
        }
        LOG() << "Time initializing: " << LOCALTIME::get_duration().count() << "s\n";
    }

    // Start AI;
    AIMethod* aiMethod = new AIMinimaxV2();
    aiMethod->solve();
    delete aiMethod;

    if (DEBUG) {
        LOG() << "Duration: " << LOCALTIME::get_duration().count() << "s\n";
        LOG() << "=================\n";
        LOG::fout.close();
    }

    Engine::fin.close();
    Engine::fout.close();
    return 0;
}