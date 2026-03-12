#pragma once

#include <random>
#include <vector>

namespace model {

struct Cell {
    size_t adjacent_mines = 0;
    bool is_opened = false;
    bool is_flagged = false;
    bool has_mine = false;
};

enum class GameResult { Win, Lose, InProgress };

class Model {
public:
    using GameField = std::vector<Cell>;

    Model();

    void SetupGameField(size_t size_x, size_t size_y, size_t mines_num);

    size_t GetSizeX() const;
    size_t GetSizeY() const;

    void OpenCell(size_t x, size_t y);
    void FlagCell(size_t x, size_t y);

    int GetMinesLeft() const;
    size_t GetMinesTotal() const;
    const GameField& GetGameField() const;
    GameResult GetGameResult() const;

    Cell& GetCell(int x, int y);
    const Cell& GetCell(int x, int y) const;

    std::vector<std::pair<size_t, size_t>> GetAdjClosedCells(int x,
                                                             int y) const;

private:
    void PlaceMines();
    void FillCellsWithoutMines();
    size_t CountAdjMines(int x, int y) const;

    void OpenConnectedCells(int x, int y);
    void BFSTraversal(int x, int y);

private:
    std::mt19937_64 gen_;
    GameField game_field_;
    size_t game_field_size_x_;
    size_t game_field_size_y_;
    size_t mines_total_;
    int mines_left_;
    int cells_opened_ = 0;
    bool bomb_opened_ = false;
    bool first_cell_opened = false;
};

}  // namespace model
