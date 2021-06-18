#include <bits/stdc++.h>
#define DEBUG 1
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
        return *this;
    }
    const constexpr std::array<int, SIZE> operator[](int idx) const {
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
    State board;

   private:
    int curPlayer;
    int value;

   public:
    Board() {
        value = std::numeric_limits<int>::min();
    }
    Board(const Board& other) {
        Board();
        this->board = other.board;
        this->curPlayer = other.curPlayer;
    }

    ~Board() = default;

    void set_player(int player) {
        this->curPlayer = player;
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
            os << " ";
        }
        os << "|\n";
    }
    os << "+---------------+\n";
    return os;
}

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
        {1000, 5, 80, 60, 60, 80, 5, 1000},
        {5, 0, 10, 10, 10, 10, 0, 5},
        {80, 10, 20, 20, 20, 20, 10, 80},
        {60, 10, 20, 20, 20, 20, 10, 60},
        {60, 10, 20, 20, 20, 20, 10, 60},
        {80, 10, 20, 20, 20, 20, 10, 80},
        {5, 0, 10, 10, 10, 10, 0, 5},
        {1000, 5, 80, 60, 60, 80, 5, 1000},
    }};
    const State PT_WEIGHT = {{
        {10000, -1000, 10, 10, 10, 10, -1000, 10000},
        {-1000, -1000, 0, 0, 0, 0, -1000, -1000},
        {10, 0, 100, 100, 100, 100, 0, 10},
        {10, 0, 100, 100, 100, 100, 0, 10},
        {10, 0, 100, 100, 100, 100, 0, 10},
        {10, 0, 100, 100, 100, 100, 0, 10},
        {-1000, -1000, 0, 0, 0, 0, -1000, -1000},
        {10000, -1000, 10, 10, 10, 10, -1000, 10000},
    }};
    int curPlayer;
    Board curBoard;
    std::set<Point> nxtSpots;

   public:
    AIMethod() {}
    virtual ~AIMethod() {}
    virtual void initialize(int& player, Board& board, std::set<Point>& next_spots) {
        this->curPlayer = player;
        this->curBoard = board;
        this->nxtSpots = next_spots;
    }
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
class AIMinimax : public AIMethod {
   public:
    AIMinimax() {
        LOG() << "Method: Minimax\n";
    }
    void solve() override {
        this->getMinimaxVal(this->curBoard, 0);
        // Point finSpot;
        // int maxVal = INT_MIN;
        // for (auto spot : nxtSpots) {  // Initialize BFS tasks
        //     Board nxtBoard = curBoard;
        //     int nxtVal;
        //     nxtBoard.set_disc(spot);
        //     nxtBoard.flip_discs(spot);
        //     nxtBoard.calc_value(WEIGHT);
        //     nxtBoard.flip_player();
        //     nxtVal = this->getMinimaxVal(nxtBoard, 1);
        //     if (nxtVal == INT_MIN)
        //         nxtVal = INT_MAX;
        //     else if (nxtVal == INT_MAX)
        //         nxtVal = INT_MIN;
        //     else
        //         nxtVal = nxtBoard.get_value() - nxtVal;
        //     if (nxtVal >= maxVal) {
        //         finSpot = spot;
        //         maxVal = nxtVal;
        //     }
        //     LOG() << "Value of (" << spot.x << ", " << spot.y << "): " << nxtVal << "\n";
        // }
    }

