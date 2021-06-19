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
        return this->board == rhs.board;
    }
    bool operator!=(const Board& rhs) const {
        return !operator==(rhs);
    }
    Board& operator=(const Board& rhs) {  // REMEMBER change when add new variables
        this->board = rhs.board;
        this->discsCnt = rhs.discsCnt;
        return *this;
    }
    const constexpr std::array<int, SIZE> operator[](int idx) const {
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
    std::array<int, 3> discsCnt;

   public:
    Board() {
        discsCnt = {0, 0, 0};
    }
    Board(const Board& other) {
        Board();
        operator=(other);
    }
    ~Board() = default;
    void initialize() {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                discsCnt[board[i][j]]++;
            }
        }
    }

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
        discsCnt[get_disc(p)]--;
        discsCnt[disc]++;
        board[p.x][p.y] = disc;
    }
    void flip_discs(Point center) {
        int player = get_disc(center);
        for (Point dir : directions) {
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(player)))
                continue;
            std::vector<Point> discs({p});
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, player)) {
                    for (Point s : discs) {
                        set_disc(s, player);
                    }
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
    }
    void set_move(Point p, int player) {
        set_disc(p, player);
        flip_discs(p);
    }
    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center, int player) const {
        if (get_disc(center) != EMPTY)
            return false;
        for (Point dir : directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(player)))
                continue;
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, player))
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
    std::set<Point> get_valid_spots(int player) const {
        std::set<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p, player))
                    valid_spots.insert(p);
            }
        }
        return valid_spots;
    }
    bool is_terminal() const {
        if (this->discsCnt[0] == 0) return true;
        int toCheck = discsCnt[0];
        for (int i = 0; i < SIZE && toCheck; i++) {
            for (int j = 0; j < SIZE && toCheck; j++) {
                if (board[i][j] != EMPTY)
                    continue;
                toCheck--;
                Point curPt = Point(i, j);
                for (auto dir : directions) {
                    Point p = curPt + dir;
                    int firstPly = get_disc(p);
                    while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                        if (get_disc(p) != firstPly) return false;
                        p = p + dir;
                    }
                }
            }
        }
        return true;
    }
    int get_cnt_discs(int i) const {
        return discsCnt[i];
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

   private:
    const State WEIGHT_EARLY = {{
        {100, -100, 80, 50, 50, 80, -100, 100},
        {-100, -100, 0, 0, 0, 0, -100, -100},
        {80, 0, 20, 20, 20, 20, 0, 80},
        {60, 0, 20, 20, 20, 20, 0, 60},
        {60, 0, 20, 20, 20, 20, 0, 60},
        {80, 0, 20, 20, 20, 20, 0, 80},
        {-100, -100, 0, 0, 0, 0, -100, -100},
        {100, -100, 80, 50, 50, 80, -100, 100},
    }};
    const State WEIGHT_MID = {{
        {1000, -300, 100, 100, 100, 100, -300, 1000},
        {-300, -500, 0, 0, 0, 0, -500, -300},
        {100, 0, 0, 0, 0, 0, 0, 100},
        {100, 0, 0, 0, 0, 0, 0, 100},
        {100, 0, 0, 0, 0, 0, 0, 100},
        {100, 0, 0, 0, 0, 0, 0, 100},
        {-300, -500, 0, 0, 0, 0, -500, -300},
        {1000, -300, 100, 100, 100, 100, -300, 1000},
    }};
    const State PT_SCORE = {{
        {110, 00, 75, 75, 75, 75, 00, 110},
        {00, 00, 25, 25, 25, 25, 00, 00},
        {75, 25, 50, 50, 50, 50, 25, 75},
        {75, 25, 50, 50, 50, 50, 25, 75},
        {75, 25, 50, 50, 50, 50, 25, 75},
        {75, 25, 50, 50, 50, 50, 25, 75},
        {00, 00, 25, 25, 25, 25, 00, 00},
        {110, 00, 75, 75, 75, 75, 00, 110},
    }};

   protected:
    int sum(int a, int b) const {
        if (a < 0 && b < 0 && a + b > 0) return MIN;
        if (a > 0 && b > 0 && a + b < 0) return MAX;
        return a + b;
    }
    int get_ptval(Point p) const {
        return PT_SCORE[p.x][p.y];
    }
    int get_stage(Board& board, int player) const {
        std::vector<int> row = {0, 1, 6, 7};
        int stage = 1;
        for (auto i : row) {
            for (int j = 0; j < SIZE && stage == 1; j++) {
                if (board[i][j] == player && WEIGHT_EARLY[i][j] <= 0) stage = 2;
            }
            if (stage != 1) break;
        }
        return stage;
    }
    int evaluate(Board& board, int player) const {
        int stage = get_stage(board, player);
        int val = 0;

        if (stage == 1) {
            val += board.get_valid_spots(player).size() * 100;
        } else {
            for (int i = 0; i < SIZE; i++) {
                for (int j = 0; j < SIZE; j++) {
                    if (board[i][j] == player)
                        val += WEIGHT_MID[i][j];
                    else if (board[i][j] == 3 - player)
                        val -= WEIGHT_MID[i][j];
                }
            }
        }
        return val;
    }
};
class AIAlphaBetaPruning : public AIMethod {  // determine with target spot + board weight + win or lose
   public:
    AIAlphaBetaPruning() {
        LOG() << "Method: Alphabeta Pruning v1\n";
    }
    void solve() override {
        this->getAlphaBetaVal(this->curBoard, 0, MIN, MAX, this->curPlayer);
    }

   private:
    const int MAXDEPTH = 7;
    int getAlphaBetaVal(Board curBoard, int depth, int alpha, int beta, int player) const {
        if (curBoard.is_terminal()) {
            int a = curBoard.get_cnt_discs(player);
            int b = curBoard.get_cnt_discs(3 - player);
            if (a > b) return MAX;
            if (a < b) return MIN;
            if (a == b) return 0;
        } else if (depth >= MAXDEPTH) {
            return evaluate(curBoard, player);
        }

        std::set<Point> nxtSpots;
        Board nxtBoard;
        int maxVal, nxtVal;

        maxVal = MIN;
        nxtSpots = (depth == 0 ? this->nxtSpots : curBoard.get_valid_spots(player));

        for (auto spot : nxtSpots) {
            nxtBoard = curBoard;
            nxtBoard.set_move(spot, player);
            nxtVal = -this->getAlphaBetaVal(nxtBoard, depth + 1, -beta, -alpha, 3 - player);
            if (nxtVal > maxVal) {
                if (depth == 0) {
                    Engine::write_spot(spot);
                    LOG() << "Value of " << player << "-" << 3 - player << " " << spot << " is " << nxtVal << "\n";
                }
                maxVal = nxtVal;
            }
            alpha = std::max(alpha, maxVal);
            if (beta <= alpha) break;
        }
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
    Engine::curBoard.initialize();

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
    AIMethod* aiMethod = new AIAlphaBetaPruning();
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