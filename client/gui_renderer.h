#ifndef GUI_RENDERER_H
#define GUI_RENDERER_H

#include <QtGui>
#include <QSignalMapper>
#include <QRadioButton>

class GuiRenderer
{
private:
    QObject * handler;
    QWidget * mainwidget;
    QFormLayout *formLayout;
public:
    //qwidget for rendering
    //qobject for handling actions
    GuiRenderer(QWidget*,QObject*);
    ~GuiRenderer();

    //clears the mainwidget by deleting all its children
    void clear();

    /*draws a link into the main widget redirecting to the given link
     *and have the given label
    */
    void drawLink(QString link,QString label);


    /*draws a form submit button with the given parameter list
     *input:
     *QStringList : from parameter list of names
     *QString link : form action
     *QString label : button label
    */
    void drawSubmitButton(QStringList* lst,QString link, QString label);


    /* draws an input text box
     * input:
     * QString name : the name of the text box object
     */
    void drawTextBox(QString name);

    /* draws an input password text box
     * input:
     * QString name : the name of the text box object
     */
    void drawPasswordTextBox(QString name);

    /* draws a plain text
     * input:
     * QString text : the text to be drawn
     */
    void drawText(QString text);

    /*
     * draws an input radio button
     * QString name: the name of the radio button object
     */
    void drawRadioButton(QString name);


    /* returns the value of the given textbox name
     * input:
     * QString name : the name of the text box object
     * output :
     * QString : the value of the text box
     */
    QString getTextBoxValue(QString name);

    /*
     *
     */
    bool getRadioButtonValue(QString name);
};

#endif // GUI_RENDERER_H
