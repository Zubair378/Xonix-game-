//Muhammd Zubair(24i2095)
//Basit Hafeez(24i2129)
//M Kashan Ali(24i2135)
//Xonix

    
    #include <SFML/Graphics.hpp>
    #include <ctime>
    #include <cstdlib>
    #include <iostream>
    #include <fstream>
    #include <string>

    using namespace sf;
    using namespace std;

    // Grid and game constants
    const int M = 25;
    const int N = 40;
    int grid[M][N] = {0};
    int ts = 18;
    const int HUD_HEIGHT = 50;
        // Background frames
        const int BG_MAX_FRAMES = 50;

        Texture bgTextures[BG_MAX_FRAMES];

        int   bgFrameCount      = 0;
    int   currentBgFrame    = 0;
    float bgFrameTime       = 0.f;
    const float BG_FRAME_DURATION = 0.04f;




    // Enemy struct
    struct Enemy {
        int x, y, dx, dy;
        Enemy() {
            x = y = 300;
            dx = 8 - rand() % 16;
            dy = 8 - rand() % 16;
        }

        void move() {
            x += dx;
            if (grid[y/ts][x/ts] == 1) { dx = -dx; x += dx; }
            y += dy;
            if (grid[y/ts][x/ts] == 1) { dy = -dy; y += dy; }
        }
    };

    // Flood fill
// Iterative DFS (custom stack)
void drop(int yy, int xx) {
    if (yy <= 0 || yy >= M - 1 || xx <= 0 || xx >= N - 1) return;
    if (grid[yy][xx] != 0) return;
    
    // Manual stack arrays
    int stackY[M * N];
    int stackX[M * N];
    int top = -1;
    
    // push start
    top++;
    stackY[top] = yy;
    stackX[top] = xx;
    grid[yy][xx] = -1;

    const int dy[4] = { -1, 1, 0, 0 };
    const int dx[4] = { 0, 0, -1, 1 };

    while (top >= 0) {
        int y = stackY[top];
        int x = stackX[top];
        top--;

        for (int i = 0; i < 4; i++) {
            int ny = y + dy[i];
            int nx = x + dx[i];

            if (ny <= 0 || ny >= M - 1 || nx <= 0 || nx >= N - 1) continue;
            if (grid[ny][nx] == 0) {
                grid[ny][nx] = -1;
                top++;
                stackY[top] = ny;
                stackX[top] = nx;
            }
        }
    }
}


    // Player and profile
    struct Player {
        string username;
        string password;
        int totalPoints;
        int bestScore;
        int wins;
        int losses;
        int powerUps; 
        int themeId;  

        Player() {
            username = "";
            password = "";
            totalPoints = 0;
            bestScore = 0;
            wins = 0;
            losses = 0;
            powerUps = 0;
            themeId = 1;
        }
    };

    struct LeaderboardEntry {
        string username;
        int totalPoints;
    };

   
struct TileNode {
    int y;
    int x;
    TileNode* next;
    TileNode(int yy, int xx) : y(yy), x(xx), next(nullptr) {}
};

struct GameSave {
    int saveId;
    string username;
    int score;
    long timestamp;
    TileNode* tilesHead;

    GameSave() : saveId(0), score(0), timestamp(0), tilesHead(nullptr) {}
};

// yeh function save id se grid wapas load karta hai
bool loadGameById(int id, const string& username) {
    ifstream fin("saves.txt");
    if (!fin) return false;
    int sid, score, count; long ts; string uname;
    while (fin >> sid >> uname >> score >> ts >> count) {
        if (sid == id && uname == username) {
            
            for (int i = 0; i < M; ++i)
                for (int j = 0; j < N; ++j)
                    grid[i][j] = 0;
            for (int i = 0; i < M; ++i)
                for (int j = 0; j < N; ++j)
                    if (i == 0 || j == 0 || i == M-1 || j == N-1)
                        grid[i][j] = 1;

            for (int k = 0; k < count; ++k) {
                int y, x;
                if (!(fin >> y >> x)) break;
                if (y >= 0 && y < M && x >= 0 && x < N)
                    grid[y][x] = 1;
            }
            return true;
        } else {
            
            int y,x; for (int k=0;k<count;++k) fin >> y >> x;
        }
    }
    return false;
}

// yeh function sare land tiles ki linked list banata hai
TileNode* buildTileListFromGrid() {
    TileNode* head = nullptr;
    TileNode* tail = nullptr;

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            if (grid[i][j] == 1) {
                TileNode* node = new TileNode(i, j);
                if (!head) {
                    head = tail = node;
                } else {
                    tail->next = node;
                    tail = node;
                }
            }
        }
    }
    return head;
}

// yeh function next save id return karta hai
int getNextSaveId() {
    ifstream fin("saves.txt");
    if (!fin) return 1;

    int lastId = 0;
    int id, score, count;
    long ts;
    string uname;
    while (true) {
        if (!(fin >> id >> uname >> score >> ts >> count)) break;
        lastId = id;
        int y, x;
        for (int i = 0; i < count; i++) {
            if (!(fin >> y >> x)) break;
        }
    }
    return lastId + 1;
}

// yeh function ek GameSave file me append karta hai
void appendGameSaveToFile(const GameSave& gs) {
    ofstream fout("saves.txt", ios::app);
    if (!fout) return;

    int count = 0;
    for (TileNode* p = gs.tilesHead; p; p = p->next) count++;

    fout << gs.saveId << " "
         << gs.username << " "
         << gs.score << " "
         << gs.timestamp << " "
         << count;

    for (TileNode* p = gs.tilesHead; p; p = p->next) {
        fout << " " << p->y << " " << p->x;
    }
    fout << "\n";
}

// yeh function tile linked list free karta hai
void freeTileList(TileNode* head) {
    TileNode* p = head;
    while (p) {
        TileNode* nxt = p->next;
        delete p;
        p = nxt;
    }
}

    
const int MAX_ALL_PLAYERS = 200;
const int HASH_SIZE = 401;

Player allPlayers[MAX_ALL_PLAYERS];
int allPlayerCount = 0;

int hashTable[HASH_SIZE];

struct FriendNode {
    int friendIndex;
    FriendNode* next;
    FriendNode(int idx) : friendIndex(idx), next(nullptr) {}
};


FriendNode* friendAdj[MAX_ALL_PLAYERS];

// Simple hash function on username
unsigned long hashUsernameString(const string& s) {
    unsigned long h = 0;
    for (size_t i = 0; i < s.size(); i++) {
        h = h * 131u + (unsigned char)s[i];
    }
    return h;
}

// Build hash table and allPlayers[] from players.txt
void initFriendSystem() {
    allPlayerCount = 0;
    for (int i = 0; i < HASH_SIZE; i++) hashTable[i] = -1;
    for (int i = 0; i < MAX_ALL_PLAYERS; i++) friendAdj[i] = nullptr;

    ifstream fin("players.txt");
    while (fin && allPlayerCount < MAX_ALL_PLAYERS) {
        Player p;
        if (!(fin >> p.username >> p.password >> p.totalPoints
               >> p.bestScore >> p.wins >> p.losses >> p.powerUps))
            break;
        allPlayers[allPlayerCount] = p;
        allPlayerCount++;
    }

    
    for (int i = 0; i < allPlayerCount; i++) {
        unsigned long h = hashUsernameString(allPlayers[i].username);
        int pos = (int)(h % HASH_SIZE);
        while (hashTable[pos] != -1) {
            pos++;
            if (pos == HASH_SIZE) pos = 0;
        }
        hashTable[pos] = i;
    }
}

// Find player index in allPlayers[] by username using open addressing
int findPlayerIndexByName(const string& uname) {
    if (allPlayerCount == 0) return -1;
    unsigned long h = hashUsernameString(uname);
    int pos = (int)(h % HASH_SIZE);
    int start = pos;
    while (hashTable[pos] != -1) {
        if (allPlayers[ hashTable[pos] ].username == uname)
            return hashTable[pos];
        pos++;
        if (pos == HASH_SIZE) pos = 0;
        if (pos == start) break;
    }
    return -1;
}

bool alreadyFriends(int a, int b) {
    FriendNode* cur = friendAdj[a];
    while (cur) {
        if (cur->friendIndex == b) return true;
        cur = cur->next;
    }
    return false;
}

void addFriendEdge(int a, int b) {
    if (a < 0 || b < 0 || a >= allPlayerCount || b >= allPlayerCount) return;
    if (alreadyFriends(a, b)) return;
    FriendNode* na = new FriendNode(b);
    na->next = friendAdj[a];
    friendAdj[a] = na;
}

// Load friend pairs from friends.txt (username1 username2) - undirected
void loadFriendsFromFile() {
    ifstream ff("friends.txt");
    if (!ff) return;
    string u1, u2;
    while (ff >> u1 >> u2) {
        int a = findPlayerIndexByName(u1);
        int b = findPlayerIndexByName(u2);
        if (a != -1 && b != -1) {
            addFriendEdge(a, b);
            addFriendEdge(b, a);
        }
    }
}

// Save all friend edges to friends.txt, each pair once
void saveFriendsToFile() {
    ofstream ff("friends.txt");
    if (!ff) return;
    for (int i = 0; i < allPlayerCount; i++) {
        FriendNode* cur = friendAdj[i];
        while (cur) {
            int j = cur->friendIndex;
            if (i < j) {
                ff << allPlayers[i].username << " " << allPlayers[j].username << "\n";
            }
            cur = cur->next;
        }
    }
}



struct FriendRequest {
    int fromIndex;
    int toIndex;
    FriendRequest* next;
    FriendRequest(int f, int t) : fromIndex(f), toIndex(t), next(nullptr) {}
};

FriendRequest* gPendingRequestsHead = nullptr;

// helper: check if a pending request already exists
bool alreadyPending(int fromIdx, int toIdx) {
    FriendRequest* cur = gPendingRequestsHead;
    while (cur) {
        if (cur->fromIndex == fromIdx && cur->toIndex == toIdx)
            return true;
        cur = cur->next;
    }
    return false;
}

// Free the pending-request list
void clearPendingRequests() {
    FriendRequest* cur = gPendingRequestsHead;
    while (cur) {
        FriendRequest* nxt = cur->next;
        delete cur;
        cur = nxt;
    }
    gPendingRequestsHead = nullptr;
}

// Save all pending friend requests to friend_requests.txt
void saveFriendRequestsToFile() {
    ofstream fout("friend_requests.txt");
    if (!fout) return;
    FriendRequest* cur = gPendingRequestsHead;
    while (cur) {
        const string& fromName = allPlayers[cur->fromIndex].username;
        const string& toName   = allPlayers[cur->toIndex].username;
        fout << fromName << " " << toName << "\n";
        cur = cur->next;
    }
}

// Load all pending friend requests from friend_requests.txt
void loadFriendRequestsFromFile() {
    clearPendingRequests();
    ifstream fin("friend_requests.txt");
    if (!fin) return;
    string fromName, toName;
    while (fin >> fromName >> toName) {
        int fromIdx = findPlayerIndexByName(fromName);
        int toIdx   = findPlayerIndexByName(toName);
        if (fromIdx == -1 || toIdx == -1) continue;
        FriendRequest* req = new FriendRequest(fromIdx, toIdx);
        req->next = gPendingRequestsHead;
        gPendingRequestsHead = req;
    }
}

// Send a friend request from 'fromUser' to 'toUser'
bool sendFriendRequest(const string& fromUser, const string& toUser) {
    int fromIdx = findPlayerIndexByName(fromUser);
    int toIdx   = findPlayerIndexByName(toUser);
    if (fromIdx == -1 || toIdx == -1) return false;   
    if (fromIdx == toIdx)             return false;   
    if (alreadyFriends(fromIdx, toIdx)) return false; 
    if (alreadyPending(fromIdx, toIdx)) return false; 

    FriendRequest* req = new FriendRequest(fromIdx, toIdx);
    req->next = gPendingRequestsHead;
    gPendingRequestsHead = req;
    saveFriendRequestsToFile();
    return true;
}

// Accept a pending friend request to 'toUser' from 'fromUser'
bool acceptFriendRequest(const string& toUser, const string& fromUser) {
    int fromIdx = findPlayerIndexByName(fromUser);
    int toIdx   = findPlayerIndexByName(toUser);
    if (fromIdx == -1 || toIdx == -1) return false;

    FriendRequest* cur = gPendingRequestsHead;
    FriendRequest* prev = nullptr;
    while (cur) {
        if (cur->fromIndex == fromIdx && cur->toIndex == toIdx) {
            
            if (prev) prev->next = cur->next;
            else      gPendingRequestsHead = cur->next;
            delete cur;

            
            addFriendEdge(fromIdx, toIdx);
            addFriendEdge(toIdx, fromIdx);
            saveFriendsToFile();
            saveFriendRequestsToFile();
            return true;
        }
        prev = cur;
        cur = cur->next;
    }
    return false;
}

