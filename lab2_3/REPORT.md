# Lab 2_3 — Group A39

---

## 1. Tool and install route — [3]

| | |
|---|---|
| Agent used for run 2 | Antigravity AI Agent |
| ubiquitous-language install route | project .agents/skills |
| refactoring/ pack install route | project .agents/skills |

If anything would not install, say what failed here. The paste fallback is a documented
route and costs no marks.

---

## 2. What I changed in the glossary — [4]

The generated file is at lab2_3/UBIQUITOUS_LANGUAGE.md. Three or four lines here on what
you corrected and why: terms it invented, definitions it got wrong, ambiguities it missed.

- Corrected Fruit citations from raw implementation logic (`type == 0`) to actual text evidence.
- Removed "Code drift" related to magic numbers, as they are architectural issues, not terminology drift.
- Removed "Player" as an alias for "Snake" and added separate entries for Player, Board, Head, and Tail. Added a "Flagged ambiguities" section resolving the Player vs. Snake conflation.

---

## 3. Smell delta — [6]

Reports: lab2_3/audits/main.md (the code as you received it) and lab2_3/audits/lab1-head.md
(after your Lab-1 PR).

| Category | count | representative site (file:line) |
|---|---|---|
| Smells my Lab-1 PR **introduced** | 1 | part5.cpp:409 |
| Smells my Lab-1 PR **left untouched** | 4 | part5.cpp:140 |
| Smells my Lab-1 PR **removed** | 0 | - |

The third row will probably be zero. Leave it in.

---

## 4. Rejected candidates — [6]

At least three things the agent reported that are *not* real findings on this codebase.

| smell reported | file:line | why it does not hold |
|---|---|---|
| Switch Statements | part5.cpp:348 | Standard mapping for 4-way direction enumeration to coordinate offsets; replacing this with polymorphism or strategy objects would be severe over-engineering and create Speculative Generality. |
| Data Class | part5.cpp:30 | `struct Pt`, `struct Fruit`, and `struct Player` are C++ Plain Old Data (POD) aggregates used as value types; adding artificial methods/getters/setters would add boilerplate without meaningful encapsulation benefit. |
| Comments (excessive) | part5.cpp:124 | Minimal section header dividers (e.g., `//Game`, `//Gameplay`, `//Input handling`) and execution instructions provide visual structure in a single-file game rather than disguising convoluted code. |

---

## 5. Refactor plan — [6]

| # | sha | subject | what it is |
|---|---|---|---|
| 1 | c95357c | Add ubiquitous language glossary | glossary |
| 2 | e620dc6 | Add code smell audit | smell report |
| 3 | 2376ac8 | Refactor snake count representation | the refactor alone |
| 4 | 8ba9004 | Add second player feature | the feature alone |

---

## 6. Commit log — [6]

| Run | Step | Changes (-w) | Changes (raw) |
|---|---|---|---|
| Run 1 (Lab 1) | Feature PR | 1 file changed, 114 insertions(+), 41 deletions(-) | 1 file changed, 142 insertions(+), 58 deletions(-) |
| Run 2 (Lab 2) | Commit 3 (Refactor) | 1 file changed, 77 insertions(+), 46 deletions(-) | 1 file changed, 467 insertions(+), 436 deletions(-) |
| Run 2 (Lab 2) | Commit 4 (Feature) | 1 file changed, 86 insertions(+), 29 deletions(-) | 1 file changed, 96 insertions(+), 39 deletions(-) |

---

## 7. What I learned about ubiquitous language — [4]

**Q1: Which smell did Commit 3 actually fix?**
Commit 3 fixed the **Data Clumps** smell at `part5.cpp:183-189`. Previously, snake state (`body`, `dir`, `pending_dir`, `score`, `dir_changed`) was held as loose, independent variables. Adding an extra snake in Lab 1 required duplicating all these variables (`snake2`, `score2`, etc.) across 6 functions. Commit 3 encapsulated snake state into `struct Snake` and parameterized the collection via `NUM_SNAKES`. Consequently, snake count is defined in exactly one place, allowing game loops to iterate generically without duplication.

**Q2: Comparing Commit 4 with Lab 1 implementation**
In Lab 1, adding the second player introduced structural duplication and hardcoded parallel variables. In Run 2 (Commit 4), because the architecture already operated over a collection of `Snake` objects, implementing the feature only required adjusting `NUM_SNAKES = 2`, initializing starting offsets, and routing player 2 key inputs. Feature diff dropped from 114 insertions to 86 insertions (-w) with zero newly introduced code smells.

---

## 8. What I learned about refactoring with an AI agent — [5]

**Q3: Did the assistant suggest restructuring in Lab 1?**
No. In `LLM-LOG.md`, Prompt #5 explicitly instructed the assistant: *"Do not add new menus, colors, renaming, or refactoring."* The assistant followed these negative constraints strictly and implemented the feature via immediate duplication without proposing structural improvements. For the assistant to suggest restructuring first, the prompt would need to explicitly ask: *"Analyze existing code smells and suggest design improvements before implementing the second player."*

**Q4: How do you know Commit 3 did not change behaviour?**
We confirmed behavioural equivalence by verifying that `NUM_SNAKES = 1` maintained identical single-player gameplay. The snake spawned at the exact center with length 5, responded to both Arrow and WASD keys, accumulated identical scores across normal/bonus/speed fruits, scaled level and speed at identical thresholds, and triggered game-over on wall/obstacle/self collisions without regression.

---
