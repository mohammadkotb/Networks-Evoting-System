#include "gui_builder.h"

#define P_TAG "P"
#define INPUT_TAG "INPUT"
#define FORM_TAG "FORM"
#define LINK_TAG "A"

#define TYPE_ATTRIB "type"
#define NAME_ATTRIB "name"
#define VALUE_ATTRIB "value"
#define ACTION_ATTRIB "action"
#define HREF_ATTRIB "href"

#define TEXT_ATTRIB_VAL "text"
#define PASSWORD_ATTRIB_VAL "password"
#define SUBMIT_ATTRIB_VAL "submit"
#define RADIO_ATTRIB_VAL "radio"
#define HIDDEN_ATTRIB_VAL "hidden"

GuiBuilder::GuiBuilder(GuiRenderer * engine){
    this->renderEngine = engine;
}

void GuiBuilder::buildFile(QString path){
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly)) {
        throw -1;
    }

    QTextStream in(&file);

    QString html = "";
    while(!in.atEnd())
        html += in.readLine();

    file.close();
    this->build(html);
}

void GuiBuilder::build(std::string html){
    QString text = QString::fromStdString(html);
    this->build(text);
}

void GuiBuilder::build(QString & html){
    //clean previous page
    renderEngine->clear();

    int formCounter = 0;
    QWebPage page;
    QWebFrame * frame = page.mainFrame();
    frame->setHtml(html);
    QWebElement element = frame->documentElement();
    //go inside <html> to <head>
    element = element.firstChild();
    //go the inside of the next sibiling of <head> (<body>)
    element = element.nextSibling().firstChild();

    while (!element.isNull()){
        QString name = element.tagName();
        if (name == P_TAG){
            //text tag
            if (element.toPlainText() != "")
                renderEngine->drawText(element.toPlainText());
        }else if (name == FORM_TAG){
            //form tag
            QString formAction = element.attribute(ACTION_ATTRIB);
            QString formName = element.attribute(NAME_ATTRIB);
            if (formName == "") formName = "form" + QString::number(formCounter);

            QWebElement e = element.firstChild();
            QWebElement submitElement;
            QStringList * parametersList = new QStringList();
            while (!e.isNull()){
                if (e.tagName() == INPUT_TAG){
                    if (e.attribute(TYPE_ATTRIB) == TEXT_ATTRIB_VAL){
                        renderEngine->drawTextBox(formName + "||" +e.attribute(NAME_ATTRIB));
                        parametersList->append(formName + "||" + e.attribute(NAME_ATTRIB));
                    } else if (e.attribute(TYPE_ATTRIB) == PASSWORD_ATTRIB_VAL){
                        renderEngine->drawPasswordTextBox(formName + "||" +e.attribute(NAME_ATTRIB));
                        parametersList->append(formName + "||" + e.attribute(NAME_ATTRIB));
                    } else if (e.attribute(TYPE_ATTRIB) == SUBMIT_ATTRIB_VAL){
                        submitElement = e;
                    } else if (e.attribute(TYPE_ATTRIB) == RADIO_ATTRIB_VAL) {
                        // radio code
                        renderEngine->drawRadioButton(formName + "|~" + e.attribute(NAME_ATTRIB), e.attribute(NAME_ATTRIB));
                        parametersList->append(formName + "|~" + e.attribute(NAME_ATTRIB));
                    } else if (e.attribute(TYPE_ATTRIB) == HIDDEN_ATTRIB_VAL) {
                        parametersList->append(formName + "|!" + e.attribute(NAME_ATTRIB) + "|!" + e.attribute(VALUE_ATTRIB));
                    }
                }else if (e.tagName() == LINK_TAG){
                    //anchor tag
                    QString label = element.toPlainText();
                    renderEngine->drawLink(element.attribute(HREF_ATTRIB),label);
                }else if (e.tagName() == P_TAG){
                    //text tag
                    if (e.toPlainText() != "")
                        renderEngine->drawText(e.toPlainText());
                }
                e = e.nextSibling();
            }
            if (!submitElement.isNull())
                renderEngine->drawSubmitButton(parametersList,formAction,submitElement.attribute(VALUE_ATTRIB));
            formCounter++;
        }else if (name == LINK_TAG){
            //anchor tag
            QString label = element.toPlainText();
            renderEngine->drawLink(element.attribute(HREF_ATTRIB),label);
        }
        element = element.nextSibling();
    }
}