// Reject (delete) friend request without adding to friends
bool rejectFriendRequest(const string& toUser, const string& fromUser) {
    int fromIdx = findPlayerIndexByName(fromUser);
    int toIdx   = findPlayerIndexByName(toUser);
    if (fromIdx == -1 || toIdx == -1) return false;

    FriendRequest* cur = gPendingRequestsHead;
    FriendRequest* prev = nullptr;
    while (cur) {
        if (cur->fromIndex == fromIdx && cur->toIndex == toIdx) {
            if (prev) prev->next = cur->next;
            else      gPendingRequestsHead = cur->next;
            delete cur;
            saveFriendRequestsToFile();
            return true;
        }
        prev = cur;
        cur = cur->next;
    }
    return false;
}

// Return a one-line summary of pending requests for 'uname'
string getPendingRequestsLineForUser(const string& uname) {
    int idx = findPlayerIndexByName(uname);
    if (idx < 0) return "Pending requests: (none)";

    string result = "Pending requests from: ";
    bool any = false;
    int shown = 0;

    FriendRequest* cur = gPendingRequestsHead;
    while (cur && shown < 5) {
        if (cur->toIndex == idx) {
            if (any) result += ", ";
            result += allPlayers[cur->fromIndex].username;
            any = true;
            shown++;
        }
        cur = cur->next;
    }
    if (!any) return "Pending requests: (none)";
    return result;
}
// Return username of the FIRST pending requester for uname
string getFirstPendingRequesterForUser(const string& uname) {
    int idx = findPlayerIndexByName(uname);
    if (idx < 0) return "";

    FriendRequest* cur = gPendingRequestsHead;
    while (cur) {
        if (cur->toIndex == idx) {
            return allPlayers[cur->fromIndex].username;
        }
        cur = cur->next;
    }
    return "";
}


// Initialize everything (call this before using friend system)
void initFriendSystemAll() {
    initFriendSystem();
    loadFriendsFromFile();
    loadFriendRequestsFromFile();   
}


// the friend system (allPlayers[] + hashTable).
void addPlayerToFriendSystem(const Player& p) {
    int idx = findPlayerIndexByName(p.username);
    if (idx != -1) {
    
        allPlayers[idx] = p;
        return;
    }
    if (allPlayerCount >= MAX_ALL_PLAYERS) return;

    int newIdx = allPlayerCount++;
    allPlayers[newIdx] = p;

    unsigned long h = hashUsernameString(p.username);
    int pos = (int)(h % HASH_SIZE);
    while (hashTable[pos] != -1) {
        pos++;
        if (pos == HASH_SIZE) pos = 0;
    }
    hashTable[pos] = newIdx;
}


// Return "Friends: a, b, c" (shows up to 5 friends)
string getFriendsLineForUser(const string& uname) {
    if (allPlayerCount == 0) return "Friends: (none)";
    int idx = findPlayerIndexByName(uname);
    if (idx < 0) return "Friends: (none)";

    string result = "Friends: ";
    FriendNode* cur = friendAdj[idx];
    if (!cur) {
        result += "(none)";
        return result;
    }
    int count = 0;
    while (cur && count < 5) {
        if (count > 0) result += ", ";
        int j = cur->friendIndex;
        result += allPlayers[j].username;
        cur = cur->next;
        count++;
    }
    return result;
}

struct MatchNode {
    int playerIndex; 
};

struct MatchPriorityQueue {
    MatchNode heap[MAX_ALL_PLAYERS + 1]; 
    int size;
    MatchPriorityQueue() : size(0) {}

    bool empty() const { return size == 0; }

    void push(int idx) {
        if (size >= MAX_ALL_PLAYERS) return;
        size++;
        heap[size].playerIndex = idx;
        int i = size;
        while (i > 1) {
            int p = i / 2;
            int ip = heap[p].playerIndex;
            int ii = heap[i].playerIndex;
            if (allPlayers[ip].totalPoints >= allPlayers[ii].totalPoints) break;
            MatchNode tmp = heap[p];
            heap[p] = heap[i];
            heap[i] = tmp;
            i = p;
        }
    }

    int pop() {
        if (size == 0) return -1;
        int res = heap[1].playerIndex;
        heap[1] = heap[size];
        size--;
        int i = 1;
        while (true) {
            int l = 2 * i, r = 2 * i + 1, largest = i;
            if (l <= size &&
                allPlayers[ heap[l].playerIndex ].totalPoints >
                allPlayers[ heap[largest].playerIndex ].totalPoints)
                largest = l;
            if (r <= size &&
                allPlayers[ heap[r].playerIndex ].totalPoints >
                allPlayers[ heap[largest].playerIndex ].totalPoints)
                largest = r;
            if (largest == i) break;
            MatchNode tmp = heap[i];
            heap[i] = heap[largest];
            heap[largest] = tmp;
            i = largest;
        }
        return res;
    }
};

MatchPriorityQueue gMatchPQ;



struct GameRoomQueue {
    int arr[MAX_ALL_PLAYERS];
    int front;
    int rear;
    int count;

    GameRoomQueue() : front(0), rear(0), count(0) {}

    bool empty() const { return count == 0; }

    bool enqueue(int idx) {
        if (count >= MAX_ALL_PLAYERS) return false;
        arr[rear] = idx;
        rear = (rear + 1) % MAX_ALL_PLAYERS;
        count++;
        return true;
    }

    int dequeue() {
        if (count == 0) return -1;
        int v = arr[front];
        front = (front + 1) % MAX_ALL_PLAYERS;
        count--;
        return v;
    }
};

GameRoomQueue gGameRoomQueue;

// Add one player (by username) into the waiting queue
void addPlayerToMatchmakingQueue(const string& uname) {
    int idx = findPlayerIndexByName(uname);
    if (idx == -1) return;  // player not in friend system
    gGameRoomQueue.enqueue(idx);
}


void processMatchmakingQueue() {
    
    while (!gGameRoomQueue.empty()) {
        int idx = gGameRoomQueue.dequeue();
        if (idx != -1) {
            gMatchPQ.push(idx);
        }
    }

    while (gMatchPQ.size >= 2) {
        int pA = gMatchPQ.pop();
        int pB = gMatchPQ.pop();
        if (pA == -1 || pB == -1) break;

        cout << "[MATCHMAKING] Room created: "
             << allPlayers[pA].username << " vs "
             << allPlayers[pB].username << "\n";
    }
}



struct ThemeNode {
    int id;
    string name;
    string desc;
    unsigned char r, g, b, a;
    int height;
    ThemeNode* left;
    ThemeNode* right;

        ThemeNode(int tid, const string& nm, const string& dsc,
                            unsigned char rr, unsigned char gg,
              unsigned char bb, unsigned char aa)
                : id(tid), name(nm), desc(dsc), r(rr), g(gg), b(bb), a(aa),
          height(1), left(nullptr), right(nullptr) {}
};

ThemeNode* themeRoot = nullptr;
int currentThemeId = 1;

int nodeHeight(ThemeNode* n) {
    return n ? n->height : 0;
}

int getBalance(ThemeNode* n) {
    if (!n) return 0;
    return nodeHeight(n->left) - nodeHeight(n->right);
}

ThemeNode* rotateRight(ThemeNode* y) {
    ThemeNode* x = y->left;
    ThemeNode* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = 1 + (nodeHeight(y->left) > nodeHeight(y->right)
                     ? nodeHeight(y->left) : nodeHeight(y->right));
    x->height = 1 + (nodeHeight(x->left) > nodeHeight(x->right)
                     ? nodeHeight(x->left) : nodeHeight(x->right));
    return x;
}

ThemeNode* rotateLeft(ThemeNode* x) {
    ThemeNode* y = x->right;
    ThemeNode* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = 1 + (nodeHeight(x->left) > nodeHeight(x->right)
                     ? nodeHeight(x->left) : nodeHeight(x->right));
    y->height = 1 + (nodeHeight(y->left) > nodeHeight(y->right)
                     ? nodeHeight(y->left) : nodeHeight(y->right));
    return y;
}

ThemeNode* insertTheme(ThemeNode* node, int id, const string& name, const string& desc,
                       unsigned char r, unsigned char g,
                       unsigned char b, unsigned char a) {
    if (!node) return new ThemeNode(id, name, desc, r, g, b, a);

    if (id < node->id)
        node->left = insertTheme(node->left, id, name, desc, r, g, b, a);
    else if (id > node->id)
        node->right = insertTheme(node->right, id, name, desc, r, g, b, a);
    else
        return node;

    node->height = 1 + (nodeHeight(node->left) > nodeHeight(node->right)
                        ? nodeHeight(node->left) : nodeHeight(node->right));

    int balance = getBalance(node);

    // Left Left
    if (balance > 1 && id < node->left->id)
        return rotateRight(node);

    // Right Right
    if (balance < -1 && id > node->right->id)
        return rotateLeft(node);

    // Left Right
    if (balance > 1 && id > node->left->id) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }

    // Right Left
    if (balance < -1 && id < node->right->id) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

ThemeNode* findThemeById(ThemeNode* node, int id) {
    if (!node) return nullptr;
    if (id == node->id) return node;
    if (id < node->id) return findThemeById(node->left, id);
    return findThemeById(node->right, id);
}

void initThemes() {
    themeRoot = nullptr;
    
    themeRoot = insertTheme(themeRoot, 1, "Blue", "",  0,   0,   0, 180);
    themeRoot = insertTheme(themeRoot, 2, "Purple", "", 10,  40,  80, 200);
    themeRoot = insertTheme(themeRoot, 3, "Red", "", 10,  80,  10, 200);
    themeRoot = insertTheme(themeRoot, 4, "Green", "", 80,  30,   0, 200);
    themeRoot = insertTheme(themeRoot, 5, "Yellow", "", 50,  20,  70, 200);
    themeRoot = insertTheme(themeRoot, 6, "Ferozi", "", 30,  30,  35, 200);
    themeRoot = insertTheme(themeRoot, 7, "Orange", "", 90,   0,  60, 200);
    themeRoot = insertTheme(themeRoot, 8, "Sea Blue", "", 20, 100,  80, 200);
    currentThemeId = 1;
}

Color getCurrentHudColor() {
    ThemeNode* t = findThemeById(themeRoot, currentThemeId);
    if (!t) return Color(0, 0, 0, 180);
    return Color(t->r, t->g, t->b, t->a);
}

int getTileThemeOffsetX() {

    int variants = 8;
    int tileW = ts; 
    int idx = currentThemeId;
    if (idx < 1) idx = 1;
    int v = (idx - 1) % variants; 
    return v * tileW;
}

// In-order traversal to array
void fillThemeArrayInOrder(ThemeNode* node, ThemeNode** arr, int& idx, int maxCount) {
    if (!node || idx >= maxCount) return;
    fillThemeArrayInOrder(node->left, arr, idx, maxCount);
    if (idx < maxCount) arr[idx++] = node;
    fillThemeArrayInOrder(node->right, arr, idx, maxCount);
}

// Search by name
ThemeNode* findThemeByName(ThemeNode* node, const string& name) {
    if (!node) return nullptr;
    if (node->name == name) return node;
    ThemeNode* l = findThemeByName(node->left, name);
    if (l) return l;
    return findThemeByName(node->right, name);
}


    struct MinHeapTop10 {
        LeaderboardEntry heap[11]; 
        int size;
        MinHeapTop10() { size = 0; }

        void heapifyUp(int i) {
            while (i > 1) {
                int p = i / 2;
                if (heap[p].totalPoints <= heap[i].totalPoints) break;
                swap(heap[p], heap[i]);
                i = p;
            }
        }

        void heapifyDown(int i) {
            while (true) {
                int l = 2*i, r = 2*i+1, smallest = i;
                if (l <= size && heap[l].totalPoints < heap[smallest].totalPoints) smallest = l;
                if (r <= size && heap[r].totalPoints < heap[smallest].totalPoints) smallest = r;
                if (smallest == i) break;
                swap(heap[i], heap[smallest]);
                i = smallest;
            }
        }

        int find(const string& uname) {
            for (int i = 1; i <= size; i++)
                if (heap[i].username == uname) return i;
            return -1;
        }

        void insertOrUpdate(const string& uname, int pts) {
            int pos = find(uname);
            if (pos != -1) {
                heap[pos].totalPoints = pts;
                heapifyUp(pos);
                heapifyDown(pos);
                return;
            }

            if (size < 10) {
                size++;
                heap[size].username = uname;
                heap[size].totalPoints = pts;
                heapifyUp(size);
            } else {
                if (pts > heap[1].totalPoints) {
                    heap[1].username = uname;
                    heap[1].totalPoints = pts;
                    heapifyDown(1);
                }
            }
        }
    };
    
    
MinHeapTop10 gLeaderboard;


