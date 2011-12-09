#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //init buffer
    buffer = new char[BUFFER_SIZE];

    //setup ui
    ui->setupUi(this);
    ui->mainLayout->setContentsMargins(10,10,10,10);
    this->setCentralWidget(ui->verticalLayoutWidget);
    QObject::connect(ui->goButton,SIGNAL(clicked()),SLOT(go()));
    QObject::connect(ui->urlLineEdit,SIGNAL(returnPressed()),SLOT(go()));
    connect(ui->remoteTreeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            SLOT(fetchFolder(QTreeWidgetItem*,int)));
    connect(ui->downloadButton,SIGNAL(clicked()),SLOT(downloadFile()));
    connect(ui->uploadButton,SIGNAL(clicked()),SLOT(uploadFile()));


    //setup local ftp tree
    QFileSystemModel *model = new QFileSystemModel();
    model->setRootPath(QDir::currentPath());
    ui->localTreeView->setModel(model);

    //dummy data
    QTreeWidgetItem* it = new QTreeWidgetItem();
    it->setText(0,"test");
    it->setText(1,"test");
    it->setData(0,Qt::UserRole,QVariant("data/"));
    ui->remoteTreeWidget->addTopLevelItem(it);

    renderEngine = new GuiRenderer(ui->canvas->widget(),this);
}

void MainWindow::fetchFolder(QTreeWidgetItem *item, int c){
    qDebug() << "fetching ..";
    qDebug() << item->data(0,Qt::UserRole).toString();
}

void MainWindow::downloadFile(){
    //get the remote file name
    QTreeWidgetItem * item = ui->remoteTreeWidget->currentItem();
    if (item == 0){
        QMessageBox::warning(this, tr("download Error"),
           tr("please select a remote file to download"));
        return;
    }
    QString remoteFile = item->data(0,Qt::UserRole).toString();
    qDebug() << remoteFile;
    if (remoteFile[remoteFile.length()-1] == '/'){
        QMessageBox::warning(this, tr("download Error"),
           tr("please select a remote file not a folder"));
        return;
    }
    //get the local folder path
    QFileSystemModel * model =(QFileSystemModel*) ui->localTreeView->model();
    QModelIndexList indexes = ui->localTreeView->selectionModel()->selectedRows();
    if (indexes.size() == 0){
        QMessageBox::warning(this, tr("download Error"),
           tr("please select a destination folder"));
        return;
    }
    QModelIndex index = indexes.at(0);
    QFileInfo info = model->fileInfo(index);
    if (info.isFile()){
        QMessageBox::warning(this, tr("download Error"),
           tr("please choose a destination folder not file"));
        return;
    }
    qDebug() << info.absoluteFilePath();
    //downloading...
}

void MainWindow::uploadFile(){
    //get local selected file
    QFileSystemModel * model =(QFileSystemModel*) ui->localTreeView->model();
    QModelIndexList indexes = ui->localTreeView->selectionModel()->selectedRows();
    if (indexes.size() == 0){
        QMessageBox::warning(this, tr("upload Error"),
           tr("please select a source file"));
        return;
    }
    QModelIndex index = indexes.at(0);
    QFileInfo info = model->fileInfo(index);
    if (info.isDir()){
        QMessageBox::warning(this, tr("upload Error"),
           tr("please choose a source file not folder"));
        return;
    }
    qDebug() << info.absoluteFilePath();
    //get remote selected folder
    QTreeWidgetItem * item = ui->remoteTreeWidget->currentItem();
    if (item == 0){
        QMessageBox::warning(this, tr("upload Error"),
           tr("please select a remote folder to upload to"));
        return;
    }
    QString remoteFolder = item->data(0,Qt::UserRole).toString();
    qDebug() << remoteFolder;
    if (remoteFolder[remoteFolder.length()-1] != '/'){
        QMessageBox::warning(this, tr("upload Error"),
           tr("please select a remote folder not a file"));
        return;
    }
    //do the uploading
}


void MainWindow::redirect(QString str){
    int index = currentUrl.lastIndexOf("/");
    QString server = currentUrl.mid(0,index);
    qDebug() << "Redirecting to " << str;
    currentUrl = server + "/" + str;
    ui->urlLineEdit->setText(currentUrl);
    go();
}

