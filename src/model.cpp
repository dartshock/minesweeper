#include "model.h"

#include <deque>
#include <unordered_set>

namespace model {

Model::Model()
    : gen_(std::random_device{}()) {
    SetupGameField(9, 9, 10);
}

void Model::SetupGameField(size_t size_x, size_t size_y, size_t mines_num) {
    game_field_.assign(size_x * size_y, Cell{});
    game_field_size_x_ = size_x;
    game_field_size_y_ = size_y;
    mines_total_ = mines_num;
    mines_left_ = static_cast<int>(mines_num);
    bomb_opened_ = false;
    first_cell_opened = false;
    cells_opened_ = 0;
}

size_t Model::GetSizeX() const {
    return game_field_size_x_;
}

size_t Model::GetSizeY() const {
    return game_field_size_y_;
}

void Model::OpenCell(size_t x, size_t y) {
    Cell& cell = GetCell(x, y);
    if (cell.is_opened) {
        return;
    }
    if (!first_cell_opened) {
        first_cell_opened = true;
        cell.is_opened = true;
        ++cells_opened_;
        PlaceMines();
        FillCellsWithoutMines();
        OpenConnectedCells(x, y);
        return;
    }

    cell.is_opened = true;
    ++cells_opened_;
    if (cell.has_mine) {
        bomb_opened_ = true;
    } else {
        OpenConnectedCells(x, y);
    }
}

void Model::FlagCell(size_t x, size_t y) {
    Cell& cell = GetCell(x, y);
    if (cell.is_opened) {
        return;
    }
    if (cell.is_flagged) {
        cell.is_flagged = false;
        ++mines_left_;
    } else {
        cell.is_flagged = true;
        --mines_left_;
    }
}

int Model::GetMinesLeft() const {
    return mines_left_;
}

size_t Model::GetMinesTotal() const {
    return mines_total_;
}

const Model::GameField& Model::GetGameField() const {
    return game_field_;
}

GameResult Model::GetGameResult() const {
    if (bomb_opened_) {
        return GameResult::Lose;
    }
    if (cells_opened_ == (game_field_.size() - mines_total_)) {
        return GameResult::Win;
    }
    return GameResult::InProgress;
}

Cell& Model::GetCell(int x, int y) {
    return game_field_.at(x + game_field_size_x_ * y);
}

const Cell& Model::GetCell(int x, int y) const {
    return const_cast<Model*>(this)->GetCell(x, y);
}

std::vector<std::pair<size_t, size_t>> Model::GetAdjClosedCells(int x,
                                                                int y) const {
    std::vector<std::pair<size_t, size_t>> result;
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) {
                continue;
            }
            int neighbor_x = x + dx;
            int neighbor_y = y + dy;
            if (neighbor_x >= 0 &&
                neighbor_x < static_cast<int>(game_field_size_x_) &&
                neighbor_y >= 0 &&
                neighbor_y < static_cast<int>(game_field_size_y_)) {
                auto cell = GetCell(neighbor_x, neighbor_y);
                if (!cell.is_opened) {
                    result.emplace_back(
                        std::pair{static_cast<size_t>(neighbor_x),
                                  static_cast<size_t>(neighbor_y)});
                }
            }
        }
    }
    return result;
}

void Model::PlaceMines() {
    std::uniform_int_distribution<int> dist(
        0, game_field_size_x_ * game_field_size_y_ - 1);

    for (size_t i = 0; i < mines_total_;) {
        auto& cell = game_field_[dist(gen_)];
        if (cell.has_mine || cell.is_opened) {
            continue;
        }
        cell.has_mine = true;
        ++i;
    }
}

void Model::FillCellsWithoutMines() {
    for (int y = 0; y < game_field_size_y_; ++y) {
        for (int x = 0; x < game_field_size_x_; ++x) {
            Cell& cell = GetCell(x, y);
            cell.adjacent_mines = CountAdjMines(x, y);
        }
    }
}

size_t Model::CountAdjMines(int x, int y) const {
    size_t count = 0;

    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) {
                continue;
            }
            size_t neighbor_x = x + dx;
            size_t neighbor_y = y + dy;
            if (neighbor_x >= 0 && neighbor_x < game_field_size_x_ &&
                neighbor_y >= 0 && neighbor_y < game_field_size_y_) {
                const Cell& neighbor_cell = GetCell(neighbor_x, neighbor_y);
                if (neighbor_cell.has_mine) {
                    count++;
                }
            }
        }
    }
    return count;
}

void Model::OpenConnectedCells(int x, int y) {
    Cell& cell = GetCell(x, y);

    if (cell.has_mine || cell.adjacent_mines != 0) {
        return;
    }

    BFSTraversal(x, y);
}

struct PairHasher {
    std::size_t operator()(const std::pair<std::size_t, std::size_t>& p) const {
        std::size_t h1 = std::hash<std::size_t>{}(p.first);
        std::size_t h2 = std::hash<std::size_t>{}(p.second);
        return h1 ^ (h2 << 1);
    }
};

void Model::BFSTraversal(int x, int y) {
    auto connected_closed_cells = GetAdjClosedCells(x, y);

    std::unordered_set<std::pair<size_t, size_t>, PairHasher> visited_cells(
        connected_closed_cells.begin(), connected_closed_cells.end());
    std::deque<std::pair<size_t, size_t>> cells_to_open(
        connected_closed_cells.begin(), connected_closed_cells.end());

    while (!cells_to_open.empty()) {
        auto [cell_to_open_x, cell_to_open_y] = cells_to_open.front();
        cells_to_open.pop_front();
        auto& cell_to_open = GetCell(cell_to_open_x, cell_to_open_y);

        cell_to_open.is_opened = true;
        ++cells_opened_;

        if (cell_to_open.adjacent_mines != 0) {
            continue;
        }

        auto connected_cells_to_open =
            GetAdjClosedCells(cell_to_open_x, cell_to_open_y);

        for (auto connected_cell_to_open : connected_cells_to_open) {
            auto iter = visited_cells.find(connected_cell_to_open);
            if (iter == visited_cells.end()) {
                cells_to_open.push_back(connected_cell_to_open);
                visited_cells.insert(connected_cell_to_open);
            }
        }
    }
}

}  // namespace model
