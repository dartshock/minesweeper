#include "controller.h"
#include <algorithm>
#include <unordered_map>

namespace app {

namespace ph = std::placeholders;

CellType GetCellType(const Cell& cell) {
    if (cell.is_opened) {
        if (cell.has_mine) {
            return CellType::Mine;
        }
        if (cell.adjacent_mines != 0) {
            return CellType::Digit;
        }
        return CellType::Empty;
    } else if (cell.is_flagged) {
        return CellType::Flag;
    }
    return CellType::Closed;
}

QString CellToQString(const Cell& cell) {
    if (cell.is_opened) {
        if (cell.has_mine) {
            return Icons::GetMine();
        }
        if (cell.adjacent_mines != 0) {
            return QChar(static_cast<char>(cell.adjacent_mines + '0'));
        }
    } else if (cell.is_flagged) {
        return Icons::GetFlag();
    }
    return {};
}

QString GetCellColor(const Cell& cell) {
    if (!cell.is_opened || cell.has_mine || cell.adjacent_mines == 0) {
        return {};
    }
    std::unordered_map<int, std::string> adj_mines_to_color = {
        {1, "#0000FF"}, {2, "#008000"}, {3, "#FF0000"}, {4, "#000080"},
        {5, "#800000"}, {6, "#008080"}, {7, "#000000"}, {8, "#808080"}};

    return QString::fromStdString(adj_mines_to_color.at(cell.adjacent_mines));
}

QString GetCellStyle(const Cell& cell) {
    if (!cell.is_opened) {
        return ButtonStyles::GetClosedCell();
    }
    if (cell.has_mine) {
        if (cell.is_opened) {
            return ButtonStyles::GetExplodedMine();
        }
        return ButtonStyles::GetMinedCell();
    }
    return ButtonStyles::GetDigitCell();
}

Controller::Controller() {
    view_.SetNewGameCallback(
        std::bind(&Controller::NewGame, this, ph::_1, ph::_2, ph::_3));
    view_.SetRestartCallback(std::bind(&Controller::Restart, this));
    view_.SetTimerCallback(std::bind(&Controller::UpdateTime, this));
    view_.SetLeftPressCallback(
        std::bind(&Controller::LeftPressOnCell, this, ph::_1, ph::_2));
    view_.SetLeftReleaseCallback(
        std::bind(&Controller::LeftReleaseOnCell, this, ph::_1, ph::_2));
    view_.SetRightPressCallback(
        std::bind(&Controller::RightPressOnCell, this, ph::_1, ph::_2));

    NewGame(field_size_x_, field_size_y_, mines_total_);
    view_.show();
}

void Controller::LeftPressOnCell(size_t x, size_t y) {
    if (!game_started_) {
        game_started_ = true;
        view_.StartTimer();
    }
    const Cell& cell = model_.GetCell(x, y);
    auto type = GetCellType(cell);

    if (type == CellType::Closed) {
        LeftPressOnClosedCell(x, y);
    } else if (type == CellType::Digit) {
        LeftPressOnDigitCell(x, y);
    }
}

void Controller::LeftReleaseOnCell(size_t x, size_t y) {
    const Cell& cell = model_.GetCell(x, y);
    auto type = GetCellType(cell);

    if (type == CellType::Closed) {
        LeftReleaseOnClosedCell(x, y);
    } else if (type == CellType::Digit) {
        LeftReleaseOnDigitCell(x, y);
    }
}

void Controller::RightPressOnCell(size_t x, size_t y) {
    if (!game_started_) {
        game_started_ = true;
        view_.StartTimer();
    }
    const Cell& cell = model_.GetCell(x, y);
    if (cell.is_opened) {
        return;
    }

    model_.FlagCell(x, y);
    view_.UpdateCellText(x, y, CellToQString(cell));
    view_.SetMinesNum(model_.GetMinesLeft());
}

void Controller::Restart() {
    model_.SetupGameField(field_size_x_, field_size_y_, mines_total_);
    view_.BlockField(false);
    view_.SetStatus(Icons::GetSmileyFace());
    view_.SetMinesNum(model_.GetMinesLeft());

    game_started_ = false;
    seconds_played_ = 0;
    view_.StopTimer();
    view_.SetTime(0);

    UpdateField();
}

void Controller::NewGame(size_t size_x, size_t size_y, size_t mines) {
    field_size_x_ = size_x;
    field_size_y_ = size_y;
    mines_total_ = mines;
    view_.SetupField(size_x, size_y);
    Restart();
}

void Controller::UpdateTime() {
    if (seconds_played_ >= 999) {
        return;
    }
    ++seconds_played_;
    view_.SetTime(seconds_played_);
}

void Controller::UpdateStatus(const GameResult& result) {
    switch (result) {
        case GameResult::Win:
            view_.SetStatus(Icons::GetHappyFace());
            break;
        case GameResult::Lose:
            view_.SetStatus(Icons::GetSadFace());
            break;
        case GameResult::InProgress:
            view_.SetStatus(Icons::GetSmileyFace());
            break;
    }
}

void Controller::UpdateCell(size_t x, size_t y) {
    auto field = model_.GetGameField();
    auto cell = field[x + y * field_size_x_];
    view_.UpdateCellText(x, y, CellToQString(cell));
    view_.UpdateCellStyle(x, y, GetCellStyle(cell));
    view_.UpdateCellTextColor(x, y, GetCellColor(cell));
}

void Controller::UpdateField() {
    for (size_t y = 0; y < field_size_y_; ++y) {
        for (size_t x = 0; x < field_size_x_; ++x) {
            UpdateCell(x, y);
        }
    }
}

void Controller::ShowLeftMines() {
    for (size_t y = 0; y < field_size_y_; ++y) {
        for (size_t x = 0; x < field_size_x_; ++x) {
            auto& cell = model_.GetCell(x, y);
            if (cell.has_mine && !cell.is_opened && !cell.is_flagged) {
                view_.UpdateCellText(x, y, Icons::GetMine());
                view_.UpdateCellStyle(x, y, ButtonStyles::GetMinedCell());
            }
        }
    }
}

void Controller::ShowIncorrectFlags() {
    for (size_t y = 0; y < field_size_y_; ++y) {
        for (size_t x = 0; x < field_size_x_; ++x) {
            auto& cell = model_.GetCell(x, y);
            if (cell.is_flagged && !cell.has_mine) {
                view_.UpdateCellStyle(x, y,
                                      ButtonStyles::GetIncorrectFlaggedCell());
            }
        }
    }
}

void Controller::LeftPressOnClosedCell(size_t x, size_t y) {
    view_.UpdateCellStyle(x, y, ButtonStyles::GetEmptyCell());
    view_.SetStatus(Icons::GetSurprisedFace());
}

void Controller::LeftPressOnDigitCell(size_t x, size_t y) {
    const Cell& cell = model_.GetCell(x, y);
    auto adj_closed_cells =
        model_.GetAdjClosedCells(static_cast<int>(x), static_cast<int>(y));
    if (adj_closed_cells.empty()) {
        return;
    }

    int flagged_cells = std::count_if(
        adj_closed_cells.begin(), adj_closed_cells.end(),
        [&](const std::pair<int, int>& cell) {
            return model_.GetCell(cell.first, cell.second).is_flagged;
        });

    if (cell.adjacent_mines == adj_closed_cells.size()) {
        ProcessAllAdjacentCells(adj_closed_cells, [this](int nx, int ny) {
            RightPressOnCell(nx, ny);
        });
    } else if (cell.adjacent_mines == flagged_cells) {
        ProcessAllAdjacentCells(adj_closed_cells, [this](int nx, int ny) {
            LeftPressOnClosedCell(nx, ny);
            LeftReleaseOnClosedCell(nx, ny);
        });
    } else {
        ProcessAllAdjacentCells(adj_closed_cells, [this](int nx, int ny) {
            view_.UpdateCellStyle(nx, ny, ButtonStyles::GetEmptyCell());
        });
    }
}

void Controller::LeftReleaseOnClosedCell(size_t x, size_t y) {
    model_.OpenCell(x, y);
    auto type = GetCellType(model_.GetCell(x, y));

    if (type == CellType::Mine || type == CellType::Digit) {
        UpdateCell(x, y);
    } else {
        UpdateField();
    }

    auto result = model_.GetGameResult();
    UpdateStatus(result);
    if (result == GameResult::InProgress) {
        return;
    }
    view_.BlockField(true);
    view_.StopTimer();
    if (result == GameResult::Lose) {
        ShowLeftMines();
        ShowIncorrectFlags();
    }
}

void Controller::LeftReleaseOnDigitCell(size_t x, size_t y) {
    auto adj_closed_cells =
        model_.GetAdjClosedCells(static_cast<int>(x), static_cast<int>(y));
    if (adj_closed_cells.empty()) {
        return;
    }

    for (auto [neighbor_x, neighbor_y] : adj_closed_cells) {
        UpdateCell(neighbor_x, neighbor_y);
    }
}

}  // namespace app
