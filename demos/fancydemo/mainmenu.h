#ifndef MAINMENU_H
#define MAINMENU_H

#include <QMenu>

class MainMenu : public QMenu
{
    Q_OBJECT
public:
    explicit MainMenu(QWidget *parent = 0);
    ~MainMenu();

private:

private slots:
    void slotOption();
    void slotHelp();
    void slotAbout();

};

#endif // MAINMENU_H
