# PA2: Aquarium

This project is a fun multi-directionaly iteractive evolution game . This repository is part of a Programming Assignment for the Advanced Programming Course. The purpose of this project is to creatively help the student create practical programming skills by implementing changes to the base code that require strong understanding of the concepts discussed in class.

Base code developed by: Max R. Berrios Cruz

PS: Any blame or criticism is not to be put on the author, blame the staff instead.

Thank you and happy coding!

Att. NOT Max R. Berrios Cruz

# Setup
---

    Clone this project inside an OpenFrameworks installation's "apps/myApps/" directory
    Build & Run the project

Submitting Assignment

To submit this assignment, create a Release with the branch that holds all the changes you have added. If no Release is made, the last commit to the master branch will be graded. Don't forget that you must also fill out an evaluation form in Moodle to consider the project submitted.
Grading

## Team Info
| Student Number | Student Name | Student Email |
|-|-|-|
|802-23-8798| Letizia M Mayor | letizia.mayor@upr.edu |
|(STUDENT NUMBER HERE)| (STUDENT NAME HERE)| (STUDENT EMAIL HERE) |

---

Any project that doesn't compile will receive a 0.

You MUST put you & your partner's information in this README file otherwise there will be a 5pts penalty!

If a partner has no commits in the repositories, they will receive a 0.

# Student Notes
If you have any bonus specs, bonus or any details the TA's should know, you should include it here:

How the game works 

The Power System

You start at Power Level 1
You can only eat fish weaker than you (strictly lower value)
Eating fish increases your score and power
You lose a life if you touch stronger fish
You start with 3 lives - game over when they're gone
Fish Values to Remember

Small fish (white) = 1
Blue fish = 3
Red fish = 4
Violet fish = 5
Lurker fish = 6 (grows over time!)
ZigZag/Bigger fish = 7
Sharks = 8 (most dangerous!)

Level Progression

6 levels total with increasing difficulty
Each level has a score target (10, 15, 20, 25, 35, 50)
Reach the target to advance
You get 5 seconds of invincibility when leveling up (flashing effect)
Power-Ups ⚡

Yellow fish(base-fish but yellow/green) = Speed boost (1.5x faster for 10 seconds)
Spawns every 20 seconds
Affects both you AND other fish
