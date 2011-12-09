#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QDebug>
#include "gui_renderer.h"
#include "gui_builder.h"
#include "client_socket.h"
#include "http_get_request_builder.h"

#define BUFFER_SIZE 2048

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    //this method is called when a link is clicked
    void redirect(QString);
    //this method is called when a submit button is clicked
    //it parses the parameter list then forward the link to
    //the redirect method
    void submit(QObject *);

    //called when the go button is pressed
    //it reads the value in the urlLineEdit
    //and creates a new request and send it
    void go();

    //called when user clicks on an item in the remote tree
    void fetchFolder(QTreeWidgetItem *,int c);

    //downloads the selected file from server
    void downloadFile();

    //uploads the selected file to server
    void uploadFile();

private:
    Ui::MainWindow *ui;
    char * buffer;
    GuiRenderer * renderEngine;
    QString currentUrl;

};

#endif // MAINWINDOW_H
