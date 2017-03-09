#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <opencv2/opencv.hpp>
#include <ctime>

#include <QDebug>
#include <QString>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QImage>
#include <QPixmap>
#include <QFileSystemModel>
#include <QGraphicsScene>

using namespace std;
using namespace cv;

//图像
vector<Mat> markedImg;
vector<Mat> unmarkedImg;
//图片路径
vector<string> markedImgPath;
vector<string> unmarkedImgPath;
//水印位置
int X, Y, WIDTH, HEIGHT;
//图片目录
QString markedImageDirPath;
QString unMarkedImageDirPath;
//当前图片列表目录
QString curLoadImageDirPath = "";
QString curSaveImageDirPath = "";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->oriImage = new QImage();
    this->currentImage = new QImage();
    /**
     * 初始化图片列表
     */
    markedImageModel = new QFileSystemModel(this);
    //QDir::NoDotAndDotDot 将Windows中的"."和".."目录忽略
    markedImageModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files);

    QStringList filter;
    filter<<"*.jpg"<<"*.png"<<"*.bmp"<<"*.jpeg"<<"*.gif"<<"*.JPG"<<"*.PNG"<<"*.BMP"<<"*.JPEG"<<"*.GIF";

    markedImageModel->setNameFilters(filter);//过滤非指定类型
    markedImageModel->setNameFilterDisables(false);//隐藏被过滤的文件，没有此句的话不符合指定类型的文件以灰色不可选显示
    ui->LoadImageListView->setModel(markedImageModel);
    ui->LoadImageListView->setRootIndex(markedImageModel->setRootPath(curLoadImageDirPath));
    ui->LoadImagePathLineEdit->setText(curLoadImageDirPath);

    unMarkedImageModel = new QFileSystemModel(this);
    unMarkedImageModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files);

    unMarkedImageModel->setNameFilters(filter);//过滤非指定类型
    unMarkedImageModel->setNameFilterDisables(false);//隐藏被过滤的文件
    ui->SaveImageListView->setModel(unMarkedImageModel);
    ui->SaveImageListView->setRootIndex(unMarkedImageModel->setRootPath(curSaveImageDirPath));
    ui->SaveImagePathLineEdit->setText(curSaveImageDirPath);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete markedImageModel;
    delete unMarkedImageModel;
    delete oriImage;
    delete currentImage;
}

/**
 * @brief MainWindow::on_Open_triggered
 * 打开图片目录
 */
void MainWindow::on_Open_triggered()
{
    QString path = QFileDialog::getExistingDirectory(this,"请选择水印图像所在文件夹",QDir::currentPath());
    //当用户没有点击取消按钮时弹出
    if(!path.isEmpty()) {
        curLoadImageDirPath = path;
        ui->LoadImageListView->setRootIndex(markedImageModel->setRootPath(curLoadImageDirPath));
        ui->LoadImagePathLineEdit->setText(curLoadImageDirPath);
    }
}

/**
 * @brief MainWindow::on_LoadImageListView_doubleClicked
 * @param index
 * 原始图片列表双击打开目录 或 打开图片
 */
void MainWindow::on_LoadImageListView_doubleClicked(const QModelIndex &index)
{
    QString path = markedImageModel->fileInfo(index).absoluteFilePath();
    if(markedImageModel->fileInfo(index).isDir()) {//是目录则进入目录
        curLoadImageDirPath = path;
        ui->LoadImageListView->setRootIndex(markedImageModel->setRootPath(path));
        ui->LoadImagePathLineEdit->setText(path);
    }else if(!oriImage->load(path) || !currentImage->load(path)){//是图片则读取图片显示
        QMessageBox::warning(this,"提示","打开图像失败！");
    }else {
        QGraphicsScene *oriScence = new QGraphicsScene;
        oriScence->addPixmap(QPixmap::fromImage(*oriImage));
        ui->OriImageGraphicsView->setScene(oriScence);
        ui->OriImageGraphicsView->show();
        QGraphicsScene *currentScence = new QGraphicsScene;
        currentScence->addPixmap(QPixmap::fromImage(*currentImage));
        ui->CurrentImageGraphicsView->setScene(currentScence);
        ui->CurrentImageGraphicsView->show();
    }
}

/**
 * @brief MainWindow::on_SaveImageListView_doubleClicked
 * @param index
 * 保存图片列表双击打开目录=
 */
void MainWindow::on_SaveImageListView_doubleClicked(const QModelIndex &index)
{
    QString path = unMarkedImageModel->fileInfo(index).absoluteFilePath();
    if(unMarkedImageModel->fileInfo(index).isDir()) {
        curSaveImageDirPath = path;
        ui->SaveImageListView->setRootIndex(unMarkedImageModel->setRootPath(path));
        ui->SaveImagePathLineEdit->setText(path);
    }
}

/**
 * @brief MainWindow::on_LoadImagePathLineEdit_returnPressed
 * 原始图片路径响应回车
 */
void MainWindow::on_LoadImagePathLineEdit_returnPressed()
{
    QString path = ui->LoadImagePathLineEdit->text().trimmed();
    QDir dir(path);
    if(dir.exists()) {
        curLoadImageDirPath = path;
        ui->LoadImageListView->setRootIndex(markedImageModel->setRootPath(path));
        ui->LoadImagePathLineEdit->setText(path);
    }else {
        QMessageBox::warning(this,"提示","目录路径错误！");
    }
}

/**
 * @brief MainWindow::on_SaveImagePathLineEdit_returnPressed
 * 保存图片路径响应回车
 */
void MainWindow::on_SaveImagePathLineEdit_returnPressed()
{
    QString path = ui->SaveImagePathLineEdit->text().trimmed();
    QDir dir(path);
    if(dir.exists()) {
        curSaveImageDirPath = path;
        ui->SaveImageListView->setRootIndex(unMarkedImageModel->setRootPath(path));
        ui->SaveImagePathLineEdit->setText(path);
    }else {
        QMessageBox::warning(this,"提示","目录路径错误！");
    }
}

/**
 * @brief MainWindow::on_LoadImagePathButton_clicked
 * 原始图片路径按钮返回上一级
 */
void MainWindow::on_LoadImagePathButton_clicked()
{
    int first = curLoadImageDirPath.lastIndexOf ("/"); //从后面查找"/"位置
    QString path = curLoadImageDirPath.left(first);//获得上一级路径
    if(first < 0) {path = "";}
    curLoadImageDirPath = path;
    ui->LoadImageListView->setRootIndex(markedImageModel->setRootPath(path));
    ui->LoadImagePathLineEdit->setText(path);
}

/**
 * @brief MainWindow::on_SaveImagePathButton_clicked
 * 保存图片路径按钮返回上一级
 */
void MainWindow::on_SaveImagePathButton_clicked()
{
    int first = curSaveImageDirPath.lastIndexOf ("/"); //从后面查找"/"位置
    QString path = curSaveImageDirPath.left(first+1);//获得上一级路径
    if(first < 0) {path = "";}
    curSaveImageDirPath = path;
    ui->SaveImageListView->setRootIndex(unMarkedImageModel->setRootPath(path));
    ui->SaveImagePathLineEdit->setText(path);
}
