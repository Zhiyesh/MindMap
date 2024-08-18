#include "movablelabel.h"

const QString LabelRes[] = {
    ":/drawable/res/Bracket.png",
    ":/drawable/res/HLine.png",
    ":/drawable/res/VLine.png"
};

MovableLabel::MovableLabel(
    const int &w,
    const int &h,
    const Ml::LabelType &type,
    const QString &text,
    const int &text_size,
    QWidget *parent) : QLabel(parent)
{
    setFixedSize(w, h);
    setText(text);
    labelType = type;

    if (labelType) setStyleSheet(QString("image: url(%1);").arg(LabelRes[labelType - 1]));
    else
    {
        is_font = true;
        setStyleSheet(
            QString("font: %1pt \"黑体\";")
                    .arg(QString::number(this->text_size = (text_size ? text_size : (13))))
        );
    }

    setStyleSheet(
        QString(styleSheet()).append("background-color: rgb(0, 0, 0, 0);")
    );
}

Ml::LabelType MovableLabel::type() const
{
    return labelType;
}

bool MovableLabel::isFont() const
{
    return is_font;
}

int MovableLabel::textSize() const
{
    return text_size;
}

void MovableLabel::setTextSize(const int &text_size)
{
    if (text_size < 4) return;

    const QString __sep = ";";
    QStringList style_sheet_list = styleSheet().split(__sep);
    style_sheet_list[0] = QString("font: %1pt \"黑体\"")
            .arg(QString::number(this->text_size = (text_size ? text_size : (13))));

    setStyleSheet(QString(style_sheet_list.join(__sep)));
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

