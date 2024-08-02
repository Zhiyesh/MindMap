#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>

#include <QMouseEvent>
#include <QMimeData>

#include <QTime>
#include <QApplication>
#include <QCoreApplication>
#include <QDesktopWidget>
#include <QLineEdit>
#include <QMessageBox>
#include <QStandardPaths>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>

#include <vector>
#include <list>

#include "movablelabel.h"

#include <QDebug>

#define FONT_LABEL_SIZE  13

using std::vector;
using std::list;

class Process
{
public:
    static void sleep(const int &ms)
    {
        QTime targetTime = QTime::currentTime().addMSecs(ms);
        while (targetTime > QTime::currentTime())
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents);
        }
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* e);

    void mousePressEvent(QMouseEvent *e);
    void keyPressEvent(QKeyEvent* e);
    void wheelEvent(QWheelEvent* e);

    void dragEnterEvent(QDragEnterEvent* e);
    void dropEvent(QDropEvent* e);

protected:
    void changeWidget(const int &step);
    void removeLabel(MovableLabel *label, const Ml::LabelSelect &select = Ml::None);

    MovableLabel*
    newLabel(const int &w,
           const int &h,
           const Ml::LabelType &type,
           const QString &text = QString(),
           const QString &text_size = QString());

    void buildFont();
    bool existedFileToSave();
    bool newLayout();
    void openFile(QString filePath = QString());
    void jsonToLabel(const QJsonObject &object);
    bool saveFile();
    void closeFile();
    void clearChangedLabelsList();

//Operation Undo and Redo
protected:
    void undoLatest();
    void cancelUndo();
private:
    list<vector<MovableLabel*>>::iterator __currentOP;
//

private:
    QMenuBar* MenuBar;   //菜单栏
    QMenu *File;         //文件
    QMenu *Edit;         //编辑
    QMenu *Widget;       //界面
    QMenu *AddLabel;     //添加
    QMenu *Text;         //文字

    QAction *NewLayout;      //新建
    QAction *OpenFile;       //打开
    QAction *SaveFile;       //保存
    QAction *CloseFile;      //关闭

    QAction *Undo;           //撤销
    QAction *Redo;           //重做

    QAction *ChangeWidget;   //设置窗口大小
    QAction *ClearAllLabel;  //清空窗口

    QAction *NewBracket;  //括号
    QAction *NewHLine;   //横线
    QAction *AddText;    //添加文字

    vector<MovableLabel*> labels;
    list<vector<MovableLabel*>> recent_changed_labels;
    QMenu* labels_menu;
    QLineEdit* font_input;
    QLabel* NoFileIsOpen;
    QString filePath;

private:
    bool isChangingWidget = false;
    bool hasLabelMoving = false;
    MovableLabel* chosedLabel;
    bool selectingFontPos = false;
    bool selectedFontPos = false;
    bool canceledSelectFontPos = false;
};

#endif // MAINWINDOW_H
