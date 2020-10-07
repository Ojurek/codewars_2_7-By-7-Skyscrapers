#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>
#include <array>
#include <set>
#include <chrono>

#define BOARD_SIZE 7

typedef std::vector<int> Field;
typedef std::array<Field, BOARD_SIZE> Row;
typedef std::array<Row, BOARD_SIZE> Board;

static std::vector<std::vector<int>> finalBoard;
static long long duration1 = 0;
static long long duration2 = 0;
static long long duration3 = 0;
static long long duration4 = 0;
static long long duration5 = 0;

enum class Result
{
    Wrong,
    Ok,
    Finish
};

class PuzzleBoard
{
private:
    Board board;
    int totalSizes = BOARD_SIZE * BOARD_SIZE * BOARD_SIZE;
    std::array<int, BOARD_SIZE> rowSizes;
    std::array<int, BOARD_SIZE> columnSizes;
    std::array<int, BOARD_SIZE> upperClue;
    std::array<int, BOARD_SIZE> downClue;
    std::array<int, BOARD_SIZE> leftClue;
    std::array<int, BOARD_SIZE> rightClue;
    void setClueArray(const std::vector<int> &clues);
    Result removeSize(int row, int column, int removeThisSize);
    bool checkAfterReduction(int row, int column);
    bool checkRow(int row);
    bool checkColumn(int column);
    void finish();

public:
    PuzzleBoard(const std::vector<int> &clues);
    Result setSize(int row, int column, int setThisSize);
    int getNumberElementsInField(int row, int column);
    Field getField(int row, int column);
    void printBoard();
};

PuzzleBoard::PuzzleBoard(const std::vector<int> &clues)
{

    Field field;

    finalBoard.clear();

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        field.push_back(i + 1);
    }
    Row row;
    row.fill(field);

    board.fill(row);

    setClueArray(clues);

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        rowSizes[i] = BOARD_SIZE * BOARD_SIZE;
        columnSizes[i] = BOARD_SIZE * BOARD_SIZE;
    }

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        //check upper clue
        if (upperClue[i] == 1)
        {
            setSize(0, i, BOARD_SIZE);
        }
        if (upperClue[i] > 1)
        {
            for (int j = 0; j < (upperClue[i] - 1); j++)
            {
                for (int k = 0; k < (upperClue[i] - j - 1); k++)
                    removeSize(j, i, BOARD_SIZE - k);
            }
        }

        //check left clue
        if (leftClue[i] == 1)
        {
            setSize(i, 0, BOARD_SIZE);
        }
        if (leftClue[i] > 1)
        {
            for (int j = 0; j < (leftClue[i] - 1); j++)
            {
                for (int k = 0; k < (leftClue[i] - j - 1); k++)
                    removeSize(i, j, BOARD_SIZE - k);
            }
        }
        //check right clue
        if (rightClue[i] == 1)
        {
            setSize(i, (BOARD_SIZE - 1), BOARD_SIZE);
        }
        if (rightClue[i] > 1)
        {
            for (int j = 0; j < (rightClue[i] - 1); j++)
            {
                for (int k = 0; k < (rightClue[i] - j - 1); k++)
                    removeSize(i, BOARD_SIZE - j - 1, BOARD_SIZE - k);
            }
        }
        //check down clue
        if (downClue[i] == 1)
        {
            setSize((BOARD_SIZE - 1), i, BOARD_SIZE);
        }
        if (downClue[i] > 1)
        {
            for (int j = 0; j < (downClue[i] - 1); j++)
            {
                for (int k = 0; k < (downClue[i] - j - 1); k++)
                    removeSize(BOARD_SIZE - j - 1, i, BOARD_SIZE - k);
            }
        }
    }
    /*
//check if some number is onle ones in row, not finished
bool founded =false;
std::array<int,BOARD_SIZE+1> element_amount;
element_amount.fill(0);
for (int i =0;i<BOARD_SIZE;i++){
    for (int j=0;j<BOARD_SIZE;j++){
        if (board[i][j].size()>1){
            for (int k=0;k<board[i][j].size();k++){
            element_amount[board[i][j][k]]++;
            }
        }
    }
    for (k)
}
*/
    //  printBoard();
}

void PuzzleBoard::setClueArray(const std::vector<int> &clues)
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        upperClue[i] = clues[i];
        downClue[i] = clues[3 * BOARD_SIZE - 1 - i];
        leftClue[i] = clues[4 * BOARD_SIZE - 1 - i];
        rightClue[i] = clues[i + BOARD_SIZE];
    }
}

