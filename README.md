<div align="center">
  <img src="images/xonix.png" alt="Xonix Logo" width="200"/>
  <h1>Xonix Game</h1>
  <p>Classic Xonix arcade game recreated in C++ using data structure concepts. Features smooth gameplay mechanics, collision detection, and progressive difficulty levels.</p>
</div>

---

## 📁 Project Structure

```text
xonix-game/
├── CMakeLists.txt        # CMake build configuration
├── k.cpp                 # Main C++ source code containing game logic and SFML rendering
├── 3x5-font/             # Custom 3x5 bitmap font assets
├── arial/                # Arial font assets for UI text
├── images/               # Game assets including backgrounds, sprites, and UI elements
│   ├── enemy.png         # Enemy sprite
│   ├── tiles.png         # Game board tiles
│   └── gameover.png      # Game over screen overlay
├── players.txt           # Player profiles and high scores storage
├── saves.txt             # Game save state data
├── friends.txt           # Social features (Friends list)
└── friend_requests.txt   # Social features (Friend requests)
```

## 📥 Installation

### Prerequisites

- C++ compiler with C++11 support (GCC, Clang, MSVC)
- CMake 3.10 or higher
- SFML 2.x or 3.x library

### Windows Installation

1. **Install SFML**: Download SFML from [https://www.sfml-dev.org/download.php](https://www.sfml-dev.org/download.php)
2. Extract to a directory (e.g., `C:\SFML`)
3. Add SFML to your PATH or set CMAKE_PREFIX_PATH
4. **Clone the Repository**:
```bash
git clone https://github.com/yourusername/xonix-game.git
cd xonix-game
```
5. **Build with CMake**:
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Linux Installation

1. **Install Dependencies**:
```bash
# Ubuntu/Debian
sudo apt-get install libsfml-dev cmake g++

# Fedora
sudo dnf install SFML-devel cmake gcc-c++

# Arch Linux
sudo pacman -S sfml cmake gcc
```

2. **Build the Project**:
```bash
git clone https://github.com/yourusername/xonix-game.git
cd xonix-game
mkdir build && cd build
cmake ..
make
```

### macOS Installation

1. **Install Dependencies** (using Homebrew):
```bash
brew install sfml cmake
```

2. **Build the Project**:
```bash
git clone https://github.com/yourusername/xonix-game.git
cd xonix-game
mkdir build && cd build
cmake ..
make
```

## 🎮 How to Play

### Controls

#### Single Player:

- **Arrow Keys**: Move the player
- **Space**: Use freeze power-up (if available)
- **ESC**: Pause/Menu

#### Multiplayer (2 Players):

- **Player 1**: Arrow Keys
- **Player 2**: W, A, S, D keys
- **Space**: Player 1 power-up
- **Shift**: Player 2 power-up

### Gameplay Rules

1. **Capturing Territory**: Move from captured land (colored tiles) onto empty space
2. Draw a trail back to captured land to claim new territory
3. Filled area is calculated using flood-fill algorithm
4. **Scoring**: Points awarded based on % of territory captured
5. Bonus multipliers for consecutive captures
6. Extra power-ups earned at score milestones
7. **Enemies**: 5 enemies move randomly on the board
8. Avoid enemy collision with your trail
9. Freeze power-ups temporarily stop all enemies
10. **Lives**: Start with 3 lives
11. Lose a life when enemy hits your trail
12. Game over when all lives are lost
13. **Winning**: Capture ≥80% of the board to win
14. Higher difficulty = more points
15. Beat high scores to climb leaderboard

### Difficulty Levels

| Level | Enemy Speed | Points Multiplier | Description |
| :--- | :--- | :--- | :--- |
| 1 - Easy | Slow | 1x | Perfect for beginners |
| 2 - Normal| Medium | 1.5x | Balanced gameplay |
| 3 - Hard | Fast | 2x | Challenging experience |
| 4 - Expert| Very Fast | 2.5x | For skilled players |
| 5 - Extreme| Maximum | 3x | Ultimate challenge |

## 🛠️ Built With
* **C++** - Core logic
* **SFML** - Graphics and media library
* **CMake** - Build system

## 📝 License
This project is open-source and free to use.