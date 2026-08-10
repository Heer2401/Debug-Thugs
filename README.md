# 🐍 Cyber-Neon Snake

A **modern, terminal-based Snake game** with **cyberpunk neon visuals**, built entirely in **C++**.  
The game runs directly in your console window with smooth animations, colorful gradients, and enhanced controls.

---

## ✨ Features

- 🟢 **Neon-Themed Interface** — dynamic color gradients for a futuristic look.  
- 🍎 **Three Fruit Types**:
  - `●` Normal fruit — +10 points  
  - `★` Bonus fruit — +25 points  
  - `✦` Speed fruit — +8 points and faster gameplay  
- 🚧 **Progressive Obstacles** — appear as you advance through levels.  
- 🌈 **Animated Tail Gradient** — snake body glows with fading neon tones.  
- 🎮 **Smooth Controls & Input Handling** — supports both arrow keys and WASD.  
- 💀 **Game Over / Pause / Restart** options built-in.  
- 🖥️ **Cross-platform** — works on Windows, Linux, and macOS terminals that support ANSI escape codes.

---

# ⚙️ Setup & Compilation

### 🪟 For Windows

1. **Install g++ (MinGW or similar).**
2. Place all source files (e.g., `part5.cpp`) in a folder.
3. Open **Command Prompt** or **PowerShell** in that folder.
4. Run the following commands:
```powershell
g++ part5.cpp -o part5
chcp 65001
.\part5.exe
```
🐧 For Linux / macOS

Open a terminal in your project directory.

Run:
```powershell
g++ part5.cpp -o part5
./part5
```

🕹️ Controls
Key	Action
⬆️ / W	Move Up,
⬇️ / S	Move Down,
⬅️ / A	Move Left,
➡️ / D	Move Right,
P	Pause / Resume,
R	Restart,
Q	Quit Game
🧩 Game Logic Overview

The snake grows when it eats a fruit.

Every few fruits, new obstacles appear.

The speed increases progressively as you level up or collect speed fruits.

Collision with walls, obstacles, or self results in Game Over.

Dynamic delay and adaptive difficulty are managed using a timing loop with std::chrono.

🧠 Technical Highlights

Fully object-oriented design (CyberSnake class encapsulates gameplay).

Uses ANSI escape codes for color, cursor control, and screen clearing.

Cross-platform keyboard input handling (conio.h on Windows, termios on Unix).

Real-time game loop using std::chrono::steady_clock.

Gradient color effects calculated dynamically with sine/cosine functions.
