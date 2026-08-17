# Ubiquitous Language

## Entities

| Term | Definition | Aliases to avoid | In code |
| :--- | :--- | :--- | :--- |
| **Snake** | The player-controlled entity that navigates the board, eats fruit, and grows. | | `CyberSnake`, `snake` - `part5.cpp:189` |
| **Player** | The human participant controlling a Snake in the multiplayer game. | | `Player 1`, `p1Lost` - `part5.cpp:298` |
| **Fruit** | An object the snake consumes to gain points, grow, and potentially alter game speed. | Apple, Food | `Fruit` - `part5.cpp:118` |
| **Obstacle** | A static hazard on the board that causes game over upon collision. | Wall, Block | `obstacles` - `part5.cpp:191` |
| **Board** | The grid area where the game takes place. | | `board` - `part5.cpp:139` |
| **Head** | The leading part of the Snake, determining direction and collisions. | | `head` - `part5.cpp:346` |
| **Tail** | The trailing segments of the Snake, featuring a visual gradient. | | `Tail Gradient` - `README.md:16` |

## Fruit Types

| Term | Definition | Aliases to avoid | In code |
| :--- | :--- | :--- | :--- |
| **Normal fruit** | Grants standard points (+10) and causes the snake to grow. | | `normal` - `part5.cpp:288`, `README.md:12` |
| **Bonus fruit** | Grants extra points (+25) and causes the snake to grow. | | `bonus` - `part5.cpp:288`, `README.md:13` |
| **Speed fruit** | Grants points (+8), causes the snake to grow, and increases game speed. | | `speed` - `part5.cpp:288`, `README.md:14` |

## Concepts and State

| Term | Definition | Aliases to avoid | In code |
| :--- | :--- | :--- | :--- |
| **Level** | A measure of progress; increases every 4 fruits eaten, boosting game speed. | | `level` - `part5.cpp:198` |
| **Score** | The total points accumulated by a player. | | `score`, `score2` - `part5.cpp:198` |
| **Game Over** | The state reached when a snake collides with a wall, an obstacle, or itself. | | `GAME OVER` - `part5.cpp:296` |
| **Delay** | The time interval between game ticks, determining the speed of the snake. | Speed | `curDelay` - `part5.cpp:199` |

## Example dialogue

> **Dev:** "What happens when the **Snake** eats a **Speed fruit**?"
> **Domain expert:** "The **Score** increases by 8 points, the **Snake** grows, and the **Delay** decreases, making the game move faster."
> **Dev:** "Does it affect the **Level**?"
> **Domain expert:** "It counts towards the total fruits eaten. Every 4 fruits eaten, regardless of type, the **Level** increases and the **Delay** decreases further."

## Flagged ambiguities

- **Player vs Snake:** In earlier iterations, "Player" was incorrectly treated as an alias for "Snake". These are distinct domain concepts: a **Player** is the human participant interacting with the game (e.g., Player 1, Player 2), while the **Snake** is the in-game entity they control. They should not be used interchangeably.

