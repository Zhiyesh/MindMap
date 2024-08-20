#include "movablelabel.h"

MovableLabel::MovableLabel(const Ml::LabelType &type,
    const QString &text,
    const int &w,
    const int &h,
    const int &rotate,
    const int &text_size,
    QWidget *parent) : QLabel(parent)
{
    setFixedSize(w, h);
    labelType = type;

    //其他控件
    if (labelType)
    {
        ResImg = QImage(LabelRes[labelType - 1]);
        if (rotate)
        {
            ResImg = ResImg.transformed(QTransform().rotate(rotate_value = rotate));
        }

        this->setPixmap(QPixmap::fromImage(ResImg));
        this->setScaledContents(true);
    }
    //字体控件
    else
    {
        is_font = true;
        setText(text);
        setStyleSheet(
            QString("font: %1pt \"黑体\";")
                    .arg(QString::number(
                        this->text_size = (text_size ? text_size : (FONT_LABEL_SIZE))
                    ))
        );
        setFixedSize(this->sizeHint().width(), FONT_HEIGHT);
    }

    setStyleSheet(
        QString(this->styleSheet()).append("background-color: rgb(0, 0, 0, 0);")
    );
}

Ml::LabelType MovableLabel::type() const
{
    return labelType;
}

void MovableLabel::rotateRight()
{
    const int previous_degree = rotate_value;
    rotate_value += 90;

    if (rotate_value >= 360) rotate_value = 0;

    setPixmap(QPixmap::fromImage(ResImg = (
        ResImg.transformed(QTransform().rotate(rotate_value - previous_degree))
    )));

    setFixedSize(this->height(), this->width());
}

int MovableLabel::rotateValue() const
{
    return rotate_value;
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
    if (labelType != Ml::FontType || text_size < 4) return;

    const QString __sep = ";";
    QStringList style_sheet_list = styleSheet().split(__sep);
    style_sheet_list[0] = QString("font: %1pt \"黑体\"")
            .arg(QString::number(this->text_size = (text_size ? text_size : (FONT_LABEL_SIZE))));

    setStyleSheet(QString(style_sheet_list.join(__sep)));

    QFontMetrics __metrics = this->fontMetrics();
    setFixedSize(__metrics.width(this->text()), __metrics.height());
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