void MainWindow::submit(QObject* obj){
    QStringList * lst = (QStringList *) obj;
    //the list contains the parameters list , you can use the GuiRender
    //instance to get the value using the method getTextBoxValue()
    //the last item in the list is the action to redirect to

    QString new_page = lst->at(lst->size()-1);
    if (lst->size() > 1){
        new_page += "?";
        for (int i=0;i<lst->size()-1;++i){
            QString name = lst->at(i).mid(lst->at(i).lastIndexOf("|") + 1);
            if (i == lst->size() -2)
                new_page += name + "=\"" + renderEngine->getTextBoxValue(lst->at(i)) + "\"";
            else
                new_page += name + "=\"" + renderEngine->getTextBoxValue(lst->at(i)) + "\"&";
        }
    }
    redirect(new_page);
}

void MainWindow::go(){
    QString url = ui->urlLineEdit->text();

    ui->canvas->widget()->setCursor(Qt::WaitCursor);
    //special case index.html
    if (url.indexOf("/") == -1){
        url = url + "/index.html";
        ui->urlLineEdit->setText(url);
    }else if (url.indexOf("/") == url.length()-1){
        url = url + "index.html";
        ui->urlLineEdit->setText(url);
    }

    currentUrl = url;
    //parse the url to get the server name and requested file name
    int root_index = url.indexOf("/");
    QString server = url.mid(0,root_index);
    QString file_name;

    GuiBuilder builder(renderEngine);

    if (server.length() == 0){
        qDebug() << "invalid server name";
        builder.build("<p>invalid server name</p>");
        ui->canvas->widget()->setCursor(Qt::ArrowCursor);
        return;
    }

    int parameter_start_index = url.indexOf("?");
    file_name = url.mid(root_index);

    //no parameters
    if (parameter_start_index == -1)
        file_name = url.mid(root_index);
    else
        file_name = url.mid(root_index,parameter_start_index-root_index);

    HttpGetRequestBuilder request(file_name.toStdString());
    if (parameter_start_index != -1){
        //parse parameters
        int secondIndex = parameter_start_index;
        while (secondIndex != -1){
            int firstIndex = secondIndex + 1;
            secondIndex = url.indexOf("&",firstIndex);
            QString parameter;
            if (secondIndex == -1)
                parameter = url.mid(firstIndex);
            else
                parameter = url.mid(firstIndex,secondIndex-firstIndex);
            int eq_index = parameter.indexOf("=");
            //if it is a valid parameter (on the format name = value)
            if (eq_index != -1){
                QString name = parameter.mid(0,eq_index);
                QString value = parameter.mid(eq_index+1,parameter.length()-eq_index+1);
                request.addParameter(name.toStdString(),value.toStdString());
            }
        }
    }

    qDebug() << "trying to connect to server" << server << "...";
    char * server_name = (char*)server.toStdString().c_str();

    ClientSocket * socket;
    try{
        socket = new ClientSocket('T',6060,server_name);
    }catch(int e){
        qDebug() << "couldn't connect to server";
        builder.build("<p>couldn't connect to server</p>");
        ui->canvas->widget()->setCursor(Qt::ArrowCursor);
        return;
    }

    qDebug() << "requesting data ...";
    if (socket->writeToSocket((char*)request.getSerialization().c_str()) < 0){
        qDebug() << "couldn't send data";
        builder.build("<p>couldn't send requst to server</p>");
        ui->canvas->widget()->setCursor(Qt::ArrowCursor);
        delete socket;
        return;
    }

    qDebug() << "waiting for response ...";
    if (socket->readFromSocket(buffer,BUFFER_SIZE) < 0){
        qDebug() << "couldn't get response ...";
        builder.build("<p>couldn't send requst to server</p>");
        ui->canvas->widget()->setCursor(Qt::ArrowCursor);
        delete socket;
        return;
    }

    //todo : optimize this (overhead of converting from char * to string to qstring)
    QString response = QString::fromStdString(string(buffer));
    builder.build(response);

    ui->canvas->widget()->setCursor(Qt::ArrowCursor);
    delete socket;
    return;

}
MainWindow::~MainWindow(){
    delete ui;
    delete buffer;
    delete renderEngine;
}
