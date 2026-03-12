#include "view.h"
#include "custom_button.h"
#include "ui_view.h"

View::View(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::View) {
    ui->setupUi(this);

    this->setWindowTitle("Minesweeper");

    SetupWindowStyle();
    SetupStatus();
    SetupMinesCounter();
    SetupTimer();
}

View::~View() {
    delete ui;
}

void View::SetupField(size_t rows, size_t columns) {
    cells_.clear();
    ui->field->setSpacing(0);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < columns; ++j) {
            cells_.emplace(std::pair{i, j}, " ");
            cells_[{i, j}].setStyleSheet(view::ButtonStyles::GetClosedCell());
            cells_[{i, j}].setFixedSize(View::BUTTON_SIZE);
            ui->field->addWidget(&cells_[{i, j}], i, j);
            connect(&cells_[{i, j}], &CustomButton::leftPressed,
                    &cells_[{i, j}], [this, i, j]() {
                        if (left_press_callback_) {
                            left_press_callback_(i, j);
                        }
                    });
            connect(&cells_[{i, j}], &CustomButton::leftReleased,
                    &cells_[{i, j}], [this, i, j]() {
                        if (left_release_callback_) {
                            left_release_callback_(i, j);
                        }
                    });
            connect(&cells_[{i, j}], &CustomButton::rightPressed,
                    &cells_[{i, j}], [this, i, j]() {
                        if (right_press_callback_) {
                            right_press_callback_(i, j);
                        }
                    });
        }
    }
    int field_width = columns * View::BUTTON_SIZE.width() +
                      (columns - 1) * ui->field->spacing();
    int field_height =
        rows * View::BUTTON_SIZE.height() + (rows - 1) * ui->field->spacing();
    ui->field_widget->setFixedSize(field_width, field_height);
    int upper_height = View::STATUS_SIZE.height() + 10;
    ui->upper_widget->setFixedSize(field_width, upper_height);

    ui->centralwidget->adjustSize();
    this->setFixedSize(this->sizeHint());
}

void View::UpdateCellText(size_t x, size_t y, const QString& value) {
    cells_[{x, y}].setText(value);
}

void View::UpdateCellStyle(size_t x, size_t y, const QString& style) {
    cells_[{x, y}].setStyleSheet(style);
}

void View::UpdateCellTextColor(size_t x, size_t y, const QString& color) {
    QPalette palette = cells_[{x, y}].palette();
    palette.setColor(QPalette::ButtonText, color);
    cells_[{x, y}].setPalette(palette);
}

void View::BlockField(bool block) {
    ui->field_widget->setEnabled(!block);
}

void View::SetStatus(const QString& new_status) {
    ui->pb_new_game->setText(new_status);
}

void View::SetStatusStyle(const QString& style) {
    ui->pb_new_game->setStyleSheet(style);
}

void View::SetMinesNum(int new_num) {
    ui->lcdN_mines->display(new_num);
}

void View::SetTime(int new_time) {
    ui->lcdN_timer->display(new_time);
}

void View::SetLeftPressCallback(
    const std::function<void(size_t x, size_t y)>& callback) {
    left_press_callback_ = callback;
}

void View::SetLeftReleaseCallback(
    const std::function<void(size_t x, size_t y)>& callback) {
    left_release_callback_ = callback;
}

void View::SetRightPressCallback(
    const std::function<void(size_t x, size_t y)>& callback) {
    right_press_callback_ = callback;
}

void View::SetNewGameCallback(
    const std::function<void(size_t size_x, size_t size_y, size_t mines)>&
        callback) {
    newgame_callback_ = callback;
}

void View::SetRestartCallback(const std::function<void()>& callback) {
    restart_callback_ = callback;
}

void View::SetTimerCallback(const std::function<void()>& callback) {
    timer_callback_ = callback;
}

void View::StartTimer() {
    timer_.start();
}

void View::StopTimer() {
    timer_.stop();
}

void View::SetupWindowStyle() {
    this->setStyleSheet(
        "QMainWindow  {"
        "background-color: #ababab;"
        "border: 5px solid;"
        "border-top-color: #dfdfdf;"
        "border-left-color: #dfdfdf;"
        "border-right-color: #808080;"
        "border-bottom-color: #808080;"
        "}");
    ui->field_widget->setStyleSheet(
        "QWidget {"
        "background-color: #c0c0c0;"
        "border: 5px solid;"
        "border-top-color: #808080;"
        "border-left-color: #808080;"
        "border-right-color: #dfdfdf;"
        "border-bottom-color: #dfdfdf ;"
        "}");
    ui->upper_widget->setStyleSheet(
        "QWidget {"
        "background-color: #c0c0c0;"
        "border: 5px solid;"
        "border-top-color: #808080;"
        "border-left-color: #808080;"
        "border-right-color: #dfdfdf;"
        "border-bottom-color: #dfdfdf ;"
        "}");
}

void View::SetupStatus() {
    ui->pb_new_game->setFixedSize(View::STATUS_SIZE);
    SetStatusStyle(view::ButtonStyles::GetStatusButton());

    connect(ui->pb_new_game, &QPushButton::pressed, ui->pb_new_game, [this]() {
        SetStatusStyle(view::ButtonStyles::GetPressedStatusButton());
    });
    connect(ui->pb_new_game, &QPushButton::released, ui->pb_new_game, [this]() {
        SetStatusStyle(view::ButtonStyles::GetStatusButton());
        if (restart_callback_) {
            restart_callback_();
        }
    });
}

void View::SetupMinesCounter() {
    ui->lcdN_mines->setFixedSize(View::NUM_DISPLAY_SIZE);
    ui->lcdN_mines->setDigitCount(3);
    ui->lcdN_mines->setStyleSheet(
        "QLCDNumber {"
        "background-color: #000000;"
        "color: #FF0000;"
        "border: 1px solid #808080;"
        "border-radius: 4px;"
        "padding: 2px;"
        "}");
}

void View::SetupTimer() {
    ui->lcdN_timer->setFixedSize(View::NUM_DISPLAY_SIZE);
    ui->lcdN_timer->setDigitCount(3);
    ui->lcdN_timer->setStyleSheet(
        "QLCDNumber {"
        "background-color: #000000;"
        "color: #FF0000;"
        "border: 1px solid #808080;"
        "border-radius: 4px;"
        "padding: 2px;"
        "}");

    timer_.setInterval(1000);
    connect(&timer_, &QTimer::timeout, this, [this]() {
        if (timer_callback_) {
            timer_callback_();
        }
    });
}

void View::on_action_new_game_triggered() {
    if (restart_callback_) {
        restart_callback_();
    }
}

void View::on_action_easy_triggered() {
    if (newgame_callback_) {
        newgame_callback_(9, 9, 10);
    }
    ui->action_medium->setChecked(false);
    ui->action_hard->setChecked(false);
}

void View::on_action_medium_triggered() {
    if (newgame_callback_) {
        newgame_callback_(16, 16, 40);
    }
    ui->action_easy->setChecked(false);
    ui->action_hard->setChecked(false);
}

void View::on_action_hard_triggered() {
    if (newgame_callback_) {
        newgame_callback_(16, 30, 99);
    }
    ui->action_easy->setChecked(false);
    ui->action_medium->setChecked(false);
}
