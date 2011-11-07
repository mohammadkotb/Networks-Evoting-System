#ifndef GUI_BUILDER_H
#define GUI_BUILDER_H

#include "gui_renderer.h"
#include <QtWebKit>

class GuiBuilder{

private:
    GuiRenderer * renderEngine;

public:
    GuiBuilder(GuiRenderer *);
    //build the gui from the given html string
    void build(QString &);

    //build the gui from the given html file path
    void buildFile(QString);

};
#endif // GUI_BUILDER_H
