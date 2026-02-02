# Postfix Equation Solver with Backtracking

This project implements an algorithmic solver for **Postfix Expressions (Reverse Polish Notation)** containing unknown variables. It determines all valid combinations of operators and operands that satisfy a given mathematical equation.

## Key Features
* **Backtracking Algorithm:** Utilizes a recursive approach to explore potential values for unknown variables, backtracking when invalid states are encountered.
* **Custom Stack Implementation:** Manages postfix evaluation using a dynamic linked-list-based stack structure.
* **Deep Copy & Memory Management:** Implements custom deep copy functions for stacks and strings to ensure data integrity during recursive branching.
* **Constraint Satisfaction:** Solves for unknowns by validating syntax rules and mathematical constraints simultaneously.

## Technical Stack
* **Language:** C (ANSI Standard)
* **Concepts:** Recursion, Backtracking, Stack Data Structure, Dynamic Memory Allocation (pointers)

## How to Run
Compile the source code:
```bash
gcc main.c -o main
./main