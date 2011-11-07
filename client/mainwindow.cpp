#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //Example of gui classes use

    //the second parameter in the constructor of GuiRenderer
    //implies that this object must provide the 2 functions
    //redirect and submit as public slots
    GuiRenderer *renderEngine = new GuiRenderer(ui->canvas->widget(),this);
    GuiBuilder builder(renderEngine);
    builder.buildFile("../htdocs/test1");
    //delete renderEngine
}

void MainWindow::redirect(QString str){
    qDebug() << "Redirect " << str;
}

void MainWindow::submit(QObject* obj){
    QStringList * lst = (QStringList *) obj;
    //the list contains the parameters list , you can use the GuiRender
    //instance to get the value using the method getTextBoxValue()
    //the last item in the list is the action to redirect to
    qDebug() << "processing parameters";
    redirect(lst->at(lst->size()-1));
}


MainWindow::~MainWindow(){
    delete ui;
}