void rebuildLeaderboardFromPlayersFile() {
    gLeaderboard.size = 0;

    ifstream fin("players.txt");
    if (!fin) return;

    string line;
    while (std::getline(fin, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        Player p;
        
        if (!(iss >> p.username >> p.password >> p.totalPoints >> p.bestScore >> p.wins >> p.losses >> p.powerUps))
            continue;
            
        if (!(iss >> p.themeId)) p.themeId = 1;
        gLeaderboard.insertOrUpdate(p.username, p.totalPoints);
    }
}


    // -------- file helpers (load / save player) --------
    bool loadPlayer(const string& uname, Player& out) {
        ifstream fin("players.txt");
        if (!fin) return false;
        string line;
        while (std::getline(fin, line)) {
            if (line.empty()) continue;
            std::istringstream iss(line);
            Player p;
            if (!(iss >> p.username >> p.password >> p.totalPoints >> p.bestScore >> p.wins >> p.losses >> p.powerUps))
                continue;
            if (!(iss >> p.themeId)) p.themeId = 1;
            if (p.username == uname) { out = p; return true; }
        }
        return false;
    }

    void saveOrUpdatePlayer(const Player& pl) {
        Player list[200];
        int count = 0;

        ifstream fin("players.txt");
        if (fin) {
            string line;
            while (std::getline(fin, line)) {
                if (line.empty()) continue;
                std::istringstream iss(line);
                Player tmp;
                if (!(iss >> tmp.username >> tmp.password >> tmp.totalPoints >> tmp.bestScore >> tmp.wins >> tmp.losses >> tmp.powerUps))
                    continue;
                if (!(iss >> tmp.themeId)) tmp.themeId = 1;
                list[count++] = tmp;
                if (count >= 200) break;
            }
            fin.close();
        }

        bool found = false;
        for (int i = 0; i < count; i++) {
            if (list[i].username == pl.username) {
                list[i] = pl;
                found = true;
                break;
            }
        }
        if (!found) {
            list[count] = pl;
            count++;
        }

        ofstream fout("players.txt");
        for (int i = 0; i < count; i++) {
            fout << list[i].username << " " << list[i].password << " "
                 << list[i].totalPoints << " " << list[i].bestScore << " "
                 << list[i].wins << " " << list[i].losses << " "
                 << list[i].powerUps << " " << list[i].themeId << "\n";
        }
    }


    void tryChangeDirection(int& dx, int& dy, int ndx, int ndy) {
        if (dx == -ndx && dy == -ndy && (dx != 0 || dy != 0)) return;
        dx = ndx; dy = ndy;
    }

    enum class GameState {
        AUTH_MENU,     
        LOGIN,
        REGISTER,
        MENU,
        LOAD_SAVE,     
        INVENTORY,     
        LEVEL_SELECT,
        LEADERBOARD,
        PROFILE,
        INSTRUCTIONS,  
        PLAYING,
        MULTI_PLAYING,
        GAME_OVER,
        LOGIN_P2       
    };

    struct LevelConfig {
        string name;
        int enemies;
        float enemyDelay;
    };


    const float ENEMY_SLOW = 0.14f; 
    const float ENEMY_FAST = 0.07f; 

   
    const int LEVEL_COUNT = 7;
    LevelConfig levels[LEVEL_COUNT] = {
        {"Noob",     2, ENEMY_SLOW}, 
        {"Beginner", 2, ENEMY_FAST}, 
        {"Average",  4, ENEMY_SLOW}, 
        {"Expert",   4, ENEMY_FAST}, 
        {"Pro",      6, ENEMY_SLOW}, 
        {"God",      6, ENEMY_FAST}, 
        {"Hacker",   0, ENEMY_FAST}  
    };


    // helper to mask password for UI
    string maskPassword(const string& pwd) {
        return string(pwd.size(), '*');
    }

    int runGameSession(RenderWindow& window,
                    Texture& t1, Texture& t2, Texture& t3,
                    Player& currentPlayer,
                    const LevelConfig& levelCfg,
                    bool& outNewHighScore)
    {
        Sprite sTile(t1), sGameover(t2), sEnemy(t3);
        sGameover.setPosition({100.f, 100.f});
        sEnemy.setOrigin({20.f, 20.f});

        int enemyCount = levelCfg.enemies;
        Enemy enemies[10];

        // reset grid
        for (int i = 0; i < M; i++)
            for (int j = 0; j < N; j++)
                grid[i][j] = 0;
        for (int i = 0; i < M; i++)
            for (int j = 0; j < N; j++)
                if (i == 0 || j == 0 || i == M - 1 || j == N - 1)
                    grid[i][j] = 1;

        
        int x = 0, y = 0, dx = 1, dy = 0;

        float timerPlayer = 0.f;
        float timerEnemy  = 0.f;  
        const float playerDelay = 0.07f;           
        const float enemyDelay  = levelCfg.enemyDelay; 
        Clock clock;

        bool alive = true;

        
        int score = 0;
        int rewardCounter = 0;
        int tilesThisMove = 0;
        int powerUps = currentPlayer.powerUps;
        bool powerActive = false;
        float powerTimer = 0.f;

        
        string saveNotification = "";
        float saveNotificationTimer = 0.f;

        Font font;
        font.openFromFile("arial/ARIAL.TTF");

        Text nameText(font, "", 18);
        Text scoreText(font, "", 18);
        Text powerText(font, "", 18);
        Text hintText(font, "Press S to save game | SPACE for power-up | ESC to quit", 14);


        while (window.isOpen()) {
            float time = clock.getElapsedTime().asSeconds();
            clock.restart();
            timerPlayer += time;
            timerEnemy  += time;

            
            if (powerActive) {
                powerTimer += time;
                if (powerTimer >= 3.f) {
                    powerActive = false;
                    powerTimer = 0.f;
                }
            }

           
            if (!saveNotification.empty()) {
                saveNotificationTimer += time;
                if (saveNotificationTimer >= 2.5f) {
                    saveNotification.clear();
                    saveNotificationTimer = 0.f;
                }
            }

           
            while (const auto event = window.pollEvent()) {
                const Event& e = *event;
                if (e.is<Event::Closed>()) {
                    window.close();
                    return score;
                }
                if (const auto* key = e.getIf<Event::KeyPressed>()) {
                    if (key->scancode == Keyboard::Scancode::Escape) {
                        alive = false;    
                    }
                    if (key->scancode == Keyboard::Scancode::Space) {
                        if (powerUps > 0 && !powerActive) {
                            powerUps--;
                            powerActive = true;
                        }
                    }
                    
                    if (key->scancode == Keyboard::Scancode::S) {
                        GameSave gs;
                        gs.username = currentPlayer.username;
                        gs.score = score;
                        gs.timestamp = static_cast<long>(std::time(nullptr));
                        gs.tilesHead = buildTileListFromGrid();
                        gs.saveId = getNextSaveId();
                        appendGameSaveToFile(gs);
                        
                        saveNotification = "Game saved! ID: " + to_string(gs.saveId);
                        saveNotificationTimer = 0.f;
                    }
                }
            }

            if (!alive) break;

            
            if (Keyboard::isKeyPressed(Keyboard::Key::Left))
                tryChangeDirection(dx, dy, -1, 0);
            if (Keyboard::isKeyPressed(Keyboard::Key::Right))
                tryChangeDirection(dx, dy, 1, 0);
            if (Keyboard::isKeyPressed(Keyboard::Key::Up))
                tryChangeDirection(dx, dy, 0, -1);
            if (Keyboard::isKeyPressed(Keyboard::Key::Down))
                tryChangeDirection(dx, dy, 0, 1);

            
            if (timerPlayer > playerDelay) {
                timerPlayer = 0.f;

                x += dx; y += dy;
                if (x < 0) x = 0;
                if (x > N - 1) x = N - 1;
                if (y < 0) y = 0;
                if (y > M - 1) y = M - 1;

                if (grid[y][x] == 2) {   
                    alive = false;
                }

                if (grid[y][x] == 0) {
                    grid[y][x] = 2;
                    tilesThisMove++;
                    score += 1;
                }

                if (grid[y][x] == 1) {
                    
                    if (tilesThisMove > 0) {
                        int threshold = 10;
                        if (rewardCounter >= 3 && rewardCounter < 5) threshold = 5;
                        if (tilesThisMove > threshold) {
                            int multiplier = (rewardCounter >= 5) ? 4 : 2;
                            score += tilesThisMove * (multiplier - 1);
                            rewardCounter++;
                        }
                    }

                    
                    if (score == 50 || score == 70 || score == 100 ||
                        score == 130 || (score > 130 && (score - 130) % 30 == 0)) {
                        powerUps++;
                    }

                    dx = dy = 0;

                    
                    for (int i = 0; i < enemyCount; i++)
                        drop(enemies[i].y / ts, enemies[i].x / ts);

                   
                    for (int i = 0; i < M; i++)
                        for (int j = 0; j < N; j++) {
                            if (grid[i][j] == -1) {
                                grid[i][j] = 0;        
                            } else if (grid[i][j] == 0 || grid[i][j] == 2) {
                                grid[i][j] = 1;        
                            } 
                        }

                    tilesThisMove = 0;
                }
            }

            
            if (!powerActive && timerEnemy > enemyDelay) {
                timerEnemy = 0.f;
                for (int i = 0; i < enemyCount; i++)
                    enemies[i].move();
            }

            
            for (int i = 0; i < enemyCount; i++) {
                if (grid[enemies[i].y / ts][enemies[i].x / ts] == 2) {
                    alive = false;
                }
            }

            if (!alive) break;

            
            window.clear();

            
            RectangleShape hudBar;
            hudBar.setSize({float(N * ts), float(HUD_HEIGHT)});
            hudBar.setPosition({0.f, 0.f});
            hudBar.setFillColor(getCurrentHudColor());
            window.draw(hudBar);

            for (int i = 0; i < M; i++)
                for (int j = 0; j < N; j++) {
                    if (grid[i][j] == 0) continue;
                    int varX = getTileThemeOffsetX();
                    if (grid[i][j] == 1) sTile.setTextureRect(IntRect({varX, 0}, {ts, ts}));
                    if (grid[i][j] == 2) sTile.setTextureRect(IntRect({54, 0}, {ts, ts}));
                    sTile.setPosition({float(j * ts), float(i * ts + HUD_HEIGHT)});

                    window.draw(sTile);
                }

            
            sTile.setTextureRect(IntRect({36, 0}, {ts, ts}));
            sTile.setPosition({float(x * ts), float(y * ts + HUD_HEIGHT)});
            window.draw(sTile);


            
            sEnemy.rotate(degrees(8));
            for (int i = 0; i < enemyCount; i++) {
                sEnemy.setPosition({float(enemies[i].x), float(enemies[i].y + HUD_HEIGHT)});
                window.draw(sEnemy);
            }


            float winWidth = N * ts;

            
            nameText.setString("User: " + currentPlayer.username + " | Mode: " + levelCfg.name);
            nameText.setPosition({10.f, 10.f});

            
            scoreText.setString("Score: " + to_string(score));
            auto sb = scoreText.getLocalBounds();       // FloatRect in SFML 3
            float scoreWidth = sb.size.x;               // <-- use size.x instead of width
            scoreText.setPosition({(winWidth - scoreWidth) / 2.f, 10.f});

            
            powerText.setString("PowerUps: " + to_string(powerUps));
            auto pb = powerText.getLocalBounds();
            float powerWidth = pb.size.x;               // <-- use size.x instead of width
            powerText.setPosition({winWidth - powerWidth - 10.f, 10.f});

            
            window.draw(nameText);
            window.draw(scoreText);
            window.draw(powerText);

            
            hintText.setPosition({10.f, 35.f});
            hintText.setFillColor(Color(180, 180, 180));
            window.draw(hintText);

            if (!saveNotification.empty()) {
                Text saveMsg(font, saveNotification, 22);
                saveMsg.setFillColor(Color::White);
                auto smb = saveMsg.getLocalBounds();
                
                float boxPadding = 15.f;
                float boxWidth = smb.size.x + boxPadding * 2;
                float boxHeight = smb.size.y + boxPadding * 2;
                float boxX = (winWidth - boxWidth) / 2.f;
                float boxY = M * ts + HUD_HEIGHT - 50.f;
                
                RectangleShape notifBox(Vector2f(boxWidth, boxHeight));
                notifBox.setPosition(Vector2f(boxX, boxY));
                notifBox.setFillColor(Color(0, 150, 0, 220)); 
                notifBox.setOutlineColor(Color(0, 255, 0));
                notifBox.setOutlineThickness(2.f);
                window.draw(notifBox);
                
                saveMsg.setPosition({boxX + boxPadding, boxY + boxPadding});
                window.draw(saveMsg);
            }



            window.display();
        }

            bool newHigh = (score > currentPlayer.bestScore);
    if (newHigh) currentPlayer.bestScore = score;
    currentPlayer.totalPoints += score;
    if (score > 0) currentPlayer.wins++;
    else currentPlayer.losses++;
    currentPlayer.powerUps = powerUps;
    saveOrUpdatePlayer(currentPlayer);
    outNewHighScore = newHigh;

    gLeaderboard.insertOrUpdate(currentPlayer.username, currentPlayer.totalPoints);


    GameSave gs;
    gs.username  = currentPlayer.username;
    gs.score     = score;
    gs.timestamp = static_cast<long>(time(nullptr));
    gs.tilesHead = buildTileListFromGrid();
    gs.saveId    = getNextSaveId();
    appendGameSaveToFile(gs);
    freeTileList(gs.tilesHead);

    return score;
}


    struct MultiResult {
        int score1;
        int score2;
        int winner; 
    };

   

    MultiResult runMultiplayerSession(RenderWindow& window,
                                  Texture& t1, Texture& t2, Texture& t3,
                                  Player& p1,
                                  Player& p2,
                                  const LevelConfig& levelCfg)
{
    Sprite sTile(t1), sGameover(t2), sEnemy(t3);
    sEnemy.setOrigin({20.f, 20.f});

    int enemyCount = levelCfg.enemies;
    Enemy enemies[10];

    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            grid[i][j] = 0;

    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            if (i == 0 || j == 0 || i == M - 1 || j == N - 1)
                grid[i][j] = 1;

    int x1 = 2,      y1 = 2;
    int x2 = N - 3,  y2 = M - 3;

    grid[y1][x1] = 1;
    grid[y2][x2] = 1;

    int dx1 = 0, dy1 = 0;
    int dx2 = 0, dy2 = 0;

    bool alive1 = true, alive2 = true;

    bool drawing1 = false, drawing2 = false;

    int score1 = 0, score2 = 0;
    int tilesMove1 = 0, tilesMove2 = 0;
    int rewardCnt1 = 0, rewardCnt2 = 0;

    int power1 = p1.powerUps;
    int power2 = p2.powerUps;

    bool freezeAll = false;
    int  freezeOwner = 0;           
    float freezeTimer = 0.f;

    float timerPlayer = 0.f;
    float timerEnemy  = 0.f;
    const float playerDelay = 0.07f;
    const float enemyDelay  = levelCfg.enemyDelay;

    Clock clock;
    float gameTime = 0.f;

    Font font;
    font.openFromFile("arial/ARIAL.TTF");

    Text p1Text(font, "", 16);
    Text p2Text(font, "", 16);
    Text timeText(font, "", 16);

    while (window.isOpen()) {
        float dt = clock.getElapsedTime().asSeconds();
        clock.restart();
        timerPlayer += dt;
        timerEnemy  += dt;
        gameTime    += dt;

        
        if (freezeAll) {
            freezeTimer += dt;
            if (freezeTimer >= 3.f) {        
                freezeAll = false;
                freezeTimer = 0.f;
                freezeOwner = 0;
            }
        }

        while (const auto event = window.pollEvent()) {
            const Event& e = *event;
            if (e.is<Event::Closed>()) {
                window.close();
                return {score1, score2, 0};
            }
            if (const auto* key = e.getIf<Event::KeyPressed>()) {
                if (key->scancode == Keyboard::Scancode::Escape) {
                    
                    alive1 = false;
                    alive2 = false;
                }

                if (key->scancode == Keyboard::Scancode::RControl) { 
                    if (power1 > 0 && !freezeAll) {
                        power1--;
                        freezeAll  = true;
                        freezeOwner = 1;  
                    }
                }
                if (key->scancode == Keyboard::Scancode::F) {   
                    if (power2 > 0 && !freezeAll) {
                        power2--;
                        freezeAll  = true;
                        freezeOwner = 2;  
                    }
                }
            }
        }

        if (!alive1 && !alive2) break;


        if (!freezeAll || freezeOwner == 1) {
            if (Keyboard::isKeyPressed(Keyboard::Key::Left))
                tryChangeDirection(dx1, dy1, -1, 0);
            if (Keyboard::isKeyPressed(Keyboard::Key::Right))
                tryChangeDirection(dx1, dy1, 1, 0);
            if (Keyboard::isKeyPressed(Keyboard::Key::Up))
                tryChangeDirection(dx1, dy1, 0, -1);
            if (Keyboard::isKeyPressed(Keyboard::Key::Down))
                tryChangeDirection(dx1, dy1, 0, 1);
        }


        if (!freezeAll || freezeOwner == 2) {
            if (Keyboard::isKeyPressed(Keyboard::Key::A))
                tryChangeDirection(dx2, dy2, -1, 0);  
            if (Keyboard::isKeyPressed(Keyboard::Key::G))
                tryChangeDirection(dx2, dy2, 1, 0);   
            if (Keyboard::isKeyPressed(Keyboard::Key::Q))
                tryChangeDirection(dx2, dy2, 0, -1);  
            if (Keyboard::isKeyPressed(Keyboard::Key::X))
                tryChangeDirection(dx2, dy2, 0, 1);   
        }

        if (timerPlayer > playerDelay) {
            timerPlayer = 0.f;

            if (alive1 && (!freezeAll || freezeOwner == 1)) {
                x1 += dx1;
                y1 += dy1;
                if (x1 < 0) x1 = 0;
                if (x1 > N - 1) x1 = N - 1;
                if (y1 < 0) y1 = 0;
                if (y1 > M - 1) y1 = M - 1;
            }
            if (alive2 && (!freezeAll || freezeOwner == 2)) {
                x2 += dx2;
                y2 += dy2;
                if (x2 < 0) x2 = 0;
                if (x2 > N - 1) x2 = N - 1;
                if (y2 < 0) y2 = 0;
                if (y2 > M - 1) y2 = M - 1;
            }


            auto handlePlayer = [&](int& x, int& y, bool& alive, bool& drawing,
                                    int myTrail, int otherTrail,
                                    int& tilesThisMove,
                                    int& score, int& rewardCnt,
                                    int& power,
                                    int& dx, int& dy) -> bool {
                if (!alive) return false;

                int cell = grid[y][x];

                if (cell == myTrail || cell == otherTrail) {
                    alive = false;
                    return false;
                }

                if (cell == 0) {
                    grid[y][x] = myTrail;
                    drawing = true;
                    tilesThisMove++;
                    score += 1;
                    return false;
                }

                bool closed = false;

                
                if (cell == 1) {
                
                    if (drawing && tilesThisMove > 0) {
                        int threshold = 10;
                        if (rewardCnt >= 3 && rewardCnt < 5) threshold = 5;
                        if (tilesThisMove > threshold) {
                            int mult = (rewardCnt >= 5) ? 4 : 2;
                            score += tilesThisMove * (mult - 1);
                            rewardCnt++;
                        }

                        if (score == 50 || score == 70 || score == 100 ||
                            score == 130 || (score > 130 && (score - 130) % 30 == 0)) {
                            power++;
                        }

                        tilesThisMove = 0;
                        drawing = false;
                        closed = true;


                        dx = 0;
                        dy = 0;
                    }
                }

                return closed;
            };

            bool closed1 = handlePlayer(x1, y1, alive1, drawing1,
                                        2, 3, tilesMove1, score1, rewardCnt1, power1,
                                        dx1, dy1);
            bool closed2 = handlePlayer(x2, y2, alive2, drawing2,
                                        3, 2, tilesMove2, score2, rewardCnt2, power2,
                                        dx2, dy2);

            if (alive1 && alive2 && x1 == x2 && y1 == y2) {
                
                if (drawing1 && drawing2) {
                    alive1 = false;
                    alive2 = false;
                }
                
                else if (drawing1 && !drawing2) {
                    alive1 = false;
                }
                else if (!drawing1 && drawing2) {
                    alive2 = false;
                }
            }

            if (closed1 || closed2) {
            
                for (int i = 0; i < enemyCount; i++)
                    drop(enemies[i].y / ts, enemies[i].x / ts);


                for (int i = 0; i < M; i++)
                    for (int j = 0; j < N; j++) {
                        if (grid[i][j] == -1) {
                            grid[i][j] = 0;
                        } else if (grid[i][j] == 0 ||
                                   grid[i][j] == 2 ||
                                   grid[i][j] == 3) {
                            grid[i][j] = 1;
                        }
                    }

                drawing1 = drawing2 = false;
                tilesMove1 = tilesMove2 = 0;
            }
        }

        if (!freezeAll && timerEnemy > enemyDelay) {
            timerEnemy = 0.f;

            for (int i = 0; i < enemyCount; i++)
                enemies[i].move();

            for (int i = 0; i < enemyCount; i++) {
                int cy = enemies[i].y / ts;
                int cx = enemies[i].x / ts;
                if (cy < 0 || cy >= M || cx < 0 || cx >= N) continue;
                if (grid[cy][cx] == 2) alive1 = false;
                if (grid[cy][cx] == 3) alive2 = false;
            }
        }

        if (!alive1 && !alive2) break;

        window.clear();

        RectangleShape hudBar;
        hudBar.setSize({float(N * ts), float(HUD_HEIGHT)});
        hudBar.setPosition({0.f, 0.f});
        hudBar.setFillColor(getCurrentHudColor());
        window.draw(hudBar);

        for (int i = 0; i < M; i++)
            for (int j = 0; j < N; j++) {
                if (grid[i][j] == 0) continue;
                int varX = getTileThemeOffsetX();
                if (grid[i][j] == 1) sTile.setTextureRect(IntRect({varX, 0}, {ts, ts}));
                if (grid[i][j] == 2) sTile.setTextureRect(IntRect({54, 0}, {ts, ts})); 
                if (grid[i][j] == 3) sTile.setTextureRect(IntRect({36, 0}, {ts, ts})); 

                sTile.setPosition({float(j * ts), float(i * ts + HUD_HEIGHT)});
                window.draw(sTile);
            }

        sTile.setTextureRect(IntRect({36, 0}, {ts, ts}));
        sTile.setPosition({float(x1 * ts), float(y1 * ts + HUD_HEIGHT)});
        window.draw(sTile);
        
        sTile.setTextureRect(IntRect({54, 0}, {ts, ts}));
        sTile.setPosition({float(x2 * ts), float(y2 * ts + HUD_HEIGHT)});
        window.draw(sTile);

        
        sEnemy.rotate(degrees(8));
        for (int i = 0; i < enemyCount; i++) {
            sEnemy.setPosition({float(enemies[i].x), float(enemies[i].y + HUD_HEIGHT)});
            window.draw(sEnemy);
        }

        
        float winWidth = N * ts;

        
        p1Text.setString(
            "P1: " + p1.username +
            " | Score: " + to_string(score1) +
            " | PowerUps: " + to_string(power1)
        );
        p1Text.setPosition({10.f, 10.f});
        window.draw(p1Text);

        
        timeText.setString("Time: " + to_string((int)gameTime) + "s");
        auto tb = timeText.getLocalBounds();
        float timeW = tb.size.x;
        timeText.setPosition({(winWidth - timeW) / 2.f, 10.f});
        window.draw(timeText);

        
        p2Text.setString(
            "P2: " + p2.username +
            " | Score: " + to_string(score2) +
            " | PowerUps: " + to_string(power2)
        );
        auto p2b = p2Text.getLocalBounds();
        float p2W = p2b.size.x;
        p2Text.setPosition({winWidth - p2W - 10.f, 10.f});
        window.draw(p2Text);

        window.display();
    }

    int winner = 0;
    if (score1 > score2) {
        p1.wins++;
        p2.losses++;
        winner = 1;
    } else if (score2 > score1) {
        p2.wins++;
        p1.losses++;
        winner = 2;
    }

    p1.totalPoints += score1;
p2.totalPoints += score2;
if (score1 > p1.bestScore) p1.bestScore = score1;
if (score2 > p2.bestScore) p2.bestScore = score2;

p1.powerUps = power1;
p2.powerUps = power2;

saveOrUpdatePlayer(p1);
saveOrUpdatePlayer(p2);

gLeaderboard.insertOrUpdate(p1.username, p1.totalPoints);
gLeaderboard.insertOrUpdate(p2.username, p2.totalPoints);

int idx1 = findPlayerIndexByName(p1.username);
int idx2 = findPlayerIndexByName(p2.username);

if (idx1 != -1 && idx2 != -1) {
        addFriendEdge(idx1, idx2);
        addFriendEdge(idx2, idx1);
        saveFriendsToFile();
    }

    return {score1, score2, winner};
}

    void drawBackground(RenderWindow& window, float winW, float winH) {
    if (bgFrameCount <= 0) return;
    if (currentBgFrame < 0 || currentBgFrame >= bgFrameCount) return;

    auto size = bgTextures[currentBgFrame].getSize();

    Sprite bg(bgTextures[currentBgFrame]);

    float scaleX = winW / static_cast<float>(size.x);
    float scaleY = winH / static_cast<float>(size.y);

    bg.setScale({scaleX, scaleY});
    bg.setPosition({0.f, 0.f});

    window.draw(bg);
}


    int main() {
        srand(static_cast<unsigned int>(time(0)));

        RenderWindow window(
        VideoMode({static_cast<unsigned int>(N*ts),
                static_cast<unsigned int>(M*ts + HUD_HEIGHT)}),
        "Xonix"
        );

            window.setFramerateLimit(60);

        {
            float winW = N * ts;
            float winH = M * ts + HUD_HEIGHT;

            bgFrameCount   = 0;
            currentBgFrame = 0;
            bgFrameTime    = 0.f;

            for (int frame = 0; frame < BG_MAX_FRAMES; ++frame) {
                int fileNo = frame + 1;  

                std::string num = std::to_string(fileNo);
                std::string padded(3 - (int)num.size(), '0'); 
                padded += num;                                

                std::string path = "images/bg_" + padded + ".png";

                if (!bgTextures[frame].loadFromFile(path)) {
                    if (frame == 0)
                        std::cerr << "No bg frames found at " << path << "\n";
                    break;
                }

                bgFrameCount++;
            }

            std::cout << "Loaded " << bgFrameCount << " background frames.\n";

        }

        Texture t1, t2, t3;
        t1.loadFromFile("images/tiles.png");
        t2.loadFromFile("images/gameover.png");
        t3.loadFromFile("images/enemy.png");


        Texture tLogo;
        if (!tLogo.loadFromFile("images/xonix.png")) {
            std::cerr << "Failed to load xonix.png\n";
        }

        Sprite sLogo(tLogo);

        const float logoScale = 0.1f;
        sLogo.setScale({logoScale, logoScale});   

        
        auto logoSize = tLogo.getSize();
        float logoWidth = logoSize.x * logoScale;

        float logoX = (N * ts - logoWidth) / 2.f;  
        float logoY = 30.f;                         

        sLogo.setPosition({logoX, logoY});         


        

        

        Font fontUI;
        if (!fontUI.openFromFile("3x5-font/3X5-6OYA.ttf")) {
            std::cerr << "Failed to load UI font\n";
        }


        Player currentPlayer;   
        Player currentPlayer2; 
        bool player2LoggedIn = false;

        GameState state = GameState::AUTH_MENU;

        
        int authMenuIndex = 0; 


        enum class InputField { USERNAME, PASSWORD };
        InputField loginField = InputField::USERNAME;
        string loginUser = "";
        string loginPass = "";
        string loginError = "";

        InputField regField = InputField::USERNAME;
        string regUser = "";
        string regPass = "";
        string regError = "";


        InputField login2Field = InputField::USERNAME;
        string login2User = "";
        string login2Pass = "";
        string login2Error = "";

        
        int menuIndex = 0;          
        int levelIndex = 0;         


        string loadSaveIdStr = "";
        string loadSaveMessage = "";


        ThemeNode* themeListArr[64];
        int themeListCount = 0;
        int themeIndex = 0; 
        string themeSearchIdStr = "";
        string themeMessage = "";


        bool mouseWasDown = false;

bool friendInputActive = false;      
string friendInputName = "";         
string friendSystemMessage = "";     


        int gameOverMenuIndex = 0;  
        int lastScore = 0;
        bool lastNewHigh = false;

        bool lastWasMultiplayer = false;
        int lastScoreP2 = 0;
        int lastWinner = 0; 

        Clock bgClock;


    initFriendSystemAll();
            initThemes();
        rebuildLeaderboardFromPlayersFile();


        while (window.isOpen()) {

                    float bgDt = bgClock.restart().asSeconds();

            if (bgFrameCount > 0) {
                bgFrameTime += bgDt;
                if (bgFrameTime >= BG_FRAME_DURATION) {
                    bgFrameTime = 0.f;
                    currentBgFrame++;
                    if (currentBgFrame >= bgFrameCount)
                        currentBgFrame = 0;
                }
            }

            while (const auto event = window.pollEvent()) {
                const Event& e = *event;
                if (e.is<Event::Closed>()) window.close();

if (state == GameState::LOGIN || state == GameState::REGISTER ||
    state == GameState::LOGIN_P2 || state == GameState::PROFILE ||
    state == GameState::LOAD_SAVE || state == GameState::INVENTORY) {

    if (const auto* te = e.getIf<Event::TextEntered>()) {
        char32_t uc = te->unicode;


        if (uc == 8) {
            if (state == GameState::LOGIN) {
                string& target = (loginField == InputField::USERNAME) ? loginUser : loginPass;
                if (!target.empty()) target.pop_back();
            }
            else if (state == GameState::REGISTER) {
                string& target = (regField == InputField::USERNAME) ? regUser : regPass;
                if (!target.empty()) target.pop_back();
            }
            else if (state == GameState::LOGIN_P2) {
                string& target = (login2Field == InputField::USERNAME) ? login2User : login2Pass;
                if (!target.empty()) target.pop_back();
            }
            else if (state == GameState::PROFILE && friendInputActive) {
                if (!friendInputName.empty()) friendInputName.pop_back();
            }
            else if (state == GameState::LOAD_SAVE) {
                if (!loadSaveIdStr.empty()) loadSaveIdStr.pop_back();
            }
            else if (state == GameState::INVENTORY) {
                if (!themeSearchIdStr.empty()) themeSearchIdStr.pop_back();
            }
        }
        
        else if (uc >= 32 && uc <= 126) {
            char c = static_cast<char>(uc);

            if (state == GameState::LOGIN) {
                string& target = (loginField == InputField::USERNAME) ? loginUser : loginPass;
                if (target.size() < 16) target.push_back(c);
            }
            else if (state == GameState::REGISTER) {
                string& target = (regField == InputField::USERNAME) ? regUser : regPass;
                if (target.size() < 16) target.push_back(c);
            }
            else if (state == GameState::LOGIN_P2) {
                string& target = (login2Field == InputField::USERNAME) ? login2User : login2Pass;
                if (target.size() < 16) target.push_back(c);
            }
            else if (state == GameState::PROFILE && friendInputActive) {
                if (friendInputName.size() < 16) friendInputName.push_back(c);
            }
            else if (state == GameState::INVENTORY) {
                if (c >= '0' && c <= '9' && themeSearchIdStr.size() < 6) themeSearchIdStr.push_back(c);
            }
            else if (state == GameState::LOAD_SAVE) {
                
                if (c >= '0' && c <= '9' && loadSaveIdStr.size() < 10) {
                    loadSaveIdStr.push_back(c);
                }
            }
        }
    }
}


               if (const auto* key = e.getIf<Event::KeyPressed>()) {
    
    if (key->scancode == Keyboard::Scancode::Escape) {


        if (state == GameState::PROFILE && friendInputActive) {
            friendInputActive = false;
            friendInputName.clear();
            friendSystemMessage = "Friend request input cancelled.";
        }
        else if (state == GameState::LOAD_SAVE) {
            
            state = GameState::MENU;
        }
        else if (state == GameState::AUTH_MENU) {
            window.close();
        } else if (state == GameState::LOGIN || state == GameState::REGISTER) {
            state = GameState::AUTH_MENU;
        } else if (state == GameState::MENU) {
            state = GameState::AUTH_MENU;
        } else if (state == GameState::LEVEL_SELECT ||
                   state == GameState::LEADERBOARD ||
                   state == GameState::PROFILE ||
                   state == GameState::INSTRUCTIONS ||
                   state == GameState::INVENTORY ||
                   state == GameState::LOGIN_P2) {
            state = GameState::MENU;
        } else if (state == GameState::GAME_OVER) {
            state = GameState::MENU;
        }
    }

                    
                    if (state == GameState::AUTH_MENU) {
                        if (key->scancode == Keyboard::Scancode::Up) {
                            if (authMenuIndex > 0) authMenuIndex--;
                        } else if (key->scancode == Keyboard::Scancode::Down) {
                            if (authMenuIndex < 2) authMenuIndex++;
                        } else if (key->scancode == Keyboard::Scancode::Enter) {
                            if (authMenuIndex == 0) {
                                loginUser.clear(); loginPass.clear();
                                loginError.clear();
                                loginField = InputField::USERNAME;
                                state = GameState::LOGIN;
                            } else if (authMenuIndex == 1) {
                                regUser.clear(); regPass.clear();
                                regError.clear();
                                regField = InputField::USERNAME;
                                state = GameState::REGISTER;
                            } else if (authMenuIndex == 2) {
                                window.close();
                            }
                        }
                    }

                    
                    else if (state == GameState::LOGIN) {
                        if (key->scancode == Keyboard::Scancode::Tab ||
                            key->scancode == Keyboard::Scancode::Down ||
                            key->scancode == Keyboard::Scancode::Up) {
                            loginField = (loginField == InputField::USERNAME)
                                    ? InputField::PASSWORD
                                    : InputField::USERNAME;
                        } else if (key->scancode == Keyboard::Scancode::Enter) {
                            if (loginUser.empty() || loginPass.empty()) {
                                loginError = "Fill both fields.";
                            } else {
                                Player p;
                                if (!loadPlayer(loginUser, p)) {
                                    loginError = "User not found.";
                                } else if (p.password != loginPass) {
                                    loginError = "Wrong password.";
                                } else {
                                    currentPlayer = p;
                                    
                                    currentThemeId = currentPlayer.themeId;
                                    addPlayerToFriendSystem(currentPlayer);   
                                    loginError.clear();
                                    state = GameState::MENU;
                                }

                            }
                        }
                    }

                  
                    else if (state == GameState::REGISTER) {
                        if (key->scancode == Keyboard::Scancode::Tab ||
                            key->scancode == Keyboard::Scancode::Down ||
                            key->scancode == Keyboard::Scancode::Up) {
                            regField = (regField == InputField::USERNAME)
                                    ? InputField::PASSWORD
                                    : InputField::USERNAME;
                        } else if (key->scancode == Keyboard::Scancode::Enter) {
                            if (regUser.empty() || regPass.empty()) {
                                regError = "Fill both fields.";
                            } else if (regPass.size() < 4) {
                                regError = "Password too short.";
                            } else {
                                Player temp;
                                if (loadPlayer(regUser, temp)) {
                                    regError = "Username already exists.";
                                } else {
                                    Player np;
                                    np.username = regUser;
                                    np.password = regPass;
                                    saveOrUpdatePlayer(np);
                                    currentPlayer = np;
                                    addPlayerToFriendSystem(currentPlayer);
                                    regError.clear();
                                    state = GameState::MENU;

                                }
                            }
                        }
                    }

                    
                    else if (state == GameState::LOGIN_P2) {
                        if (key->scancode == Keyboard::Scancode::Tab ||
                            key->scancode == Keyboard::Scancode::Down ||
                            key->scancode == Keyboard::Scancode::Up) {
                            login2Field = (login2Field == InputField::USERNAME)
                                    ? InputField::PASSWORD
                                    : InputField::USERNAME;
                        } else if (key->scancode == Keyboard::Scancode::Enter) {
                            if (login2User.empty() || login2Pass.empty()) {
                                login2Error = "Fill both fields.";
                            } else {
                                
                                if (login2User == currentPlayer.username) {
                                    login2Error = "Cannot use the same account as Player 1.";
                                } else {
                                    Player p;
                                    if (!loadPlayer(login2User, p)) {
                                        login2Error = "User not found.";
                                    } else if (p.password != login2Pass) {
                                        login2Error = "Wrong password.";
                                    } else {
                                       currentPlayer2 = p;
                                        addPlayerToFriendSystem(currentPlayer2);
                                        player2LoggedIn = true;
                                        login2Error.clear();
                                        state = GameState::MULTI_PLAYING;
                                    }
                                }
                            }
                        }
                    }

                   
                    else if (state == GameState::MENU) {
                        if (key->scancode == Keyboard::Scancode::Up) {
                            if (menuIndex > 0) menuIndex--;
                        } else if (key->scancode == Keyboard::Scancode::Down) {
                            if (menuIndex < 8) menuIndex++;  
                        } else if (key->scancode == Keyboard::Scancode::Enter) {
                            if (menuIndex == 0) {
                                state = GameState::PLAYING;
                            } else if (menuIndex == 1) {
                                
                                if (!player2LoggedIn) {
                                    login2User.clear();
                                    login2Pass.clear();
                                    login2Error.clear();
                                    login2Field = InputField::USERNAME;
                                    state = GameState::LOGIN_P2;
                                } else {
                                   
                                    if (currentPlayer2.username == currentPlayer.username) {
                                        
                                        player2LoggedIn = false;
                                        login2User.clear();
                                        login2Pass.clear();
                                        login2Error.clear();
                                        login2Field = InputField::USERNAME;
                                        state = GameState::LOGIN_P2;
                                    } else {
                                        addPlayerToMatchmakingQueue(currentPlayer.username);
                                        addPlayerToMatchmakingQueue(currentPlayer2.username);
                                        processMatchmakingQueue();   
                                        state = GameState::MULTI_PLAYING;
                                    }
                                }
                            } else if (menuIndex == 2) {
                                loadSaveIdStr.clear();
                                loadSaveMessage.clear();
                                state = GameState::LOAD_SAVE;
                            } else if (menuIndex == 3) {
                                state = GameState::LEVEL_SELECT;
                            } else if (menuIndex == 4) {
                                state = GameState::LEADERBOARD;
                            } else if (menuIndex == 5) {
                                state = GameState::PROFILE;
                            } else if (menuIndex == 6) {
                                state = GameState::INVENTORY;
                            } else if (menuIndex == 7) {
                                state = GameState::INSTRUCTIONS;
                            } else if (menuIndex == 8) {
                                window.close();
                            }
                        }
                    }

                    else if (state == GameState::LEVEL_SELECT) {
                        if (key->scancode == Keyboard::Scancode::Up) {
                            if (levelIndex > 0) levelIndex--;
                        } else if (key->scancode == Keyboard::Scancode::Down) {
                            if (levelIndex < LEVEL_COUNT - 1) levelIndex++;
                        } else if (key->scancode == Keyboard::Scancode::Enter) {
                            state = GameState::MENU;
                        }
                    }

                    else if (state == GameState::INVENTORY) {
                        if (themeListCount == 0) {
                            themeListCount = 0; int idx = 0;
                            fillThemeArrayInOrder(themeRoot, themeListArr, idx, 64);
                            themeListCount = idx;
                            if (themeIndex >= themeListCount) themeIndex = themeListCount > 0 ? themeListCount - 1 : 0;
                        }

                        if (key->scancode == Keyboard::Scancode::Up) {
                            if (themeIndex > 0) themeIndex--;
                        } else if (key->scancode == Keyboard::Scancode::Down) {
                            if (themeIndex + 1 < themeListCount) themeIndex++;
                        } else if (key->scancode == Keyboard::Scancode::Enter) {
                            if (!themeSearchIdStr.empty()) {
                                int tid = stoi(themeSearchIdStr);
                                ThemeNode* found = findThemeById(themeRoot, tid);
                                if (found) {
                                    currentThemeId = found->id;
                                    currentPlayer.themeId = currentThemeId;
                                    saveOrUpdatePlayer(currentPlayer);
                                    themeMessage = "Applied: " + found->name;
                                    themeSearchIdStr.clear();
                                } else {
                                    themeMessage = "Theme ID not found.";
                                }
                            } else if (themeListCount > 0) {
                                ThemeNode* sel = themeListArr[themeIndex];
                                currentThemeId = sel->id;
                                currentPlayer.themeId = currentThemeId;
                                saveOrUpdatePlayer(currentPlayer);
                                themeMessage = "Applied: " + sel->name;
                            }
                        }
                    }

                    else if (state == GameState::LOAD_SAVE) {
                        if (key->scancode == Keyboard::Scancode::Enter) {
                            if (loadSaveIdStr.empty()) {
                                loadSaveMessage = "Enter a Save ID.";
                            } else {
                                int id = stoi(loadSaveIdStr);
                                bool ok = loadGameById(id, currentPlayer.username);
                                if (ok) {
                                    loadSaveMessage = "Loaded save #" + to_string(id) + ". Starting game...";
                                    state = GameState::PLAYING;
                                } else {
                                    loadSaveMessage = "Save not found for your username.";
                                }
                            }
                        }
                        
                    }

                    else if (state == GameState::GAME_OVER) {
                        if (key->scancode == Keyboard::Scancode::Up) {
                            if (gameOverMenuIndex > 0) gameOverMenuIndex--;
                        } else if (key->scancode == Keyboard::Scancode::Down) {
                            if (gameOverMenuIndex < 2) gameOverMenuIndex++;
                        } else if (key->scancode == Keyboard::Scancode::Enter) {
                            if (gameOverMenuIndex == 0) {
                                if (lastWasMultiplayer) {
                                    if (!player2LoggedIn) {
                                        login2User.clear(); login2Pass.clear();
                                        login2Error.clear();
                                        login2Field = InputField::USERNAME;
                                        state = GameState::LOGIN_P2;
                                    } else {
                                        state = GameState::MULTI_PLAYING;
                                    }
                                } else {
                                    state = GameState::PLAYING;
                                }
                            } else if (gameOverMenuIndex == 1) {
                                state = GameState::MENU;
                            } else if (gameOverMenuIndex == 2) {
                                window.close();
                            }
                        }
                    }
                                        
            else if (state == GameState::PROFILE) {

    if (friendInputActive) {
        if (key->scancode == Keyboard::Scancode::Enter) {
            if (friendInputName.empty()) {
                friendSystemMessage = "Enter a username before sending request.";
            } else {
                bool ok = sendFriendRequest(currentPlayer.username, friendInputName);
                if (ok) {
                    friendSystemMessage = "Friend request sent to: " + friendInputName;
                } else {
                    friendSystemMessage = "Failed to send request (maybe already friends/pending or user not found).";
                }
            }
            friendInputActive = false;
            friendInputName.clear();
        }
    }
    else {
        if (key->scancode == Keyboard::Scancode::Num1)
            currentThemeId = 1;
        else if (key->scancode == Keyboard::Scancode::Num2)
            currentThemeId = 2;
        else if (key->scancode == Keyboard::Scancode::Num3)
            currentThemeId = 3;
        else if (key->scancode == Keyboard::Scancode::Num4)
            currentThemeId = 4;

        else if (key->scancode == Keyboard::Scancode::S) {
            friendInputActive = true;
            friendInputName.clear();
            friendSystemMessage = "Type username and press ENTER to send friend request.";
        }

        else if (key->scancode == Keyboard::Scancode::A) {
            string fromUser = getFirstPendingRequesterForUser(currentPlayer.username);
            if (fromUser.empty()) {
                friendSystemMessage = "No pending friend requests to accept.";
            } else {
                bool ok = acceptFriendRequest(currentPlayer.username, fromUser);
                if (ok)
                    friendSystemMessage = "Accepted friend request from: " + fromUser;
                else
                    friendSystemMessage = "Failed to accept friend request.";
            }
        }

        else if (key->scancode == Keyboard::Scancode::R) {
            string fromUser = getFirstPendingRequesterForUser(currentPlayer.username);
            if (fromUser.empty()) {
                friendSystemMessage = "No pending friend requests to reject.";
            } else {
                bool ok = rejectFriendRequest(currentPlayer.username, fromUser);
                if (ok)
                    friendSystemMessage = "Rejected friend request from: " + fromUser;
                else
                    friendSystemMessage = "Failed to reject friend request.";
            }
        }
    }
}


                }
            }

            const auto mp = Mouse::getPosition(window);
            const sf::Vector2f mousePosF{static_cast<float>(mp.x), static_cast<float>(mp.y)};
            const bool mouseDownNow = Mouse::isButtonPressed(Mouse::Button::Left);
            if (mouseDownNow && !mouseWasDown) {
                if (state == GameState::MENU) {
                    float uiWinWidth  = N * ts;
                    float uiWinHeight = M * ts + HUD_HEIGHT;
                    float centerX     = uiWinWidth / 2.f;
                    float logoHeight  = logoSize.y * logoScale;
                    float startY      = logoY + logoHeight + 8.f;
                    float optionSizeF = uiWinHeight * 0.06f;
                    float spacingY    = optionSizeF * 1.4f;

                    const float hitW = 360.f;
                    const float hitH = optionSizeF + 10.f;

                    for (int i = 0; i < 9; ++i) {
                        float y = startY + float(i) * spacingY;
                        FloatRect r({centerX - hitW/2.f, y}, {hitW, hitH});
                        if (r.contains(mousePosF)) {
                            menuIndex = i;
                            if (menuIndex == 0) {
                                state = GameState::PLAYING;
                            } else if (menuIndex == 1) {
                                if (!player2LoggedIn) {
                                    login2User.clear(); login2Pass.clear();
                                    login2Error.clear();
                                    login2Field = InputField::USERNAME;
                                    state = GameState::LOGIN_P2;
                                } else {
                                    addPlayerToMatchmakingQueue(currentPlayer.username);
                                    addPlayerToMatchmakingQueue(currentPlayer2.username);
                                    processMatchmakingQueue();
                                    state = GameState::MULTI_PLAYING;
                                }
                            } else if (menuIndex == 2) {
                                loadSaveIdStr.clear();
                                loadSaveMessage.clear();
                                state = GameState::LOAD_SAVE;
                            } else if (menuIndex == 3) {
                                state = GameState::LEVEL_SELECT;
                            } else if (menuIndex == 4) {
                                state = GameState::LEADERBOARD;
                            } else if (menuIndex == 5) {
                                state = GameState::PROFILE;
                            } else if (menuIndex == 6) {
                                state = GameState::INVENTORY;
                            } else if (menuIndex == 7) {
                                state = GameState::INSTRUCTIONS;
                            } else if (menuIndex == 8) {
                                window.close();
                            }
                            break;
                        }
                    }
                }

                else if (state == GameState::LEVEL_SELECT) {
                    float baseX = N*ts/2.f - 160.f;
                    float baseY = logoY + logoSize.y * logoScale + 8.f;
                    for (int i = 0; i < LEVEL_COUNT; ++i) {
                        FloatRect r({baseX, baseY + float(i)*30.f}, {320.f, 26.f});
                        if (r.contains(mousePosF)) {
                            levelIndex = i;
                            state = GameState::MENU; 
                            break;
                        }
                    }
                }
            }
            mouseWasDown = mouseDownNow;

            if (state == GameState::PLAYING) {
                bool newHigh = false;
                lastScore = runGameSession(window, t1, t2, t3,
                                        currentPlayer,
                                        levels[levelIndex],
                                        newHigh);
                lastNewHigh = newHigh;

                lastWasMultiplayer = false;
                lastScoreP2 = 0;
                lastWinner = 0;

                gameOverMenuIndex = 0;
                state = GameState::GAME_OVER;
                continue;
            }

            if (state == GameState::MULTI_PLAYING) {
                MultiResult mr = runMultiplayerSession(
                    window, t1, t2, t3,
                    currentPlayer,
                    currentPlayer2,
                    levels[levelIndex]  
                );

                lastWasMultiplayer = true;
                lastScore  = mr.score1;    
                lastScoreP2 = mr.score2;  
                lastWinner = mr.winner; 

                lastNewHigh = false;

                gameOverMenuIndex = 0;
                state = GameState::GAME_OVER;
                continue;
            }
            
    float uiWinWidth  = N * ts;
    float uiWinHeight = M * ts + HUD_HEIGHT;
    float centerX     = uiWinWidth / 2.f;

    unsigned int titleSize  = static_cast<unsigned int>(uiWinHeight * 0.10f); 
    unsigned int optionSize = static_cast<unsigned int>(uiWinHeight * 0.06f); 
    unsigned int smallSize  = static_cast<unsigned int>(uiWinHeight * 0.04f); 


            window.clear();

            if (state == GameState::AUTH_MENU) {
                window.clear();

            if (bgFrameCount > 0)
        drawBackground(window, uiWinWidth, uiWinHeight);


                RectangleShape darkRect;
                darkRect.setSize(Vector2f(uiWinWidth, uiWinHeight));
                darkRect.setPosition(Vector2f(0.f, 0.f));
                darkRect.setFillColor(Color(0, 0, 0, 200)); 
                window.draw(darkRect);

                Text title(fontUI, "WELCOME TO XONIX", titleSize);
                auto tb = title.getLocalBounds();
                title.setPosition({centerX - tb.size.x / 2.f, uiWinHeight * 0.15f});
                window.draw(title);

                string items[3] = {"Login", "Sign Up", "Exit"};
                float startY   = uiWinHeight * 0.40f;
                float spacingY = optionSize * 1.4f;

                for (int i = 0; i < 3; i++) {
                    Text t(fontUI, items[i], optionSize);
                    auto b = t.getLocalBounds();
                    float x = centerX - b.size.x / 2.f;
                    float y = startY + i * spacingY;
                    t.setPosition({x, y});

                FloatRect r({x, y}, {b.size.x, b.size.y});


                    bool hovered = r.contains(mousePosF);

                    if (i == authMenuIndex || hovered)
                        t.setFillColor(Color::Yellow);
                    else
                        t.setFillColor(Color::White);

                    window.draw(t);
                }
            }


            else if (state == GameState::LOGIN) {
        window.clear();

        if (bgFrameCount > 0)
        drawBackground(window, uiWinWidth, uiWinHeight);


        RectangleShape darkRect;
        darkRect.setSize(Vector2f(uiWinWidth, uiWinHeight));
        darkRect.setPosition(Vector2f(0.f, 0.f));
        darkRect.setFillColor(Color(0, 0, 0, 200));
        window.draw(darkRect);

        Text title(fontUI, "LOGIN (Player 1)", titleSize);
        auto tb = title.getLocalBounds();
        title.setPosition({centerX - tb.size.x / 2.f, uiWinHeight * 0.12f});
        window.draw(title);

        string uLabel = "Username: " + loginUser;
        string pLabel = "Password: " + maskPassword(loginPass);

        
        Text u(fontUI, uLabel, optionSize);
        auto ub = u.getLocalBounds();
        u.setPosition({centerX - ub.size.x / 2.f, uiWinHeight * 0.35f});
        u.setFillColor(loginField == InputField::USERNAME ? Color::Yellow : Color::White);
        window.draw(u);

        
        Text p(fontUI, pLabel, optionSize);
        auto pb = p.getLocalBounds();
        p.setPosition({centerX - pb.size.x / 2.f, uiWinHeight * 0.45f});
        p.setFillColor(loginField == InputField::PASSWORD ? Color::Yellow : Color::White);
        window.draw(p);

      
        Text hint(
            fontUI,
            "TAB / UP / DOWN to switch, ENTER to submit, ESC to back",
            smallSize
        );
        auto hb = hint.getLocalBounds();
        hint.setPosition({centerX - hb.size.x / 2.f, uiWinHeight * 0.60f});
        window.draw(hint);

        if (!loginError.empty()) {
            Text err(fontUI, loginError, smallSize);
            auto eb = err.getLocalBounds();
            err.setFillColor(Color::Red);
            err.setPosition({centerX - eb.size.x / 2.f, uiWinHeight * 0.70f});
            window.draw(err);
        }
    }

            else if (state == GameState::REGISTER) {
        window.clear();

        if (bgFrameCount > 0)
        drawBackground(window, uiWinWidth, uiWinHeight);


                RectangleShape darkRect;
                darkRect.setSize(Vector2f(uiWinWidth, uiWinHeight));
                darkRect.setPosition(Vector2f(0.f, 0.f));
                darkRect.setFillColor(Color(0, 0, 0, 200));
                window.draw(darkRect);

        Text title(fontUI, "SIGN UP (Player 1)", titleSize);
        auto tb = title.getLocalBounds();
        title.setPosition({centerX - tb.size.x / 2.f, uiWinHeight * 0.12f});
        window.draw(title);

        string uLabel = "Username: " + regUser;
        string pLabel = "Password: " + maskPassword(regPass);

        Text u(fontUI, uLabel, optionSize);
        auto ub = u.getLocalBounds();
        u.setPosition({centerX - ub.size.x / 2.f, uiWinHeight * 0.35f});
        u.setFillColor(regField == InputField::USERNAME ? Color::Yellow : Color::White);
        window.draw(u);

        Text p(fontUI, pLabel, optionSize);
        auto pb = p.getLocalBounds();
        p.setPosition({centerX - pb.size.x / 2.f, uiWinHeight * 0.45f});
        p.setFillColor(regField == InputField::PASSWORD ? Color::Yellow : Color::White);
        window.draw(p);

        Text hint(
            fontUI,
            "Password min length 4. TAB / UP / DOWN to switch, ENTER to submit, ESC to back",
            smallSize
        );
        auto hb = hint.getLocalBounds();
        hint.setPosition({centerX - hb.size.x / 2.f, uiWinHeight * 0.60f});
        window.draw(hint);

        if (!regError.empty()) {
            Text err(fontUI, regError, smallSize);
            auto eb = err.getLocalBounds();
            err.setFillColor(Color::Red);
            err.setPosition({centerX - eb.size.x / 2.f, uiWinHeight * 0.70f});
            window.draw(err);
        }
    }

            else if (state == GameState::LOGIN_P2) {
        window.clear();

        if (bgFrameCount > 0)
        drawBackground(window, uiWinWidth, uiWinHeight);


                RectangleShape darkRect;
                darkRect.setSize(Vector2f(uiWinWidth, uiWinHeight));
                darkRect.setPosition(Vector2f(0.f, 0.f));
                darkRect.setFillColor(Color(0, 0, 0, 200));
                window.draw(darkRect);

        Text title(fontUI, "LOGIN (Player 2)", titleSize);
        auto tb = title.getLocalBounds();
        title.setPosition({centerX - tb.size.x / 2.f, uiWinHeight * 0.12f});
        window.draw(title);

        string uLabel = "Username: " + login2User;
        string pLabel = "Password: " + maskPassword(login2Pass);

        Text u(fontUI, uLabel, optionSize);
        auto ub = u.getLocalBounds();
        u.setPosition({centerX - ub.size.x / 2.f, uiWinHeight * 0.35f});
        u.setFillColor(login2Field == InputField::USERNAME ? Color::Yellow : Color::White);
        window.draw(u);

        Text p(fontUI, pLabel, optionSize);
        auto pb = p.getLocalBounds();
        p.setPosition({centerX - pb.size.x / 2.f, uiWinHeight * 0.45f});
        p.setFillColor(login2Field == InputField::PASSWORD ? Color::Yellow : Color::White);
        window.draw(p);

        Text hint(
            fontUI,
            "Player 2 uses existing account. TAB / UP / DOWN to switch, ENTER to submit, ESC for Menu",
            smallSize
        );
        auto hb = hint.getLocalBounds();
        hint.setPosition({centerX - hb.size.x / 2.f, uiWinHeight * 0.60f});
        window.draw(hint);

        if (!login2Error.empty()) {
            Text err(fontUI, login2Error, smallSize);
            auto eb = err.getLocalBounds();
            err.setFillColor(Color::Red);
            err.setPosition({centerX - eb.size.x / 2.f, uiWinHeight * 0.70f});
            window.draw(err);
        }
    }

            else if (state == GameState::MENU) {
        window.clear();

        if (bgFrameCount > 0)
        drawBackground(window, uiWinWidth, uiWinHeight);


                RectangleShape darkRect;
                darkRect.setSize(Vector2f(uiWinWidth, uiWinHeight));
                darkRect.setPosition(Vector2f(0.f, 0.f));
                darkRect.setFillColor(Color(0, 0, 0, 200));
                window.draw(darkRect);

        
        window.draw(sLogo);

        float panelW = uiWinWidth * 0.65f;
        float panelH = uiWinHeight * 0.55f;
        float logoHeight = logoSize.y * logoScale;
        float panelX = centerX - panelW / 2.f;
        float panelY = logoY + logoHeight + 20.f;
        
        RectangleShape menuPanel;
        menuPanel.setSize(Vector2f(panelW, panelH));
        menuPanel.setPosition(Vector2f(panelX, panelY));
        menuPanel.setFillColor(Color(15, 15, 20, 230));
        menuPanel.setOutlineColor(Color(80, 150, 255, 180));
        menuPanel.setOutlineThickness(3.f);
        window.draw(menuPanel);

        string items[9];
        items[0] = "Single Player";
        items[1] = "Multi Player";
        items[2] = "Load Saved Game";
        items[3] = "Select Level";
        items[4] = "Leaderboard";
        items[5] = "Profile";
        items[6] = "Themes";
        items[7] = "Instructions";
        items[8] = "Exit Game";

        float startY = panelY + 30.f;
        float spacingY = (panelH - 60.f) / 9.f;

        for (int i = 0; i < 9; i++) {
            Text t(fontUI, items[i], optionSize);
            auto b = t.getLocalBounds();
            float x = centerX - b.size.x / 2.f;
            float y = startY + i * spacingY;
            
            t.setPosition({x, y});
            FloatRect r({x, y}, {b.size.x, b.size.y});
            bool hovered = r.contains(mousePosF);

            if (i == menuIndex || hovered)
                t.setFillColor(Color(255, 255, 100));
            else
                t.setFillColor(Color(220, 220, 220));

            window.draw(t);
        }

        string levelInfo = "Current Level: " + levels[levelIndex].name;
        Text levelText(fontUI, levelInfo, smallSize);
        auto lb = levelText.getLocalBounds();
        levelText.setFillColor(Color(100, 200, 255));
        levelText.setPosition({centerX - lb.size.x / 2.f, panelY + panelH + 15.f});
        window.draw(levelText);

        Text hint(fontUI, "Use UP/DOWN or mouse, ENTER to select", smallSize);
        auto hb = hint.getLocalBounds();
        hint.setFillColor(Color(180, 180, 180));
        hint.setPosition({centerX - hb.size.x / 2.f, uiWinHeight - hb.size.y - 10.f});
        window.draw(hint);
    }

            else if (state == GameState::INVENTORY) {
        window.clear();

        if (bgFrameCount > 0)
        drawBackground(window, uiWinWidth, uiWinHeight);

                RectangleShape darkRect;
                darkRect.setSize(Vector2f(uiWinWidth, uiWinHeight));
                darkRect.setPosition(Vector2f(0.f, 0.f));
                darkRect.setFillColor(Color(0, 0, 0, 200));
                window.draw(darkRect);

        Text title(fontUI, "THEME INVENTORY", titleSize);
        auto tb = title.getLocalBounds();
        title.setPosition({centerX - tb.size.x / 2.f, uiWinHeight * 0.10f});
        window.draw(title);

        RectangleShape panel;
        float panelW = uiWinWidth * 0.82f;
        float panelH = uiWinHeight * 0.58f;
        float panelX = centerX - panelW / 2.f;
        float panelY = uiWinHeight * 0.20f;
        panel.setSize(Vector2f(panelW, panelH));
        panel.setPosition(Vector2f(panelX, panelY));
        panel.setFillColor(Color(20, 20, 25, 200));
        panel.setOutlineColor(getCurrentHudColor());
        panel.setOutlineThickness(2.f);
        window.draw(panel);

        if (themeListCount == 0) {
            int idx = 0; themeListCount = 0;
            fillThemeArrayInOrder(themeRoot, themeListArr, idx, 64);
            themeListCount = idx;
            themeIndex = 0;
        }

        float listStartY = panelY + 20.f;
        float rowH = optionSize * 1.3f; 
        int maxRows = (int)((panelH - 40.f) / rowH);
        if (maxRows < 1) maxRows = 1;
        int startIdx = 0;
        if (themeIndex >= maxRows) startIdx = themeIndex - (maxRows - 1);

        for (int i = 0; i < maxRows && (startIdx + i) < themeListCount; ++i) {
            ThemeNode* t = themeListArr[startIdx + i];
            string line = t->name;
            Text row(fontUI, line, optionSize);
            auto rb = row.getLocalBounds();
            float y = listStartY + i * rowH;
            row.setPosition({panelX + 30.f, y});
            if ((startIdx + i) == themeIndex) row.setFillColor(Color::Yellow); else row.setFillColor(Color::White);
            window.draw(row);
        }

        float belowY = panelY + panelH + 10.f;
        ThemeNode* currentTheme = findThemeById(themeRoot, currentThemeId);
        string info = "Current Theme: " + (currentTheme ? currentTheme->name : "Unknown");
        Text infoText(fontUI, info, smallSize);
        auto ib = infoText.getLocalBounds();
        infoText.setPosition({panelX + 10.f, belowY});
        window.draw(infoText);

        string searchLine = "Search by ID: " + themeSearchIdStr + "_  (digits, ENTER applies)";
        Text sText(fontUI, searchLine, smallSize);
        auto sb2 = sText.getLocalBounds();
        sText.setFillColor(Color::Cyan);
        sText.setPosition({panelX + 10.f, belowY + smallSize * 1.4f});
        window.draw(sText);

        if (!themeMessage.empty()) {
            Text m(fontUI, themeMessage, smallSize);
            auto mb = m.getLocalBounds();
            m.setFillColor(Color(120,255,120));
            m.setPosition({panelX + 10.f, belowY + smallSize * 2.8f});
            window.draw(m);
        }

        Text hint(fontUI, "UP/DOWN to browse. ENTER to apply. ESC for Menu.", smallSize);
        auto hb = hint.getLocalBounds();
        hint.setPosition({centerX - hb.size.x / 2.f, uiWinHeight - hb.size.y - 10.f});
        window.draw(hint);
    }

            else if (state == GameState::LOAD_SAVE) {
        window.clear();

        if (bgFrameCount > 0)
        drawBackground(window, uiWinWidth, uiWinHeight);

        RectangleShape darkRect;
        darkRect.setSize(Vector2f(uiWinWidth, uiWinHeight));
        darkRect.setPosition(Vector2f(0.f, 0.f));
        darkRect.setFillColor(Color(0, 0, 0, 200));
        window.draw(darkRect);

        Text title(fontUI, "LOAD SAVED GAME", titleSize);
        auto tb = title.getLocalBounds();
        title.setPosition({centerX - tb.size.x / 2.f, uiWinHeight * 0.15f});
        window.draw(title);

        string prompt = "Enter Save ID: " + loadSaveIdStr + "_";
        Text tPrompt(fontUI, prompt, optionSize);
        auto pb = tPrompt.getLocalBounds();
        tPrompt.setPosition({centerX - pb.size.x / 2.f, uiWinHeight * 0.40f});
        tPrompt.setFillColor(Color::Yellow);
        window.draw(tPrompt);

        Text hintText(fontUI, "Type ID, ENTER to load, ESC for menu", smallSize);
        auto hb2 = hintText.getLocalBounds();
        hintText.setPosition({centerX - hb2.size.x / 2.f, uiWinHeight * 0.55f});
        window.draw(hintText);

        if (!loadSaveMessage.empty()) {
            Text tMsg(fontUI, loadSaveMessage, smallSize);
            auto mb = tMsg.getLocalBounds();
            tMsg.setFillColor(loadSaveMessage.find("not found") != string::npos ? Color::Red : Color::Cyan);
            tMsg.setPosition({centerX - mb.size.x / 2.f, uiWinHeight * 0.65f});
            window.draw(tMsg);
        }
    }

            else if (state == GameState::LEVEL_SELECT) {
        window.clear();

        Text title(fontUI, "SELECT MODE", titleSize);
        auto tb = title.getLocalBounds();
        title.setPosition({centerX - tb.size.x / 2.f, uiWinHeight * 0.12f});
        window.draw(title);

        float startY   = uiWinHeight * 0.30f;
        float spacingY = optionSize * 1.3f;

        for (int i = 0; i < LEVEL_COUNT; i++) {
            string txt = levels[i].name + " | ";

            if (levels[i].enemies == 0) {
                txt += "Enemies: 0 (No enemies)";
            } else {
                txt += "Enemies: " + to_string(levels[i].enemies);
                txt += " | Enemy Speed: ";
                txt += (levels[i].enemyDelay == ENEMY_SLOW ? "Slow" : "Fast");
            }

            Text t(fontUI, txt, optionSize);
            auto b = t.getLocalBounds();
            float x = centerX - b.size.x / 2.f;
            float y = startY + i * spacingY;
            t.setPosition({x, y});

            FloatRect r({x, y}, {b.size.x, b.size.y});

            bool hovered = r.contains(mousePosF);

            if (i == levelIndex || hovered)
                t.setFillColor(Color::Yellow);
            else
                t.setFillColor(Color::White);

            window.draw(t);
        }

        Text hint(fontUI, "ESC for Main Menu", smallSize);
        auto hb = hint.getLocalBounds();
        hint.setPosition({centerX - hb.size.x / 2.f, uiWinHeight - hb.size.y - 10.f});
        window.draw(hint);
    }


            else if (state == GameState::LEADERBOARD) {
        window.clear();

        if (bgFrameCount > 0)
        drawBackground(window, uiWinWidth, uiWinHeight);


                RectangleShape darkRect;
                darkRect.setSize(Vector2f(uiWinWidth, uiWinHeight));
                darkRect.setPosition(Vector2f(0.f, 0.f));
                darkRect.setFillColor(Color(0, 0, 0, 200));
                window.draw(darkRect);

        Text title(fontUI, "LEADERBOARD", titleSize);
        auto tb = title.getLocalBounds();
        title.setPosition({centerX - tb.size.x / 2.f, uiWinHeight * 0.10f});
        window.draw(title);

LeaderboardEntry tmp[10];
int n = gLeaderboard.size;

for (int i = 0; i < n; i++)
    tmp[i] = gLeaderboard.heap[i + 1];

for (int i = 0; i < n - 1; i++) {
    for (int j = 0; j < n - 1 - i; j++) {
        if (tmp[j].totalPoints < tmp[j + 1].totalPoints)
            std::swap(tmp[j], tmp[j + 1]);
    }
}

        float startY   = uiWinHeight * 0.28f;
        float spacingY = smallSize * 1.3f;

        for (int i = 0; i < n; i++) {
            string line = to_string(i+1) + ". " + tmp[i].username +
                        "  Total Points: " + to_string(tmp[i].totalPoints);
            Text t(fontUI, line, smallSize);
            auto b = t.getLocalBounds();
            t.setPosition({centerX - b.size.x / 2.f, startY + i * spacingY});
            window.draw(t);
        }

            


        Text hint(fontUI, "ESC for Main Menu", smallSize);
        auto hb = hint.getLocalBounds();
        hint.setPosition({centerX - hb.size.x / 2.f, uiWinHeight - hb.size.y - 10.f});
        window.draw(hint);
    }
    else if (state == GameState::PROFILE) {
    window.clear();

    if (bgFrameCount > 0)
        drawBackground(window, uiWinWidth, uiWinHeight);

    RectangleShape darkRect;
    darkRect.setSize(Vector2f(uiWinWidth, uiWinHeight));
    darkRect.setPosition(Vector2f(0.f, 0.f));
    darkRect.setFillColor(Color(0, 0, 0, 200));
    window.draw(darkRect);


    Text title(fontUI, "PLAYER PROFILE", titleSize);
    auto tb = title.getLocalBounds();
    title.setFillColor(Color::Yellow);
    title.setPosition({centerX - tb.size.x / 2.f, uiWinHeight * 0.08f});
    window.draw(title);

    float boxW = uiWinWidth * 0.60f;
    float boxH = uiWinHeight * 0.32f;
    float boxX = centerX - boxW/2.f;
    float boxY = uiWinHeight * 0.18f;
    RectangleShape profBox(Vector2f(boxW, boxH));
    profBox.setPosition(Vector2f(boxX, boxY));
    profBox.setFillColor(Color(30,30,60,180));
    profBox.setOutlineColor(Color::Cyan);
    profBox.setOutlineThickness(2.f);
    window.draw(profBox);

    string lines[6];
    lines[0] = "Username:   " + currentPlayer.username;
    lines[1] = "Total Points:   " + to_string(currentPlayer.totalPoints);
    lines[2] = "Best Score:   " + to_string(currentPlayer.bestScore);
    lines[3] = "Wins / Losses:   " + to_string(currentPlayer.wins) + " / " + to_string(currentPlayer.losses);
    lines[4] = "PowerUps:   " + to_string(currentPlayer.powerUps);
    lines[5] = "Theme ID:   " + to_string(currentThemeId);

    unsigned int infoSize = static_cast<unsigned int>(optionSize * 0.75f);
    float infoStartY = boxY + 20.f;
    float infoSpacing = (boxH - 30.f) / 6.f;
    for (int i = 0; i < 6; i++) {
        Text t(fontUI, lines[i], infoSize);
        t.setFillColor(Color(230, 230, 230));
        auto b = t.getLocalBounds();
        float y = infoStartY + i * infoSpacing;
        t.setPosition({centerX - b.size.x / 2.f, y});
        window.draw(t);
    }

    float friendsY = boxY + boxH + 18.f;
    Text friendsTitle(fontUI, "Friends:", smallSize);
    friendsTitle.setFillColor(Color(120,255,255));
    auto ftb = friendsTitle.getLocalBounds();
    friendsTitle.setPosition({centerX - ftb.size.x / 2.f, friendsY});
    window.draw(friendsTitle);

    string friendsLine = getFriendsLineForUser(currentPlayer.username);
    Text tf(fontUI, friendsLine, smallSize);
    tf.setFillColor(Color::White);
    auto fb = tf.getLocalBounds();
    tf.setPosition({centerX - fb.size.x / 2.f, friendsY + smallSize + 2.f});
    window.draw(tf);

    string pendingLine = getPendingRequestsLineForUser(currentPlayer.username);
    Text tr(fontUI, pendingLine, smallSize);
    tr.setFillColor(Color(255,180,120));
    auto rb = tr.getLocalBounds();
    tr.setPosition({centerX - rb.size.x / 2.f, friendsY + 2*smallSize + 10.f});
    window.draw(tr);

    string controlLine1 = "Friend System: S = Send Request, A = Accept first, R = Reject first";
    Text tCtrl1(fontUI, controlLine1, smallSize);
    auto c1b = tCtrl1.getLocalBounds();
    tCtrl1.setPosition({centerX - c1b.size.x / 2.f, uiWinHeight * 0.76f});
    window.draw(tCtrl1);

    if (friendInputActive) {
        string inputLine = "Send request to: " + friendInputName + "_";
        Text tInp(fontUI, inputLine, smallSize);
        auto ib = tInp.getLocalBounds();
        tInp.setFillColor(Color::Yellow);
        tInp.setPosition({centerX - ib.size.x / 2.f, uiWinHeight * 0.80f});
        window.draw(tInp);

        Text tHint(fontUI, "Type username, ENTER to send, ESC to cancel", smallSize);
        auto ihb = tHint.getLocalBounds();
        tHint.setPosition({centerX - ihb.size.x / 2.f, uiWinHeight * 0.84f});
        window.draw(tHint);
    } else {
        Text themeHint(fontUI, "Go to Menu > Themes Inventory to change theme", smallSize);
        auto thb = themeHint.getLocalBounds();
        themeHint.setPosition({centerX - thb.size.x / 2.f, uiWinHeight * 0.82f});
        window.draw(themeHint);
    }

    if (!friendSystemMessage.empty()) {
        Text tMsg(fontUI, friendSystemMessage, smallSize);
        auto mb = tMsg.getLocalBounds();
        tMsg.setFillColor(Color::Cyan);
        tMsg.setPosition({centerX - mb.size.x / 2.f, uiWinHeight * 0.88f});
        window.draw(tMsg);
    }

    if (player2LoggedIn) {
        Text p2title(fontUI, "Player 2: " + currentPlayer2.username, smallSize);
        auto pb = p2title.getLocalBounds();
        p2title.setPosition({centerX - pb.size.x / 2.f, uiWinHeight * 0.92f});
        window.draw(p2title);
    }

    Text hint(fontUI, "ESC for Main Menu", smallSize);
    auto hb = hint.getLocalBounds();
    hint.setPosition({centerX - hb.size.x / 2.f, uiWinHeight - hb.size.y - 10.f});
    window.draw(hint);
}




            else if (state == GameState::INSTRUCTIONS) {
        window.clear();

        if (bgFrameCount > 0)
        drawBackground(window, uiWinWidth, uiWinHeight);


                RectangleShape darkRect;
                darkRect.setSize(Vector2f(uiWinWidth, uiWinHeight));
                darkRect.setPosition(Vector2f(0.f, 0.f));
                darkRect.setFillColor(Color(0, 0, 0, 200));
                window.draw(darkRect);

        Text title(fontUI, "INSTRUCTIONS", titleSize);
        auto tb = title.getLocalBounds();
        title.setPosition({centerX - tb.size.x / 2.f, uiWinHeight * 0.08f});
        window.draw(title);

        string textLines[] = {
            "MODES:",
            "Noob     : 2 enemies, slow speed",
            "Beginner : 2 enemies, fast speed",
            "Average  : 4 enemies, slow speed",
            "Expert   : 4 enemies, fast speed",
            "Pro      : 6 enemies, slow speed",
            "God      : 6 enemies, fast speed",
            "Hacker   : 0 enemies (only your own trail can kill you)",
            "",
            "Single Player Controls:",
            "Arrow Keys to move, Space = PowerUp (3s protection).",
            "",
            "Multiplayer Controls:",
            "P1: Arrow Keys, PowerUp: Right Ctrl",
            "P2: Q/A/G/X, PowerUp: F (freeze enemies + other player 3s).",
            "",
            "Rules (Multiplayer):",
            "If both are drawing and collide: both die.",
            "Touching opponent's drawing trail: you die.",
            "Colliding with standing player while you are drawing:",
            "drawing player dies.",
            "",
            "Press ESC to return to Main Menu."
        };

        float startY   = uiWinHeight * 0.22f;
        float spacingY = smallSize * 1.2f;

        for (int i = 0; i < (int)(sizeof(textLines)/sizeof(textLines[0])); ++i) {
            Text t(fontUI, textLines[i], smallSize);
            auto b = t.getLocalBounds();
            t.setPosition({centerX - b.size.x / 2.f, startY + i * spacingY});
            window.draw(t);
        }
    }

            else if (state == GameState::GAME_OVER) {
        window.clear();

        if (bgFrameCount > 0)
        drawBackground(window, uiWinWidth, uiWinHeight);


                RectangleShape darkRect;
                darkRect.setSize(Vector2f(uiWinWidth, uiWinHeight));
                darkRect.setPosition(Vector2f(0.f, 0.f));
                darkRect.setFillColor(Color(0, 0, 0, 200));
                window.draw(darkRect);

        Sprite sGameover(t2);
        sGameover.setPosition({uiWinWidth * 0.15f, uiWinHeight * 0.05f});
        window.draw(sGameover);

        float textY = uiWinHeight * 0.45f;

        if (!lastWasMultiplayer) {
            Text tScore(fontUI, "Final Score: " + to_string(lastScore), optionSize);
            auto sb = tScore.getLocalBounds();
            tScore.setPosition({centerX - sb.size.x / 2.f, textY});
            window.draw(tScore);

            if (lastNewHigh) {
                Text tHigh(fontUI, "NEW HIGH SCORE!", optionSize);
                tHigh.setFillColor(Color::Yellow);
                auto hb = tHigh.getLocalBounds();
                tHigh.setPosition({centerX - hb.size.x / 2.f, textY + optionSize * 1.4f});
                window.draw(tHigh);
            }
        } else {
            Text tScore1(fontUI, "P1 Score: " + to_string(lastScore), optionSize);
            auto b1 = tScore1.getLocalBounds();
            tScore1.setPosition({centerX - b1.size.x / 2.f, textY});
            window.draw(tScore1);

            Text tScore2(fontUI, "P2 Score: " + to_string(lastScoreP2), optionSize);
            auto b2 = tScore2.getLocalBounds();
            tScore2.setPosition({centerX - b2.size.x / 2.f, textY + optionSize * 1.3f});
            window.draw(tScore2);

            string winText = "Result: Tie";
            if (lastWinner == 1) winText = "Winner: Player 1";
            else if (lastWinner == 2) winText = "Winner: Player 2";

            Text tWin(fontUI, winText, optionSize);
            auto wb = tWin.getLocalBounds();
            tWin.setPosition({centerX - wb.size.x / 2.f, textY + optionSize * 2.6f});
            window.draw(tWin);
        }

        string opts[3] = {"Restart", "Main Menu", "Exit Game"};
        float startY   = uiWinHeight * 0.70f;
        float spacingY = optionSize * 1.3f;

        for (int i = 0; i < 3; i++) {
            Text t(fontUI, opts[i], optionSize);
            auto b = t.getLocalBounds();
            float x = centerX - b.size.x / 2.f;
            float y = startY + i * spacingY;
            t.setPosition({x, y});

            FloatRect r({x, y}, {b.size.x, b.size.y});

            bool hovered = r.contains(mousePosF);

            if (i == gameOverMenuIndex || hovered)
                t.setFillColor(Color::Yellow);
            else
                t.setFillColor(Color::White);

            window.draw(t);
        }
    }


            window.display();
        }

        return 0;
    }
