#ifndef VIEW_H
#define VIEW_H

#include <qtimer.h>
#include <QMainWindow>
#include <QPushButton>

#include <functional>
#include <map>
#include <string_view>
#include "custom_button.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class View;
}  // namespace Ui
QT_END_NAMESPACE

namespace view {
struct Icons {
    static constexpr std::string_view mine = "💣";
    static constexpr std::string_view flag = "🚩";
    static constexpr std::string_view smiley_face = "🙂";
    static constexpr std::string_view happy_face = "😃";
    static constexpr std::string_view sad_face = "😩";
    static constexpr std::string_view surprised_face = "😬";

    static QString GetMine() { return QString::fromUtf8(mine); }
    static QString GetFlag() { return QString::fromUtf8(flag); }
    static QString GetSmileyFace() { return QString::fromUtf8(smiley_face); }
    static QString GetHappyFace() { return QString::fromUtf8(happy_face); }
    static QString GetSadFace() { return QString::fromUtf8(sad_face); }
    static QString GetSurprisedFace() {
        return QString::fromUtf8(surprised_face);
    }
};

struct ButtonStyles {
    static constexpr std::string_view closed_cell =
        "QPushButton {"
        "background-color: #c0c0c0;"
        "border: 5px solid;"
        "border-top-color: #dfdfdf;"
        "border-left-color: #dfdfdf;"
        "border-right-color: #808080;"
        "border-bottom-color: #808080;"
        "font-weight: bold;"
        "font-size: 14px;"
        "}";
    static constexpr std::string_view empty_cell =
        "QPushButton {"
        "background-color: #c0c0c0;"
        "border: 1px solid #808080;"
        "}";
    static constexpr std::string_view mined_cell =
        "QPushButton {"
        "background-color: #ababab;"
        "border: 1px solid #808080;"
        "}";
    static constexpr std::string_view exploded_mine =
        "QPushButton {"
        "background-color: #e85151;"
        "border: 1px solid #808080;"
        "}";
    static constexpr std::string_view incorrect_flagged_cell =
        "QPushButton {"
        "background-color: #e85151;"
        "border: 5px solid;"
        "border-top-color: #dfdfdf;"
        "border-left-color: #dfdfdf;"
        "border-right-color: #808080;"
        "border-bottom-color: #808080;"
        "font-weight: bold;"
        "font-size: 14px;"
        "}";
    static constexpr std::string_view digit_cell =
        "QPushButton {"
        "background-color: #c0c0c0;"
        "border: 1px solid #808080;"
        "font-weight: bold;"
        "font-size: 20px;"
        "}";
    static constexpr std::string_view status =
        "QPushButton {"
        "background-color: #c0c0c0;"
        "border: 5px solid;"
        "border-top-color: #dfdfdf;"
        "border-left-color: #dfdfdf;"
        "border-right-color: #808080;"
        "border-bottom-color: #808080;"
        "font-size: 20px;"
        "}";
    static constexpr std::string_view pressed_status =
        "QPushButton {"
        "background-color: #c0c0c0;"
        "border: 1px solid #808080;"
        "font-size: 20px;"
        "}";
    static QString GetClosedCell() {
        return QString::fromStdString(std::string(closed_cell));
    }
    static QString GetEmptyCell() {
        return QString::fromStdString(std::string(empty_cell));
    }
    static QString GetMinedCell() {
        return QString::fromStdString(std::string(mined_cell));
    }
    static QString GetExplodedMine() {
        return QString::fromStdString(std::string(exploded_mine));
    }
    static QString GetIncorrectFlaggedCell() {
        return QString::fromStdString(std::string(incorrect_flagged_cell));
    }
    static QString GetDigitCell() {
        return QString::fromStdString(std::string(digit_cell));
    }
    static QString GetStatusButton() {
        return QString::fromStdString(std::string(status));
    }
    static QString GetPressedStatusButton() {
        return QString::fromStdString(std::string(pressed_status));
    }
};

}  // namespace view

class View : public QMainWindow {
    Q_OBJECT

public:
    View(QWidget* parent = nullptr);
    ~View();

    void SetupField(size_t rows, size_t columns);

    void UpdateCellText(size_t x, size_t y, const QString& value);
    void UpdateCellStyle(size_t x, size_t y, const QString& style);
    void UpdateCellTextColor(size_t x, size_t y, const QString& color);

    void BlockField(bool block);

    void SetStatus(const QString& new_status);
    void SetStatusStyle(const QString& style);
    void SetMinesNum(int new_num);
    void SetTime(int new_time);

    void SetLeftPressCallback(
        const std::function<void(size_t x, size_t y)>& callback);
    void SetLeftReleaseCallback(
        const std::function<void(size_t x, size_t y)>& callback);
    void SetRightPressCallback(
        const std::function<void(size_t x, size_t y)>& callback);
    void SetNewGameCallback(
        const std::function<void(size_t size_x, size_t size_y, size_t mines)>&
            callback);
    void SetRestartCallback(const std::function<void()>& callback);
    void SetTimerCallback(const std::function<void()>& callback);
    void StartTimer();
    void StopTimer();

private slots:
    void on_action_new_game_triggered();
    void on_action_easy_triggered();
    void on_action_medium_triggered();
    void on_action_hard_triggered();

private:
    void SetupWindowStyle();
    void SetupStatus();
    void SetupMinesCounter();
    void SetupTimer();

private:
    Ui::View* ui;

    std::map<std::pair<size_t, size_t>, CustomButton> cells_;
    std::function<void(size_t x, size_t y)> left_press_callback_;
    std::function<void(size_t x, size_t y)> left_release_callback_;
    std::function<void(size_t x, size_t y)> right_press_callback_;
    std::function<void(size_t size_x, size_t size_y, size_t mines)>
        newgame_callback_;
    std::function<void()> restart_callback_;
    std::function<void()> timer_callback_;

    static constexpr QSize BUTTON_SIZE{30, 30};
    static constexpr QSize STATUS_SIZE{40, 40};
    static constexpr QSize NUM_DISPLAY_SIZE{60, 40};

    QTimer timer_{this};
};

#endif  // VIEW_H