Result PuzzleBoard::removeSize(int row, int column, int removeThisSize)
{
    int deletedElements;
    int sizesBeforeErasing = board[row][column].size();
    if (sizesBeforeErasing == 1)
    {
        return Result::Ok;
    }

    board[row][column].erase(std::remove(board[row][column].begin(), board[row][column].end(), removeThisSize), board[row][column].end());

    deletedElements = sizesBeforeErasing - board[row][column].size();

    totalSizes -= deletedElements;
    rowSizes[row] -= deletedElements;
    columnSizes[column] -= deletedElements;
    if ((board[row][column].size() == 1) && (deletedElements > 0))
    {
        if (checkAfterReduction(row, column) == false)
        {
            return Result::Wrong;
        }
        if (totalSizes == (BOARD_SIZE * BOARD_SIZE))
        {
            if (finalBoard.size() != (BOARD_SIZE))
            {
                finish();
            }
            return Result::Finish;
        }
    }
    return Result::Ok;
}

Result PuzzleBoard::setSize(int row, int column, int setThisSize)
{

    int deletedElements = board[row][column].size() - 1;

    board[row][column] = {setThisSize};

    totalSizes -= deletedElements;
    rowSizes[row] -= deletedElements;
    columnSizes[column] -= deletedElements;

    if (checkAfterReduction(row, column) == false)
    {
        return Result::Wrong;
    }

    if (totalSizes == (BOARD_SIZE * BOARD_SIZE))
    {
        return Result::Finish;
    }
    return Result::Ok;
}

bool PuzzleBoard::checkAfterReduction(int row, int column)
{
    Result result;

    if (rowSizes[row] == BOARD_SIZE)
    {
        if (checkRow(row) == false)
        {
            return false;
        }
    }

    if (columnSizes[column] == BOARD_SIZE)
    {
        if (checkColumn(column) == false)
        {
            return false;
        }
    }

    if (board[row][column].size() == 1)
    {
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            result = (removeSize(i, column, board[row][column][0]));
            if (result == Result::Wrong)
            {
                return false;
            }

            if (result == Result::Finish)
            {
                return true;
            }
        }

        for (int i = 0; i < BOARD_SIZE; i++)
        {
            result = removeSize(row, i, board[row][column][0]);
            if (result == Result::Wrong)
            {
                return false;
            }

            if (result == Result::Finish)
            {
                return true;
            }
        }
    }

    return true;
}

bool PuzzleBoard::checkRow(int row)
{
    std::set<int> allSizes;
    int highestSeenBuilding = 0;
    int numberOfSeenBuildings = 0;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        allSizes.insert(board[row][i][0]);
    }
    if (allSizes.size() != BOARD_SIZE)
    {
        return false;
    }
    if (leftClue[row] > 0)
    {
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            if (board[row][i][0] > highestSeenBuilding)
            {
                numberOfSeenBuildings++;
                highestSeenBuilding = board[row][i][0];
            }
        }

        if (numberOfSeenBuildings != leftClue[row])
        {
            return false;
        }
    }

    if (rightClue[row] > 0)
    {
        highestSeenBuilding = 0;
        numberOfSeenBuildings = 0;

        for (int i = (BOARD_SIZE - 1); i >= 0; i--)
        {
            if (board[row][i][0] > highestSeenBuilding)
            {
                numberOfSeenBuildings++;
                highestSeenBuilding = board[row][i][0];
            }
        }

        if (numberOfSeenBuildings != rightClue[row])
        {
            return false;
        }
    }

    return true;
}

bool PuzzleBoard::checkColumn(int column)
{

    std::set<int> allSizes;
    int highestSeenBuilding = 0;
    int numberOfSeenBuildings = 0;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        allSizes.insert(board[i][column][0]);
    }

    if (allSizes.size() != BOARD_SIZE)
    {
        return false;
    }

    if (upperClue[column] > 0)
    {
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            if (board[i][column][0] > highestSeenBuilding)
            {
                numberOfSeenBuildings++;
                highestSeenBuilding = board[i][column][0];
            }
        }

        if (numberOfSeenBuildings != upperClue[column])
        {
            return false;
        }
    }

    if (downClue[column] > 0)
    {
        highestSeenBuilding = 0;
        numberOfSeenBuildings = 0;

        for (int i = (BOARD_SIZE - 1); i >= 0; i--)
        {
            if (board[i][column][0] > highestSeenBuilding)
            {
                numberOfSeenBuildings++;
                highestSeenBuilding = board[i][column][0];
            }
        }

        if (numberOfSeenBuildings != downClue[column])
        {
            return false;
        }
    }

    return true;
}

void PuzzleBoard::finish()
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        std::vector<int> myRow;
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            myRow.push_back(board[i][j][0]);
        }
        finalBoard.push_back(myRow);
        myRow.clear();
    }
}
int PuzzleBoard::getNumberElementsInField(int row, int column)
{
    return board[row][column].size();
}

