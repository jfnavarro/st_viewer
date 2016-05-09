#ifndef SETTINGSSTYLE_H
#define SETTINGSSTYLE_H

#include <QString>
#include <QSizeF>
#include <QColor>

// TODO The style configuration could be loaded from a QSS file
namespace Style
{

/** STYLE VARIABLES **/

static const QString BUTTON_STYLE_INIT_PAGE
    = "{background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, "
      "                                   stop:0 rgba(245, 245, 245, 255), "
      "                                   stop:1 rgba(255, 255, 255, 255)); "
      " border: 1px solid rgb(209, 209, 209); "
      " border-radius: 5px;}";

static const QString GENERAL_STYLE
    = "QTableView {alternate-background-color: rgb(245,245,245); "
      "            background-color: transparent; "
      "            selection-background-color: rgb(215,215,215); "
      "            selection-color: rgb(238,122,0); "
      "            gridline-color: rgb(240,240,240);"
      "            border: 1px solid rgb(240,240,240);} "
      "QTableView::indicator:unchecked {image: url(:/images/unchecked-box.png);} "
      "QTableView::indicator:checked {image: url(:/images/checked-box.png);} "
      "QTableView::indicator {padding-left: 10px; "
      "                       width: 15px; "
      "                       height: 15px; "
      "                       background-color: transparent;} "
      "QPushButton:focus:pressed {background-color: transparent; border: none;} "
      "QPushButton:pressed {background-color: transparent; border: none;} "
      "QPushButton:flat {background-color: transparent; border: none;} "
      "QHeaderView::section {height: 35px; "
      "                      padding-left: 4px; "
      "                      padding-right: 2px; "
      "                      spacing: 5px;"
      "                      background-color: rgb(230,230,230); "
      "                      border: 1px solid rgb(240,240,240);} "
      "QTableCornerButton::section {background-color: transparent;} "
      "QLineEdit {border: 1px solid rgb(209,209,209); "
      "           border-radius: 5px; "
      "           background-color: rgb(255,255,255); "
      "           selection-background-color: darkgray;} "
      "QToolButton {border: none;} "
      "QRadioButton::indicator::unchecked {border: 1px solid darkgray; "
      "                                    border-radius: 6px; "
      "                                    background-color: white; "
      "                                    width: 10px; "
      "                                    height: 10px; "
      "                                    margin-left: 5px;}"
      "QRadioButton::indicator::checked {border: 1px solid darkgray; "
      "                                  border-radius: 6px; "
      "                                  background-color: rgb(238,122,0); "
      "                                  width: 10px; "
      "                                  height: 10px; "
      "                                  margin-left: 5px;} "
      "QComboBox::item::selected {background: rgb(238,122,0);} "
      "QMenu::item:selected {background-color: rgb(238,122,0); }";
static const QString PAGE_WIDGETS_STYLE = "{background-color: rgb(240,240,240);}";

static const QString PAGE_FRAME_STYLE = "{background-color: rgb(230,230,230); "
                                        " border-color: rgb(206,202,202);}";

static const int CELL_PAGE_SUB_MENU_BUTTON_SPACE = 15;
static const QSize CELL_PAGE_SUB_MENU_BUTTON_SIZE = QSize(40, 30);
static const QSize CELL_PAGE_SUB_MENU_ICON_SIZE = QSize(40, 30);
static const QSize CELL_PAGE_SUB_MENU_LINE_EDIT_SIZE = QSize(150, 25);
static const QString CELL_PAGE_SUB_MENU_BUTTON_STYLE
    = "QPushButton {border: 1px solid rgb(209, 209, 209); "
      "             border-radius: 5px; "
      "             background-color: rgb(255, 255, 255);}";
static const QString CELL_PAGE_SUB_MENU_LINE_EDIT_STYLE
    = "QLineEdit {border: 1px solid rgb(209, 209, 209); "
      "           border-radius: 5px; "
      "           background-color: rgb(255, 255, 255); "
      "           selection-background-color: darkgray;}";
}
#endif // SETTINGSSTYLE_H
