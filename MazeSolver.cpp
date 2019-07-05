#include "MazeSolver.h"
#include <fstream>

// constructor that takes in an input file, which will be a series of characters
// An ifstream object is created to read in the characters to begin assembling the maze
// Once the rows and columns are read in, it initializes the maze with the necessary memory to hold the characters, all of which is dynamically allocated
// It then fills the maze with the characters and copies it over to the solution maze and sets the maze_ready flag to true 
MazeSolver::MazeSolver(std::string input_file)
{
    std::ifstream maze_stream;
    maze_stream.open(input_file);

    if ( maze_stream.fail() )
    {
        std::cout << "Cannot read from input_file" << std::endl;
    }
    else
    {
        maze_stream >> maze_rows_ >> maze_columns_;
        initializeMaze(maze_rows_, maze_columns_);
        fillMaze(maze_stream); 
        initializeSolution();
        maze_ready = true;
    }
    maze_stream.close();
}
//destructor that deletes maze_ and solution_ completely by iterating over the arrays within the pointer array to ensure no memory leaks 
MazeSolver::~MazeSolver()
{
    if ( maze_ == nullptr || solution_ == nullptr )
    {
        std::cout << "Maze or solution matrix not allocated " << std::endl;
    }
    else
    {
        for ( int i = 0; i < maze_rows_; i++ )
        {
            delete[] maze_[i];
            delete[] solution_[i];
        }
        delete[] maze_;
        delete[] solution_;
    }
}

//This function initializes the maze_ by dynamically allocating memory, which is called in the constructor
void MazeSolver::initializeMaze(int rows, int columns)
{
    maze_ = new char*[rows]; 
    for ( int i = 0; i < rows; i++ )
    {
        maze_[i] = new char[columns]; // Where the actual characters will be stored
    }
}
// This function, which is called by the constructor, fills the maze with the input characters by reading them in from the ifstream object
void MazeSolver::fillMaze(std::ifstream& input_stream)
{
    for ( int i = 0; i < maze_rows_; i++ ) 
    {
        for ( int j = 0; j < maze_columns_; j++ )
        {
            input_stream >> maze_[i][j];
        }
    }
}
// This function first calls upon another function, copyMazetoSolution(), which copies the contents of maze_ into solution_ 
// and then initializes the stack being used in the program by pushing on to it a starting position
void MazeSolver::initializeSolution()
{
    copyMazetoSolution();

    Position starting_position;
    starting_position.row = 0, starting_position.column = 0;

    backtrack_stack_.push(starting_position);
}
// Copies maze_ contents to solution_
void MazeSolver::copyMazetoSolution()
{
    solution_ = new char*[maze_rows_]; // Initializes solution matrix by allocating necessary memory
    for ( int i = 0; i < maze_rows_; i++ )
    {
        solution_[i] = new char[maze_columns_];
    }
    for ( int i = 0; i < maze_rows_; i++ )
    {
        for ( int j = 0; j < maze_columns_; j++ )
        {
            solution_[i][j] = maze_[i][j];
        }
    }
}
// Used in the main function and lets the program know the constructor is done setting up maze_ and solution_ so
// the function can being solving the maze 
bool MazeSolver::mazeIsReady()
{
    if ( maze_ready )
    {
        return true;
    }
    else
    {
        return false;
    }
}

// This function, which is called upon in the main function, solves the maze with the help of extendPath(). While the stack is not empty, 
// the current position is set to the top of the stack to ensure the newest move available. It first checks if the current position is the 
// exit, and if not, it goes on to an else if where extendPath() is called upon to check if the current position can in fact be extended. 
// If it can, you set the current position to a ‘>’ to show the path and then sets the position to the top of the stack. If none of these are satisfied, 
// then the function backtracks and pops that position off the stack and sets the current position to the old position at the top of the stack. 
// The while loop then continues until it finds a solution, or does not. 
bool MazeSolver::solveMaze()
{
    Position current_position;

    while ( !backtrack_stack_.empty() )
    {
        current_position = backtrack_stack_.top();
        if ( solution_[current_position.row][current_position.column] == '$')
        {
            std::cout << "Found the exit!!!" << std::endl;
            return true;
        }
        else if ( extendPath(current_position) )
        {
            solution_[current_position.row][current_position.column] = '>';
            current_position = backtrack_stack_.top();
        }
        else
        {
            maze_[current_position.row][current_position.column] = 'X';
            solution_[current_position.row][current_position.column] = '@';
            backtrack_stack_.pop();
            if ( !backtrack_stack_.empty() )
            {
                current_position = backtrack_stack_.top();
            }
            else
            {
                std::cout << "This maze has no solution." << std::endl;
                return false;
            }
        }
    }
    return false;
}

// This function first checks if the current position is extensible, so it calls on isExtensible with 
// the position and direction it wants to go in. If the move is valid, it then pushes that position on to the stack. 
// Possible for multiple positions to be put on to the stack if both directions are free.
bool MazeSolver::extendPath(Position current_position)
{
    bool extended = false;
    if ( isExtensible(current_position, SOUTH) )
    {
        backtrack_stack_.push(getNewPosition(current_position, SOUTH));
        extended = true;
    }
    if ( isExtensible(current_position, EAST) )
    {
        backtrack_stack_.push(getNewPosition(current_position, EAST));
        extended = true;
    }
    return extended;  
}

// Called by extendPath() and acquires a new position based off the direction and position in which it wants to travel from. 
Position MazeSolver::getNewPosition(Position old_position, direction dir)
{
    Position new_position = old_position;
    if ( dir == SOUTH )
    {
        new_position.row = old_position.row + 1;
        return new_position;
    }
    else if ( dir == EAST )
    {
        new_position.column = old_position.column + 1;
        return new_position;
    }
    else
    {
        std::cout << "New position could not be acquired" << std::endl;
    }
    return old_position;
}

// This function, called by extendPath(), takes in a position and direction and computes if the 
// direction the program wants to move to from a current_position is valid. If it is, the function returns true. False otherwise.
bool MazeSolver::isExtensible(Position current_position, direction dir)
{
    bool extensible = false;
    int current_row = current_position.row;
    int current_column = current_position.column;

    if ( dir == SOUTH )
    {   
        int updated_row = current_row + 1;
        if ( current_row < maze_rows_ - 1 )
        {
            if ( (solution_[updated_row][current_column] == '_') || (solution_[updated_row][current_column] == '$') )
            {
                extensible = true;
            } 
        }
        else
        {
            return false;
        }
    }
    else if ( dir == EAST )
    {
        int updated_column = current_column + 1;
        if ( current_column < maze_rows_ - 1 )
        {
            if ( (solution_[current_row][updated_column] == '_') || (solution_[current_row][updated_column] == '$') )
            {
                extensible = true;
            } 
        }
        else
        {
            return false;
        }
    }
    else
    {
        std::cout << "Not a valid direction" << std::endl;
    }
    return extensible;
}

// Outputs the solution to the input once the solveMaze function is finished
void MazeSolver::printSolution()
{
    std::cout << "The solution to this maze is: " << std::endl;
    for ( int i = 0; i < maze_rows_; i++ )
    {
        for ( int j = 0; j < maze_columns_; j++ )
        {
            std::cout << solution_[i][j] << " ";
        }
        std::cout << std::endl;
    }
}
