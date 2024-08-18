#ifndef PAINTING_H
#define PAINTING_H

#include <QLabel>


class Painting : public QLabel
{
    Q_OBJECT

public:
    Painting(QWidget *parent = 0);

protected:
    void resizeEvent(QResizeEvent*);

signals:
    void sizeChanged();
};

#endif // PAINTING_H