Field PuzzleBoard::getField(int row, int column)
{
    return board[row][column];
}

void PuzzleBoard::printBoard()
{
    std::cout << std::endl
              << "Board have " << totalSizes << " elements: " << std::endl;
    std::cout << "       ";
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        std::cout << upperClue[i] << "       ";
    }
    std::cout << std::endl
              << "*********************************************************" << std::endl;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        std::cout << leftClue[i] << " * ";
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            for (int k = 0; k < board[i][j].size(); k++)
            {
                std::cout << board[i][j][k];
            }
            for (int k = 0; k < (BOARD_SIZE - board[i][j].size()); k++)
            {
                std::cout << " ";
            }
            std::cout << "  ";
        }
        std::cout << "* " << rightClue[i] << "* "
                  << "elements: " << rowSizes[i] << std::endl;
    }
    std::cout << "*********************************************************" << std::endl;
    std::cout << "       ";
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        std::cout << downClue[i] << "       ";
    }
}

Result reduceElement(PuzzleBoard temp_board)
{
    int sizes_in_field;
    int row;
    int column;
    Field reduced_field;
    Result operation_result;
    PuzzleBoard new_board = temp_board;

    [&] {
        for (int k = 2; k < BOARD_SIZE; k++)
        {
            for (int i = 0; i < BOARD_SIZE; i++)
            {
                for (int j = 0; j < BOARD_SIZE; j++)
                {
                    sizes_in_field = temp_board.getNumberElementsInField(i, j);
                    if (sizes_in_field == k)
                    {
                        row = i;
                        column = j;
                        reduced_field = temp_board.getField(i, j);
                        return;
                    }
                }
            }
        }
    }();

    for (int size : reduced_field)
    {
        new_board = temp_board;
        operation_result = new_board.setSize(row, column, size);

        if (operation_result == Result::Wrong)
        {
            continue;
        }

        if (operation_result == Result::Finish)
        {
            return Result::Finish;
        }

        operation_result == reduceElement(new_board);

        if (operation_result == Result::Finish)
        {
            return Result::Finish;
        }
    }
    return Result::Wrong;
}

std::vector<std::vector<int>> SolvePuzzle(const std::vector<int> &clues)
{
    PuzzleBoard puzzle_board(clues);
    reduceElement(puzzle_board);
    return finalBoard;
}

int main()
{
    static std::vector<std::vector<int>> clues = {
        {7, 0, 0, 0, 2, 2, 3,
         0, 0, 3, 0, 0, 0, 0,
         3, 0, 3, 0, 0, 5, 0,
         0, 0, 0, 0, 5, 0, 4},
        {0, 2, 3, 0, 2, 0, 0,
         5, 0, 4, 5, 0, 4, 0,
         0, 4, 2, 0, 0, 0, 6,
         5, 2, 2, 2, 2, 4, 1}};

    static std::vector<std::vector<std::vector<int>>> expected = {
        {{1, 5, 6, 7, 4, 3, 2},
         {2, 7, 4, 5, 3, 1, 6},
         {3, 4, 5, 6, 7, 2, 1},
         {4, 6, 3, 1, 2, 7, 5},
         {5, 3, 1, 2, 6, 4, 7},
         {6, 2, 7, 3, 1, 5, 4},
         {7, 1, 2, 4, 5, 6, 3}},
        {{7, 6, 2, 1, 5, 4, 3},
         {1, 3, 5, 4, 2, 7, 6},
         {6, 5, 4, 7, 3, 2, 1},
         {5, 1, 7, 6, 4, 3, 2},
         {4, 2, 1, 3, 7, 6, 5},
         {3, 7, 6, 2, 1, 5, 4},
         {2, 4, 3, 5, 6, 1, 7}}};
    auto start = std::chrono::steady_clock::now();
    assert(SolvePuzzle(clues[0]) == expected[0]);
    assert(SolvePuzzle(clues[1]) == expected[1]);
    auto stop = std::chrono::steady_clock::now();

    std::cout << "Duration1 time in milliseconds : "
              << duration1 << " ms" << std::endl;
    std::cout << "Duration2 time in milliseconds : "
              << duration2 << " ms" << std::endl;
    std::cout << "Duration3 time in milliseconds : "
              << duration3 << " ms" << std::endl;
    std::cout << "Duration4 time in milliseconds : "
              << duration4 << " ms" << std::endl;
    std::cout << "Duration5 time in milliseconds : "
              << duration5 << " ms" << std::endl;

    std::cout << "Elapsed time in milliseconds : "
              << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count()
              << " ms" << std::endl;

    std::cout << "All test passed" << std::endl;
}