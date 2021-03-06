#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <opencv2/opencv.hpp>
#include <ctime>

#include <OpenCVTool.h>
#include <ImageGraphicsview.h>
#include <ColorLabel.h>
#include <ThicknessSlider.h>
#include <ImagePair.h>

#include <QMainWindow>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QString>
#include <QColor>
#include <QPalette>
#include <QAction>
#include <QFile>
#include <QDir>
#include <QFileSystemModel>
#include <QMessageBox>
#include <QFileDialog>
#include <QColorDialog>
#include <QPixmap>
#include <QPoint>
#include <QPointF>
#include <QScrollBar>
#include <QDebug>

using namespace std;
using namespace cv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_Open_triggered();
    void on_LoadImageListView_doubleClicked(const QModelIndex &index);
    void on_SaveImageListView_doubleClicked(const QModelIndex &index);
    void on_LoadImagePathLineEdit_returnPressed();
    void on_SaveImagePathLineEdit_returnPressed();
    void on_LoadImagePathButton_clicked();
    void on_SaveImagePathButton_clicked();
    void updatePixelLocationLabel(QString location);//当加载图片后，在状态栏显示鼠标所指向的图片的像素位置
    void on_Pencil_triggered();
    void on_Hand_triggered();
    void on_Eraser_triggered();
    void on_Glasses_triggered();
    void on_RectSelect_triggered();
    void on_FreeSelect_triggered();
    void OriHor2CurHorScrollBar(int);//当点击原始视图水平滚动条移动时，同步操作视图水平滚动条的位置
    void OriVer2CurVerScrollBar(int);//当点击原始视图垂直滚动条移动时，同步操作视图垂直滚动条的位置
    void CurHor2OriHorScrollBar(int);//当点击操作视图水平滚动条移动时，同步原始视图水平滚动条的位置
    void CurVer2OriVerScrollBar(int);//当点击操作视图垂直滚动条移动时，同步原始视图垂直滚动条的位置
    void on_GetColor_triggered();
    void changeForeColor();//改变前景色
    void changeBackColor();//改变背景色
    void on_Save_triggered();
    void on_Undo_triggered();
    void on_Redo_triggered();
    void on_getMaskAction_triggered();
    void on_getResultAction_triggered();

private:
    Ui::MainWindow *ui;
    QFileSystemModel *markedImageModel;
    QFileSystemModel *unMarkedImageModel;
    QPixmap oriPixmap;
    QPixmap currentPixmap;
    QGraphicsScene *oriScence;
    QGraphicsPixmapItem *oriPixmapItem;
    QGraphicsScene *currentScence;
    QGraphicsPixmapItem *currentPixmapItem;

    QPalette palette;//调色板

    inline void resetAction();//重置工具栏按钮的选中状态

    ThicknessSlider *thicknessSlider;//粗细设置控件
    QAction *thicknessAction;//粗细设置action

    OpenCVTool opencvtool;

    //图像
    vector<ImagePair> calculateImg;
    int isContainMarkedImg(string markedPath);//集合中是否存在水印图
    int isContainUnMarkedImg(string unmarkedPath);//集合中是否存在去水印图
    //水印位置
    int X, Y, WIDTH, HEIGHT;
    //当前的图片Mat
    Mat markedMat;
    Mat unmarkedMat;
    //当前图片目录路径
    string markedImageDirPath;
    string unMarkedImageDirPath;
    //当前图片的名字
    QString markedImageFileName;
    //当前图片列表目录
    QString curLoadImageDirPath = "D://Pictures/WatermarkTest/"; // 调试输入图片路径
    QString curSaveImageDirPath = "";

    QPixmap maskPixmap;//水印Pixmap
    Mat maskMat;//水印Mat
    Mat opacityMat;//混合比Mat
    QGraphicsScene *maskScene;
    QGraphicsPixmapItem *maskItem;//水印Item

    QPixmap resultPixmap;//模拟去除pixmap
    Mat resultMat;//模拟去除Mat
    QGraphicsScene *resultScene;
    QGraphicsPixmapItem *resultItem;//模拟去除Item

    Rect watermarkRegion();
    void updateWatermarkRegionLabel(Rect region);
};

#endif // MAINWINDOW_H
