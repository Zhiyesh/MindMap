#ifndef MOVABLELABELMENU_H
#define MOVABLELABELMENU_H

#include <QMenu>
#include <QKeyEvent>


namespace MlMenu
{
    enum Position
    {
        Up = 0,
        Right,
        Down,
        Left
    };
}

class MovableLabelMenu : public QMenu
{
    Q_OBJECT
public:
    explicit MovableLabelMenu(QWidget *parent = 0);

signals:
    void positionPressed(int);

protected:
    void keyPressEvent(QKeyEvent* e);
};

#endif // MOVABLELABELMENU_H
