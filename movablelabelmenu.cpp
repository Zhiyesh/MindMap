#include "movablelabelmenu.h"

MovableLabelMenu::MovableLabelMenu(QWidget *parent) : QMenu(parent)
{
}

void MovableLabelMenu::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
    case Qt::Key_Left:
        emit positionPressed(MlMenu::Left);
        break;
    case Qt::Key_Right:
        emit positionPressed(MlMenu::Right);
        break;
    case Qt::Key_Up:
        emit positionPressed(MlMenu::Up);
        break;
    case Qt::Key_Down:
        emit positionPressed(MlMenu::Down);
        break;
    }
}

