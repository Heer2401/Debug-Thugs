# Code Smell Analysis

### 1. Accepted Code Smell Findings

These findings represent genuine architectural or design weaknesses that negatively impact the maintainability and scalability of the codebase.

#### Large Class
* **Severity:** HIGH
* **Citation:** part5.cpp:123 (Class CyberSnake)
* **Explanation:** This class violates the Single Responsibility Principle. It acts as a "God Class" that manages ANSI terminal rendering, OS-specific keyboard polling, game loop timing, and core game logic (collisions, score, snake growth). This tight coupling makes it very difficult to change the rendering engine without risking gameplay bugs, or to test the game logic in isolation.
* **Refactoring Technique:** **Extract Class**. Separate responsibilities into independent classes like GameEngine (logic), TerminalRenderer (ANSI drawing), and InputHandler (keyboard).

#### Duplicate Code
* **Severity:** HIGH
* **Citation:** part5.cpp:372-382
* **Explanation:** The complex business logic for collision detection is completely copy-pasted. The exact same boundary, obstacle, and self-hit evaluations are run for p1_hit on nxt, and then identically repeated for p2_hit on nxt2. If collision rules change, the developer must remember to update multiple places, which frequently introduces bugs.
* **Refactoring Technique:** **Extract Method**. Create a reusable collision function, e.g., bool checkHit(Pt head, const vector<Pt>& self, const vector<Pt>& other).

#### Data Clumps
* **Severity:** MEDIUM
* **Citation:** part5.cpp:189-197
* **Explanation:** The same groups of variables appear together repeatedly, which is a strong indicator of a missing domain abstraction. The class declares paired variables for every player trait: snake/snake2, dir/dir2, pending_dir/pending_dir2, p1Lost/p2Lost, and score/score2. This is a procedural anti-pattern leaking into object-oriented design.
* **Refactoring Technique:** **Extract Class**. Introduce a Player or SnakeState class that encapsulates a snake's coordinate queue, current direction, score, and alive status.

#### Long Method
* **Severity:** HIGH
* **Citation:** part5.cpp:342 (Method CyberSnake::step())
* **Explanation:** The method spans over 70 lines and does far too much. It calculates the next position for both snakes, handles fruit consumption, evaluates all collisions, updates the delay interval, increments the level, and triggers new obstacle spawns.
* **Refactoring Technique:** **Extract Method**. Break the execution down into smaller, self-documenting methods like moveSnakes(), evaluateCollisions(), and handleFruitConsumption().

#### Primitive Obsession
* **Severity:** MEDIUM
* **Citation:** part5.cpp:120 (struct Fruit)
* **Explanation:** The code uses raw primitives (magic numbers) to represent specific domain concepts. int type; dictates the fruit type, and scattered across the codebase are arbitrary checks like type == 0, type == 1, and type == 2 to determine scoring and color.
* **Refactoring Technique:** **Replace Type Code with Class** or, at a minimum, introduce an enum class FruitType { Normal, Bonus, Speed }.

---

### 2. Rejected Code-Smell Candidates

These are plausible findings that a novice reviewer might flag, but they should be **rejected** because they represent acceptable, pragmatic C++ usage rather than genuine smells.

#### Rejected Candidate: Switch Statements
* **Citation:** part5.cpp:348-353 (Switching on dir for movement).
* **Explanation:** The skill lists "Switch Statements" as an Object-Orientation Abuser, suggesting polymorphism should be used instead of switching on type codes.
* **Why it is rejected:** In a simple grid-based game, mapping an enumeration (UP, DOWN, LEFT, RIGHT) to coordinate math (nxt.r--) via a switch is the most readable and efficient approach. Attempting to replace this with Polymorphism (e.g., creating an UpMovementStrategy class) would be severe over-engineering and would introduce the **Speculative Generality** smell.

#### Rejected Candidate: Data Class
* **Citation:** part5.cpp:118 (struct Fruit) and part5.cpp:22 (struct Pt).
* **Explanation:** The skill identifies classes that contain only fields (no behavior) as the "Data Class" smell. 
* **Why it is rejected:** C++ structs intended strictly as Plain Old Data (POD) aggregates are not code smells. Adding arbitrary getters/setters or trying to force behavior into a simple x,y coordinate struct (Pt) adds unnecessary bloat without providing meaningful encapsulation.

#### Rejected Candidate: Comments (Excessive)
* **Citation:** part5.cpp:1-5 and standard section headers like //Gameplay (part5.cpp:310).
* **Explanation:** The skill states that excessive comments suggest code that is too complex to be self-documenting.
* **Why it is rejected:** The comments in this file are minimal and serve either as necessary compilation instructions (//execution commands:) or as high-level visual dividers. There are no inline comments desperately trying to explain convoluted logic, so the code does not suffer from the "Excessive Comments" smell.
