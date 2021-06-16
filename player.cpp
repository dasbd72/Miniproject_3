#include <bits/stdc++.h>
#define DEBUG 0
const int SIZE = 8;
using State = std::array<std::array<int, SIZE>, SIZE>;

/// @brief Call the class as cout, ex: LOG() << "Hello World";
class LOG {
   private:
    static std::ofstream fout;

   public:
    LOG() {}
    ~LOG() {}
    template <class T>
    LOG& operator<<(const T& msg) {
        if (DEBUG) {
            fout << msg;
        }
        return *this;
    }
    static void initialize() {
        if (DEBUG) {
            auto startTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            fout.open("debuglog.txt", std::ofstream::app);
            fout << "Start Time: " << std::ctime(&startTime);
        }
    }
    static void terminate() {
        if (DEBUG) {
            auto endTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            fout << "End Time: " << std::ctime(&endTime);
            fout << "----------------------------------------\n";
            fout.close();
        }
    }
};
std::ofstream LOG::fout;

/// @brief Stores x and y
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
        return x < rhs.x || !(x > rhs.x) || (x == rhs.x && (y < rhs.y || !(y >= rhs.y)));
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

/// @brief Board class
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

    int curPlayer;
    State board;

   public:
    Board() {}
    Board(const Board& board) {
        this->board = board.board;
        this->curPlayer = board.curPlayer;
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

    void set_disc(Point p) {
        board[p.x][p.y] = curPlayer;
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

    int calc_weighted_state(const State& weight) const {
        int val = 0;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (board[i][j] == curPlayer) val += weight[i][j];
            }
        }
        return val;
    }
};

std::ostream& operator<<(std::ostream& os, const Point& rhs) {
    os << "(" << rhs.x << ", " << rhs.y << ")";
    return os;
}
std::ostream& operator<<(std::ostream& os, const Board& rhs) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            os << rhs[i][j] << " ";
        }
        os << std::endl;
    }
    return os;
}

/// @brief Virtual Base Class Of the methods
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
    int curPlayer;
    Board curBoard;
    std::set<Point> nxtSpots;

   public:
    AIMethod() {}
    virtual ~AIMethod() {}
    virtual void __init__(int& player, Board& board, std::set<Point>& next_spots) {
        this->curPlayer = player;
        this->curBoard = board;
        this->nxtSpots = next_spots;
    }
    virtual Point solve() = 0;
};

class AIRandom : public AIMethod {
   public:
    Point solve() {
        int n_valid_spots = nxtSpots.size();
        srand(time(NULL));
        int index = (rand() % n_valid_spots);
        for (auto it = nxtSpots.begin(); it != nxtSpots.end(); it++) {
            if (!index) return *it;
            index--;
        }
        return *nxtSpots.begin();
    }
};
class AIStateValueFunction : public AIMethod {
   public:
    Point solve() override {
        Point finSpot;
        int maxValue = -1;
        for (auto spot : nxtSpots) {
            Board nxtBoard = curBoard;
            int nxtValue = 0;
            nxtBoard.set_disc(spot);
            nxtBoard.flip_discs(spot);
            nxtValue = nxtBoard.calc_weighted_state(WEIGHT);
            if (nxtValue > maxValue) {
                nxtValue = maxValue;
                finSpot = spot;
            }
            LOG() << "Value of (" << spot.x << ", " << spot.y << "): " << nxtValue << "\n";
        }
        return finSpot;
    }
};
class AIMinmax : public AIMethod {
   public:
    Point solve() override {
        std::queue<Board> tasks;
        Point finSpot;
    }
};
class Player {
   private:
    std::ifstream fin;
    std::ofstream fout;

    int curPlayer;
    Board curBoard;
    std::set<Point> nxtSpots;
    Point finSpot;

   public:
    Player(int argc, char** argv) {
        if (argc >= 3) {
            fin.open(argv[1]);   // Input file stream
            fout.open(argv[2]);  // Output file stream
        }
    }
    ~Player() {
        fin.close();
        fout.close();
    }

    void start(AIMethod* aiMethod) {
        this->read_board(fin);        // Read the board from fin
        this->read_valid_spots(fin);  // Read Valid inputs from fin
        write_valid_spot(fout);       // Output once first to prevent running too long

        LOG() << "Current Board:\n";
        LOG() << curBoard;
        LOG() << "Valid moves:\n";
        for (auto it = nxtSpots.begin(); it != nxtSpots.end(); it++) {
            auto nxtit = it;
            LOG() << (*it) << (++nxtit != nxtSpots.end() ? ", " : "\n");
        }

        if (aiMethod) {
            aiMethod->__init__(curPlayer, curBoard, nxtSpots);
            this->finSpot = aiMethod->solve();
            delete aiMethod;
        }

        this->write_valid_spot(fout);  // Output My Move
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
            this->nxtSpots.insert({x, y});
        }
    }

    void write_valid_spot(std::ofstream& fout) {
        if (nxtSpots.find(finSpot) != nxtSpots.end())
            fout << finSpot.x << " " << finSpot.y << std::endl;
        else
            fout << (*nxtSpots.begin()).x << " " << (*nxtSpots.begin()).y << std::endl;
        fout.flush();
    }
};

int main(int argc, char** argv) {
    if (DEBUG) LOG::initialize();
    Player player(argc, argv);
    player.start(new AIStateValueFunction());
    if (DEBUG) LOG::terminate();
    return 0;
}