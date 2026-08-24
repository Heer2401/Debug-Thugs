# Code Smell Audit — Original `main` Branch

> **Scope:** Analysis of the original baseline code as received on the `main` branch (`part5.cpp` single-player baseline).  
> **Confidence Model:** Calibrated using `review-accuracy-calibration` (C1–C4).  
> **Taxonomy:** Based on `detect-code-smells`.

---

## Accepted Code Smell Findings

### 1. Large Class (Bloater)
* **Severity:** HIGH
* **Confidence:** C3 — High
* **Location:** `part5.cpp:131` (Class `CyberSnake`)
* **Explanation:** `CyberSnake` acts as a God Class violating the Single Responsibility Principle. It combines low-level ANSI terminal rendering (`draw()`, `drawFrame()`, `drawSplashThenWaitKey()`), OS-specific keyboard polling (`getKeyNonBlocking()`, `handleInput()`), game loop timing (`run()`), and core game rules (movement, score calculation, fruit spawn, level progression, and collision detection). This tight coupling makes testing logic in isolation impossible and complicates any UI modifications.
* **Refactoring Technique:** **Extract Class** (`refactor-moving-features`). Separate responsibilities into dedicated abstractions: `GameEngine` (state & rules), `TerminalRenderer` (ANSI drawing), and `InputHandler` (keyboard polling).

---

### 2. Long Method (Bloater)
* **Severity:** HIGH
* **Confidence:** C3 — High
* **Location:** `part5.cpp:312` (Method `CyberSnake::step()`)
* **Explanation:** The `step()` method spans 47 lines and executes multiple distinct game loop phases: applying pending direction, computing next head position, evaluating wall and obstacle collisions, popping the tail, evaluating self-collisions, inserting the new head, checking fruit consumption, awarding points, accelerating game speed, leveling up, and optionally spawning new obstacles.
* **Refactoring Technique:** **Extract Method** (`refactor-composing-methods`). Decompose into focused helper methods such as `moveSnake()`, `checkCollisions()`, `handleFruitConsumption()`, and `maybeAddObstacle()`.

---

### 3. Primitive Obsession (Bloater)
* **Severity:** MEDIUM
* **Confidence:** C3 — High
* **Location:** `part5.cpp:126` (`struct Fruit`) & `part5.cpp:344-348`
* **Explanation:** The fruit representation relies on a raw integer primitive (`int type;`) rather than a domain abstraction. This introduces magic numbers (`0`, `1`, `2`) scattered across drawing routines, spawn calculations, and scoring/speed logic, reducing readability and type safety.
* **Refactoring Technique:** **Replace Type Code with Class** / **Replace Type Code with Enum** (`refactor-organizing-data`). Introduce a scoped enum `enum class FruitType { Normal = 0, Bonus = 1, Speed = 2 };`.

---

### 4. Long Method (Bloater)
* **Severity:** MEDIUM
* **Confidence:** C3 — High
* **Location:** `part5.cpp:207` (Method `CyberSnake::draw()`)
* **Explanation:** The `draw()` method spans 79 lines and handles screen clearing, frame border calculation, background checkerboard drawing, obstacle rendering, fruit color branching, snake body gradient generation, side HUD metrics formatting, and game-over/pause footer rendering all in a single sequential function.
* **Refactoring Technique:** **Extract Method** (`refactor-composing-methods`). Separate into helper functions: `drawBoard()`, `drawEntities()`, `drawHUD()`, and `drawFooter()`.

---