   private:
    const int MAXDEPTH = 5;
    /// @todo Fix the minimax the part of mini
    int getMinimaxVal(Board curBoard, int depth) const {
        if (depth > MAXDEPTH) return 0;

        std::set<Point> nxtSpots;
        int maxVal = INT_MIN;

        nxtSpots = (depth == 0 ? this->nxtSpots : curBoard.get_valid_spots());
        for (auto spot : nxtSpots) {
            Board nxtBoard = curBoard;
            int nxtVal;
            nxtBoard.set_disc(spot);
            nxtBoard.flip_discs(spot);
            nxtBoard.calc_value(WEIGHT);
            nxtBoard.flip_player();
            nxtVal = this->getMinimaxVal(nxtBoard, depth + 1);
            if (nxtVal == INT_MIN)
                nxtVal = INT_MAX;
            else if (nxtVal == INT_MAX)
                nxtVal = INT_MIN;
            else
                nxtVal = nxtBoard.get_value() - nxtVal;

            if (nxtVal > maxVal) {
                if (depth == 0) Engine::write_spot(spot);
                maxVal = nxtVal;
            }
        }
        if (nxtSpots.empty()) maxVal = -10000;

        return maxVal;
    }
};
class AIAlphaBetaPruning : public AIMethod {
   public:
    AIAlphaBetaPruning() {
        LOG() << "Method: Alpha-Beta Pruning\n";
    }
    void solve() override {
        this->getAlphaBetaVal(this->curBoard, 0, INT_MIN, INT_MAX);
        // int maxVal = INT_MIN;
        // int alpha = INT_MIN;
        // int beta = INT_MAX;
        // for (auto spot : nxtSpots) {  // Initialize BFS tasks
        //     Board nxtBoard = curBoard;
        //     int nxtVal;
        //     nxtBoard.set_disc(spot);
        //     nxtBoard.flip_discs(spot);
        //     nxtBoard.calc_value(WEIGHT);
        //     nxtBoard.flip_player();
        //     nxtVal = this->getAlphaBetaVal(nxtBoard, 1, -beta, -alpha);
        //     if (nxtVal == INT_MIN)
        //         nxtVal = INT_MAX;
        //     else if (nxtVal == INT_MAX)
        //         nxtVal = INT_MIN;
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
        if (depth > MAXDEPTH + (curBoard.get_player() == 2 ? 1 : 0)) return 0;

        std::set<Point> nxtSpots = (depth == 0 ? this->nxtSpots : curBoard.get_valid_spots());
        int maxVal = INT_MIN;
        for (auto spot : nxtSpots) {
            Board nxtBoard = curBoard;
            int nxtVal;
            nxtBoard.set_disc(spot);
            nxtBoard.flip_discs(spot);
            nxtBoard.calc_value(WEIGHT);
            nxtBoard.flip_player();
            nxtVal = this->getAlphaBetaVal(nxtBoard, depth + 1, -beta, -alpha);
            if (nxtVal == INT_MIN)
                nxtVal = INT_MAX;
            else if (nxtVal == INT_MAX)
                nxtVal = INT_MIN;
            else
                nxtVal = nxtBoard.get_value() + PT_WEIGHT[spot.x][spot.y] - nxtVal;

            if (nxtVal > maxVal) {
                if (depth == 0) {
                    LOG() << "Value of: " << spot << " tot: " << nxtVal << ", board: " << nxtBoard.get_value() << ", point: " << PT_WEIGHT[spot.x][spot.y] << "\n";
                    Engine::write_spot(spot);
                }
                maxVal = nxtVal;
            }

            // alpha-beta
            alpha = std::max(alpha, maxVal);
            if (beta <= alpha)
                break;
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

    AIMethod* aiMethod = new AIAlphaBetaPruning();
    Engine::fin.open(argv[1]);                      // Input file stream
    Engine::fout.open(argv[2]);                     // Output file stream
    Engine::read_board();                           // Read the board from fin
    Engine::read_spots();                           // Read Valid inputs from fin
    Engine::write_spot(*Engine::nxtSpots.begin());  // Output once first to prevent running too long
    Engine::curBoard.set_player(Engine::curPlayer);

    if (DEBUG) {
        LOG() << "Current Player: " << (Engine::curPlayer == 1 ? "O" : "X") << "\n";
        LOG() << "Current Board:\n";
        LOG() << Engine::curBoard;
        LOG() << "Valid moves:\n";
        for (auto it = Engine::nxtSpots.begin(); it != Engine::nxtSpots.end(); it++) {
            auto nxtit = it;
            LOG() << (*it) << (++nxtit != Engine::nxtSpots.end() ? ", " : "\n");
        }
        LOG() << "Time initializing: " << LOCALTIME::get_duration().count() << "s\n";
    }

    // Start AI;
    aiMethod->initialize(Engine::curPlayer, Engine::curBoard, Engine::nxtSpots);
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