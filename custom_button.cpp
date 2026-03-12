#include "custom_button.h"

CustomButton::CustomButton(QWidget* parent)
    : QPushButton(parent) {}

CustomButton::CustomButton(const QString& text, QWidget* parent)
    : QPushButton(text, parent) {}

void CustomButton::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit leftPressed();
    } else if (event->button() == Qt::RightButton) {
        emit rightPressed();
    }
}

void CustomButton::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit leftReleased();
    }
}
