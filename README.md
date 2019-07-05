# Maze-Solver

Description: This program solves a maze that is constructed from an input file of characters. It does so by dynamically allocating
memory for the characters to be read in and then fills the maze with those characters and copies it to a solution maze. 
After, through the use of multiple helper functions and the public function solveMaze(), it computes a way to an exit if there is one. If there
is not one, then it backtracks until it can't. 

How to run the program
1. Open up a terminal and locate the program folder
2. Type the following command: g++ MazeSolver.cpp main.cpp
3. Lastly, run this command: ./a.out

If you want to test the different input files, go into main.cpp and change the string to which ever input file you want.
