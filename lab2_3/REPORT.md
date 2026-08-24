# Lab 2_3 — Group A

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

---

## 6. Commit log — [6]

---

## 7. What I learned about ubiquitous language — [4]

---

## 8. What I learned about refactoring with an AI agent — [5]

---
