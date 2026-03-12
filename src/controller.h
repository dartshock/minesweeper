#pragma once

#include <qtimer.h>
#include <QString>
#include <QTimer>
#include "model.h"
#include "view.h"

namespace app {

using namespace model;
using namespace view;

enum class CellType { Closed, Empty, Digit, Mine, Flag };

CellType GetCellType(const Cell& cell);

QString CellToQString(const Cell& cell);

QString GetCellColor(const Cell& cell);

QString GetCellStyle(const Cell& cell);

class Controller {
public:
    Controller();

private:
    void LeftPressOnCell(size_t x, size_t y);
    void LeftReleaseOnCell(size_t x, size_t y);
    void RightPressOnCell(size_t x, size_t y);

    void Restart();
    void NewGame(size_t size_x, size_t size_y, size_t mines);

    void UpdateTime();
    void UpdateStatus(const GameResult& result);
    void UpdateCell(size_t x, size_t y);
    void UpdateField();

    void ShowLeftMines();
    void ShowIncorrectFlags();

    void LeftPressOnClosedCell(size_t x, size_t y);
    void LeftPressOnDigitCell(size_t x, size_t y);
    void LeftReleaseOnClosedCell(size_t x, size_t y);
    void LeftReleaseOnDigitCell(size_t x, size_t y);

    template <typename A>
    void ProcessAllAdjacentCells(
        const std::vector<std::pair<size_t, size_t>>& cells, A&& action) {
        for (auto [nx, ny] : cells) {
            if (!model_.GetCell(nx, ny).is_flagged) {
                action(nx, ny);
            }
        }
    }

private:
    Model model_;
    View view_;

    size_t field_size_x_{9};
    size_t field_size_y_{9};
    size_t mines_total_{10};

    bool game_started_ = false;
    size_t seconds_played_ = 0;
};

}  // namespace app
