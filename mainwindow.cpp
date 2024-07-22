#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    this->setFixedSize(1346, 867);
    this->setStyleSheet("font: 11.5pt \"黑体\";");

    this->MenuBar = new QMenuBar(this);
    MenuBar->setFixedSize(this->width() - 20, 33);
    MenuBar->move(MenuBar->x() + 10, MenuBar->y());
    MenuBar->setStyleSheet("background-color: rgb(0, 0, 0, 0);");

    this->File = new QMenu("文件", this);
    this->Widget = new QMenu("界面", this);
    this->AddLabel = new QMenu("添加", this);
    this->Text = new QMenu("文字", this);
    MenuBar->addMenu(File);
    MenuBar->addMenu(Widget);
    MenuBar->addMenu(AddLabel);
    MenuBar->addMenu(Text);

    //文件栏
    this->NewLayout = new QAction("新建", File);
    this->OpenFile = new QAction("打开", File);
    this->SaveFile = new QAction("保存", File);
    SaveFile->setEnabled(false);
    this->CloseFile = new QAction("关闭", File);
    CloseFile->setEnabled(false);
    connect(NewLayout, &QAction::triggered, this, &MainWindow::newLayout);

    connect(OpenFile, &QAction::triggered, this, &MainWindow::openFile);

    connect(SaveFile, &QAction::triggered, this, &MainWindow::saveFile);

    connect(CloseFile, &QAction::triggered, this, &MainWindow::closeFile);
    File->addAction(NewLayout);
    File->addAction(OpenFile);
    File->addAction(SaveFile);
    File->addAction(CloseFile);

    //界面栏
    this->ChangeWidget = new QAction("调整窗口大小", Widget);
    this->ClearAllLabel = new QAction("清空", Widget);
    connect(ChangeWidget, &QAction::triggered, [this]() {
        //Set cursor style
        this->setCursor(Qt::SizeVerCursor);
        if (this->isChangingWidget) return;
        this->isChangingWidget = true;

        while (this->isChangingWidget)
        {
            if (this->isHidden()) break;
            Process::sleep(1e2);
        }
        //Recover the cursor
        this->setCursor(Qt::ArrowCursor);
    });

    connect(ClearAllLabel, &QAction::triggered, [this]() {
        this->removeLabel(Q_NULLPTR, Ml::AllLabels);
    });
    Widget->addAction(ChangeWidget);
    Widget->addAction(ClearAllLabel);

    //添加控件栏
    this->NewBracket = new QAction(QIcon(":/drawable/res/Bracket.png"), "括号", AddLabel);
    this->NewHLine = new QAction(QIcon(":/drawable/res/HLine.png"), "横线", AddLabel);
    connect(NewBracket, &QAction::triggered, [this]() {
        MovableLabel* currentML = this->newLabel(32, 183, Ml::Bracket, NULL);
        emit currentML->click();
    });

    connect(NewHLine, &QAction::triggered, [this]() {
        MovableLabel* currentML = this->newLabel(138, 26, Ml::HLine, NULL);
        emit currentML->click();
    });
    AddLabel->addAction(NewBracket);
    AddLabel->addAction(NewHLine);

    //文字栏
    this->AddText = new QAction("添加文字", Text);
    connect(AddText, &QAction::triggered, [this]() {
        if (!this->font_input->isHidden())
        {
            this->font_input->setFocus();
        }
        //Set cursor style
        this->setCursor(Qt::IBeamCursor);
        this->selectingFontPos = true;

        //鼠标左键确定文字位置
        while (!this->selectedFontPos)
        {
            if (this->isHidden()) break;
            if (this->canceledSelectFontPos)
            {
                this->canceledSelectFontPos = false;
                this->setCursor(Qt::ArrowCursor);
                return;
            }
            Process::sleep(1e2);
        }
        QPoint mouse = this->mapFromGlobal(this->cursor().pos());
        this->font_input->move(mouse.x(), mouse.y() - (font_input->height() >> 1));
        this->font_input->show();
        this->font_input->setFocus();
        this->selectedFontPos = false;
        //Recover the cursor
        this->setCursor(Qt::ArrowCursor);
    });
    Text->addAction(AddText);

    this->labels_menu = new QMenu(this);
    QAction* const labels_menu_remove = new QAction("移除", this);
    connect(labels_menu_remove, &QAction::triggered, [this]() {
        if (this->chosedLabel != Q_NULLPTR)
        {
            this->removeLabel(this->chosedLabel);
            this->chosedLabel = Q_NULLPTR;
        }
    });
    labels_menu->addAction(labels_menu_remove);

    this->font_input = new QLineEdit(this);
    font_input->resize(FONT_LABEL_SIZE * 2, 40);
    font_input->setContextMenuPolicy(Qt::NoContextMenu);
    font_input->setStyleSheet(QString("font: %1pt \"黑体\"; background-color: rgb(0, 0, 0, 0); border: 1.5px dotted;").arg(FONT_LABEL_SIZE));
    font_input->hide();
    connect(font_input, &QLineEdit::textChanged, [this]() {
        //Adaptive width
        int __width = FONT_LABEL_SIZE * 2;
        for (int i = 0; i < this->font_input->text().size(); i++)
        {
            const QChar each= this->font_input->text().at(i);
            if (each.isNumber() || each.isSpace()
                    || (each > 'A' && each < 'Z') || (each > 'a' && each < 'z'))
            {
                __width += FONT_LABEL_SIZE;
            }
            else __width += FONT_LABEL_SIZE * 2;
        }
        font_input->resize(__width, font_input->height());
    });
    connect(font_input, &QLineEdit::returnPressed, this, &MainWindow::buildFont);

    this->NoFileIsOpen = new QLabel("无文件打开", this);
    NoFileIsOpen->setFixedSize(5 * 2 * 12, 50);
    NoFileIsOpen->move((this->width() >> 1) - NoFileIsOpen->width(), (this->height() >> 1) - NoFileIsOpen->height());
    NoFileIsOpen->setStyleSheet("font: 12pt;");
    NoFileIsOpen->hide();
}

