#ifndef CUSTOM_BUTTON_H
#define CUSTOM_BUTTON_H

#include <QMouseEvent>
#include <QPushButton>

class CustomButton : public QPushButton {
    Q_OBJECT

public:
    explicit CustomButton(QWidget* parent = nullptr);
    explicit CustomButton(const QString& text, QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

signals:
    void leftPressed();
    void leftReleased();
    void rightPressed();
};

#endif  // CUSTOM_BUTTON_H
