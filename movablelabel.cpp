#include "movablelabel.h"

const QString LabelRes[] = {
    ":/drawable/res/Bracket.png",
    ":/drawable/res/HLine.png"
};

MovableLabel::MovableLabel(const int &w, const int &h, const Ml::LabelType &type, const QString &text, const QString &text_size, QWidget *parent) : QLabel(parent)
{
    this->setFixedSize(w, h);
    this->setText(text);
    this->labelType = type;

    if (this->labelType) this->setStyleSheet(QString("image: url(%1);").arg(LabelRes[this->labelType - 1]));
    else
    {
        this->is_font = true;
        if (!text_size.isEmpty()) this->setStyleSheet(QString("font: %1pt \"黑体\";").arg(text_size));
        else this->setStyleSheet("font: 13pt \"黑体\";");
    }
}

Ml::LabelType MovableLabel::type() const
{
    return this->labelType;
}

bool MovableLabel::isFont() const
{
    return this->is_font;
}

void MovableLabel::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
    {
        emit click();
    }
    else if (e->button() == Qt::RightButton)
    {
        emit clickRight();
    }
}

void MovableLabel::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        emit doubleClick();
    }
}

