#include "gui_renderer.h"

GuiRenderer::GuiRenderer(QWidget* mainwidget,QObject * handler){
    this->mainwidget = mainwidget;
    this->formLayout = new QFormLayout(mainwidget);
    this->handler = handler;
}

void GuiRenderer::drawLink(QString name,QString label){
    QPushButton * q = new QPushButton(mainwidget);
    q->setText(label);
    formLayout->addWidget(q);

    //TODO: when does signal mapper get deleted !!!!
    QSignalMapper* signalMapper = new QSignalMapper();
    signalMapper->setMapping(q, name);
    QObject::connect(q,SIGNAL(clicked()),signalMapper,SLOT(map()));
    QObject::connect(signalMapper, SIGNAL(mapped(QString)),handler, SLOT(redirect(QString)));
}

void GuiRenderer::drawSubmitButton(QStringList *lst,QString link,QString label){

    if (label == "")
        label = "submit";
    QPushButton * q = new QPushButton(mainwidget);
    q->setText(label);
    formLayout->addWidget(q);

    //last element in list is the link which the submit button should go to
    lst->append(link);

    QSignalMapper* signalMapper = new QSignalMapper();
    signalMapper->setMapping(q,(QObject *) lst);
    QObject::connect(q,SIGNAL(clicked()),signalMapper,SLOT(map()));
    QObject::connect(signalMapper, SIGNAL(mapped(QObject *)),
                     handler, SLOT(submit(QObject *)));
}

void GuiRenderer::drawTextBox(QString name){
    QLineEdit * textbox = new QLineEdit(mainwidget);
    textbox->setObjectName(name);
    formLayout->addWidget(textbox);
}

void GuiRenderer::drawText(QString text){
    QLabel * lbl = new QLabel(mainwidget);
    formLayout->addWidget(lbl);
    lbl->setText(text);
}

QString GuiRenderer::getTextBoxValue(QString name){
    QLineEdit* tb = mainwidget->findChild<QLineEdit*>(name);
    if (tb != NULL)
        return tb->text();
    return "";
}

void GuiRenderer::clear(){
    QList<QWidget *> widgets = mainwidget->findChildren<QWidget *>();
    foreach(QWidget * widget, widgets)
        delete widget;
}

GuiRenderer::~GuiRenderer(){
    delete mainwidget;
}