MainWindow::~MainWindow()
{
    delete labels_menu;
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    //鼠标按键取消 滚轮缩放窗口 的状态
    if (this->isChangingWidget)
    {
        this->isChangingWidget = false;
    }
    else if (e->button() == Qt::MiddleButton) emit this->ChangeWidget->triggered();

    if (e->button() == Qt::LeftButton)
    {
        //左键确定字体控件位置
        if (this->selectingFontPos && !this->selectedFontPos)  //当鼠标为 I 时
        {
            this->selectedFontPos = true;
            this->selectingFontPos = false;
        }
        //再次点击左键
        else this->buildFont();
    }

    if (e->button() == Qt::RightButton)
    {
        //右键取消放置字体控件
        if (this->selectingFontPos && !this->selectedFontPos)  //当鼠标为 I 时
        {
            this->canceledSelectFontPos = true;
            this->selectedFontPos = false;
            this->selectingFontPos = false;
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    //未选中控件
    if (!this->hasLabelMoving)
    {
        if (e->modifiers() == Qt::ControlModifier)
        {
            if (e->key() == Qt::Key_R)
            {
                //Ctrl + R
                this->reloadFile();
            }
            return;
        }

        if (this->AddLabel->isEnabled() && this->Text->isEnabled())
        {
            switch (e->key())
            {
            //按键快速添加控件
            case Qt::Key_1:
                emit this->NewBracket->triggered();
                break;
            case Qt::Key_2:
                emit this->NewHLine->triggered();
                break;
            case Qt::Key_F:
                emit this->AddText->triggered();
                break;
            }
        }
    }
    //已选中控件
    else
    {
        switch (e->key())
        {
        //回车键确定控件位置
        case Qt::Key_Return:
            if (this->chosedLabel != Q_NULLPTR)
            {
                emit this->chosedLabel->click();
                this->chosedLabel = Q_NULLPTR;
            }
            break;
        //删除控件
        case Qt::Key_Delete:
            {
                this->hasLabelMoving = false;
                this->removeLabel(this->chosedLabel);
            }
            break;
        //方向键移动控件
        case Qt::Key_Left:
            QCursor::setPos(QCursor::pos().x() - 1, QCursor::pos().y());
            break;
        case Qt::Key_Right:
            QCursor::setPos(QCursor::pos().x() + 1, QCursor::pos().y());
            break;
        case Qt::Key_Up:
            QCursor::setPos(QCursor::pos().x(), QCursor::pos().y() - 1);
            break;
        case Qt::Key_Down:
            QCursor::setPos(QCursor::pos().x(), QCursor::pos().y() + 1);
            break;
        }
    }
}

void MainWindow::wheelEvent(QWheelEvent *e)
{
    //控件大小变化步长
    const int LABEL_STEP = 2;
    //控件最大和最小大小
    const int LABEL_MAX = 343;
    const int LABEL_MIN = 17;

    //向上滚动
    if (e->angleDelta().y() > 0)
    {
        //窗口缩小
        if (this->isChangingWidget) this->changeWidget(-20);

        //控件缩小
        if (this->hasLabelMoving &&
                this->chosedLabel->width() > LABEL_MIN && chosedLabel->height() > LABEL_MIN)
        {
            double __ratio = (double)chosedLabel->width() / (double)chosedLabel->height();
            if (__ratio > 1)
            {
                int __chheight = chosedLabel->height() - LABEL_STEP;
                this->chosedLabel->setFixedSize(__chheight * __ratio, __chheight);
            }
            else
            {
                int __chwidth = chosedLabel->width() - LABEL_STEP;
                this->chosedLabel->setFixedSize(__chwidth, (int)__chwidth / __ratio);
            }
        }
    }
    //向下滚动
    else if (e->angleDelta().y() < 0)
    {
        //窗口放大
        if (this->isChangingWidget) this->changeWidget(20);

        //控件放大
        if (this->hasLabelMoving &&
                this->chosedLabel->width() < LABEL_MAX && chosedLabel->height() < LABEL_MAX)
        {
            double __ratio = (double)chosedLabel->width() / (double)chosedLabel->height();
            if (__ratio > 1)
            {
                int __chheight = chosedLabel->height() + LABEL_STEP;
                this->chosedLabel->setFixedSize(__chheight * __ratio, __chheight);
            }
            else
            {
                int __chwidth = chosedLabel->width() + LABEL_STEP;
                this->chosedLabel->setFixedSize(__chwidth, (int)__chwidth / __ratio);
            }
        }
    }
}

void MainWindow::changeWidget(const int &step)
{
    const int targetHeight = this->height() + step;
    if (targetHeight < 300 || targetHeight > QApplication::desktop()->screenGeometry().height()) return;
    this->setFixedSize(this->width() + step, this->height() + step);
    this->move(this->x() - (step >> 1), this->y() - (step >> 1));
    this->MenuBar->setFixedSize(MenuBar->width() + step, MenuBar->height());
}

void MainWindow::removeLabel(MovableLabel *label, const Ml::LabelSelect &select)
{
    bool flag = false;
    if (select == Ml::AllLabels)
    {
        flag = true;
        this->font_input->hide();
        this->setFocus();
        this->font_input->setText("");
    }

    for (vector<MovableLabel*>::iterator it = this->labels.begin(); it != labels.end(); it++)
    {
        MovableLabel* const currentML = *it;
        if (currentML == label || flag)
        {
            currentML->hide();
            delete currentML;
            this->labels.erase(it--);
            if (!flag) break;
        }
    }
    if (!(this->filePath.isEmpty() && this->labels.empty())) this->SaveFile->setEnabled(true);
    else this->SaveFile->setEnabled(false);
}

MovableLabel* MainWindow::newLabel(const int &w, const int &h, const Ml::LabelType &type, const QString &text)
{
    this->isChangingWidget = false;
    MovableLabel* const label = new MovableLabel(w, h, type, text, this);
    label->show();

    //控件移动
    connect(label, &MovableLabel::click, [=]() {
        if (this->isChangingWidget) return;
        this->hasLabelMoving = !this->hasLabelMoving;
        if (this->hasLabelMoving)
        {
            this->setCursor(Qt::SizeAllCursor);
        }
        else if (this->selectingFontPos)
        {
            this->setCursor(Qt::IBeamCursor);
        }

        this->chosedLabel = label;
        while (this->hasLabelMoving)
        {
            if (this->isHidden()) break;
            QPoint mouse = this->mapFromGlobal(this->cursor().pos());
            label->move(mouse.x() - (label->width() >> 1), mouse.y() - (label->height() >> 1));
            Process::sleep(1e1);
        }
        this->setCursor(Qt::ArrowCursor);
        this->chosedLabel = Q_NULLPTR;
        this->SaveFile->setEnabled(true);
    });

    //控件菜单栏呼出
    connect(label, &MovableLabel::clickRight, [=]() {
        if (this->hasLabelMoving) emit label->click();
        this->chosedLabel = label;
        this->labels_menu->move(this->cursor().pos().x(), this->cursor().pos().y());
        this->labels_menu->show();
    });

    //字体控件编辑
    connect(label, &MovableLabel::doubleClick, [=]() {
        emit label->click();
        if (!label->isFont()) return;

        this->font_input->show();
        font_input->setFocus();
        font_input->setText(label->text());
        font_input->move(label->x(), label->y());
        this->removeLabel(label);
    });

    this->labels.push_back(label);
    this->SaveFile->setEnabled(true);

    return label;
}

void MainWindow::buildFont()
{
    this->font_input->hide();
    this->setFocus();

    QString text = this->font_input->text();
    if (!text.isEmpty())
    {
        //创建Label控件替换LineEdit控件
        MovableLabel *label = this->newLabel(this->font_input->width() - FONT_LABEL_SIZE * 2, this->font_input->height(), Ml::FontType, text);
        label->move(this->font_input->x(), this->font_input->y());
        label->setStyleSheet(QString("font: %1pt \"黑体\";").arg(FONT_LABEL_SIZE));
    }
    this->font_input->setText("");
}

bool MainWindow::existedFileToSave()
{
    //提示是否要保存当前的labels
    int result = QMessageBox::information(this, "保存文件", "当前文件未保存，是否先保存？", "保存", "放弃", "取消");
    if (result == 2) return true;  //取消

    if (result == 0)  //保存
    {
        emit this->SaveFile->triggered();
    }
    //放弃  不执行任何代码

    return false;
}

bool MainWindow::newLayout()
{
    if (this->SaveFile->isEnabled() && this->existedFileToSave()) return false;

    this->removeLabel(Q_NULLPTR, Ml::AllLabels);
    this->SaveFile->setEnabled(false);
    this->CloseFile->setEnabled(false);

    this->filePath = QString();
    this->Widget->setEnabled(true);
    this->AddLabel->setEnabled(true);
    this->Text->setEnabled(true);
    this->NoFileIsOpen->hide();

    return true;
}

void MainWindow::openFile()
{
    if (this->SaveFile->isEnabled() && this->existedFileToSave()) return;

    QString filePath_Tmp = QFileDialog::getOpenFileName(this, "打开文件", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation), "Json文件 (*.json)");
    if (filePath_Tmp.isEmpty()) return;

    this->filePath = filePath_Tmp;

    if (!this->labels.empty())
    {
        this->removeLabel(Q_NULLPTR, Ml::AllLabels);
    }

    //读文件
    QFile fileManager(this->filePath);
    if (!fileManager.open(QFile::ReadOnly))
    {
        QMessageBox::warning(this, "失败", "打开失败！\n请检查文件是否正确");
        return;
    }

    QString fileData = fileManager.readAll();

    QJsonDocument labelInfo = QJsonDocument::fromJson(fileData.toUtf8());

    this->NoFileIsOpen->hide();

    if (!labelInfo.isObject()) return;

    QJsonObject labelObj = labelInfo.object();
    jsonToLabel(labelObj);

    this->SaveFile->setEnabled(false);
    this->CloseFile->setEnabled(true);
    this->Widget->setEnabled(true);
    this->AddLabel->setEnabled(true);
    this->Text->setEnabled(true);
}

void MainWindow::jsonToLabel(const QJsonObject &object)
{
    int x = 0, y = 0, w = 0, h = 0, type = 0;
    QString text = QString();

    QStringList __keys = object.keys();
    for (const QString &key : __keys)
    {
        QJsonValue value = object.value(key);
        if (value.isObject())
        {
            jsonToLabel(value.toObject());
            continue;
        }

        if (value.isDouble())
        {
            if (key == "x") x = value.toDouble();
            if (key == "y") y = value.toDouble();
            if (key == "width") w = value.toDouble();
            if (key == "height") h = value.toDouble();
            if (key == "type") type = value.toDouble();
        }

        if (value.isString())
        {
            if (key == "text")
            {
                text = value.toString();
            }
        }
    }

    if (w * h != 0)
    {
        MovableLabel* label = this->newLabel(w, h, Ml::LabelType(type), text);
        label->move(x, y);
    }
}

void MainWindow::reloadFile()
{
    if (this->filePath.isEmpty()) return;

    if (!this->labels.empty())
    {
        this->removeLabel(Q_NULLPTR, Ml::AllLabels);
        if (!this->filePath.isEmpty()) this->SaveFile->setEnabled(false);
    }

    //读文件
    qDebug() << filePath;
}

void MainWindow::saveFile()
{
    if (this->filePath.isEmpty())
    {
        this->filePath = QFileDialog::getSaveFileName(
                    this, "保存文件",
                    QString((this->filePath.isNull() || this->filePath.isEmpty())
                            ? QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) : this->filePath),
                    "Json文件 (*.json)"
                    );
        if (this->filePath.isEmpty()) return;
    }

//    this->filePath is not empty

    //写文件
    qDebug() << filePath;
    QJsonDocument __label_info;

    QJsonObject __flag;
    QJsonObject __label_obj;

    for (size_t i = 0; i < this->labels.size(); i++)
    {
        MovableLabel* const currentLabel = this->labels[i];
        QJsonObject currentObj;
        currentObj.insert("x", QJsonValue(currentLabel->x()));
        currentObj.insert("y", QJsonValue(currentLabel->y()));
        currentObj.insert("width", QJsonValue(currentLabel->width()));
        currentObj.insert("height", QJsonValue(currentLabel->height()));
        currentObj.insert("type", QJsonValue(currentLabel->type()));
        currentObj.insert("text", QJsonValue(currentLabel->text()));
        __label_obj.insert(QString::number(i + 1), QJsonValue(currentObj));
    }
    __flag.insert("Build by MindMap", __label_obj);

    __label_info.setObject(__flag);
    QByteArray fileData =  __label_info.toJson();

    QFile fileManager(this->filePath);
    if (fileManager.open(QFile::WriteOnly))
    {
        fileManager.write(fileData);
        this->SaveFile->setEnabled(false);
        fileManager.close();
    }
    else
    {
        QMessageBox::warning(this, "失败", "文件保存失败！");
        return;
    }
}

void MainWindow::closeFile()
{
    if (this->filePath.isEmpty()) return;

    if (!this->newLayout()) return;

    this->Widget->setEnabled(false);
    this->AddLabel->setEnabled(false);
    this->Text->setEnabled(false);
    this->NoFileIsOpen->show();
}

