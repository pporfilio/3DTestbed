#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//handle key presses here so I can update the GUI as well.
void MainWindow::keyPressEvent(QKeyEvent *event) {
    GLWidget *mgl = ui->mainGL;
    mgl->keyPressEvent(event);
}
