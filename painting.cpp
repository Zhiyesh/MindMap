#include "painting.h"

Painting::Painting(QWidget *parent) : QLabel(parent)
{

}

void Painting::resizeEvent(QResizeEvent *)
{
    emit sizeChanged();
}

