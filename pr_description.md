# Part A - Local Multiplayer Implementation

## Measurement

| Metric | Details |
|--------|---------|
| **Functions changed (headline number)** | 8 functions + class fields |
| **Functions changed** | `CyberSnake::init()`, `CyberSnake::run()`, `CyberSnake::placeFruit()`, `CyberSnake::maybeAddObstacle()`, `CyberSnake::step()`, `CyberSnake::handleInput()`, `CyberSnake::drawSmooth()`, `(global)` member declarations |
| **Files changed** | `part5.cpp`, `part52.cpp`, `README.md` |
| **Lines added / deleted** | 270 insertions(+), 145 deletions(-) |
| **% of codebase touched** | ~22.6% (415 changed lines / ~1831 total lines) |
| **Hours spent** | ~2 hours |
| **LLM model + tool** | Claude Opus 4.6 via Antigravity IDE Agent |
| **LLM prompts to working code** | 3 |
| **Code given to LLM** | selected functions in `part5.cpp` & `part52.cpp` (`step()`, `handleInput()`, `drawSmooth()`, `placeFruit()`, `maybeAddObstacle()`) |
| **Assignment document pasted into LLM?** | Yes |

---

## Analysis Questions

### 1. Which functions did you have to change that are not about snakes at all? Why did a second snake reach them?

We had to change `placeFruit()`, `maybeAddObstacle()`, `drawSmooth()`, and the main `run()` loop. A second snake reached these functions because the environment generation (fruits and obstacles) and the rendering system assumed there was only one snake entity to check against. For example, `placeFruit()` checks the board to avoid spawning a fruit inside a player, which inherently requires knowing every snake's position.

### 2. Was there a single place where "there is exactly one snake" was written down — or was that assumption spread across the code?

The assumption was heavily spread across the code. Rather than having a unified `Player` or `Snake` object, the snake's state (its `deque`, direction, alive status, and score) was stored as individual standalone variables directly inside the main `CyberSnake` class. This forced us to duplicate variables (`snake2`, `dir2`, `score2`) and manually inject the second snake into every phase of the game loop: input, physics, collision, and rendering.

### 3. In Lecture 3 your group predicted which functions would change. Compare your prediction to what actually happened. What did you miss, and why was it not visible from reading?

In Lecture 3, our group predicted that `placeFruit()`, `maybeAddObstacle()`, `drawSplashThenWaitKey()/init()`, `handleInput()`, `step()`, and `draw()` would change. Comparing this to the actual diff, our structural prediction was spot-on regarding the core logic and rendering functions. However, we missed `run()` (where `dir_changed2` reset logic was added inside the game loop) and the `(global)` member variable declarations required in the class scope. `run()` was not obvious from reading because key input flags seemed encapsulated within `handleInput()`, whereas in reality `run()` maintained frame-by-frame state resets for input throttling.

### 4. What one design change to this codebase would have made this a twenty-minute job?

Refactoring the code by introducing a `Snake` or `Player` struct/class that encapsulates attributes such as body, direction, score, controls, and colors. This allows multiple players to be managed as a collection (e.g., `std::vector<Player>`) instead of relying on hardcoded single-player variables in `part5.cpp`.
