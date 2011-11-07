#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QDebug>
#include "gui_renderer.h"
#include "gui_builder.h"

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

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
