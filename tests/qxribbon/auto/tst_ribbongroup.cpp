#include "tst_global.h"

#include "qxribbon/ribbongroup.h"
#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbonpage.h"
#include "qxribbon/ribbonbutton.h"
#include "qxribbon/ribbongallery.h"

#include <QAction>
#include <QMenu>
#include <QWidget>
#include <QWidgetAction>
#include <QIcon>

QX_RIBBON_USE_NAMESPACE

class tst_RibbonGroup : public QObject
{
    Q_OBJECT
private slots:
    void group();
    void mode();
    void expanding();
    void height();
    void action();
    void menu();
    void widget();
    void option();
    void other();
};

void tst_RibbonGroup::group()
{
    // name
    RibbonGroup anonymousGrp;
    QCOMPARE(anonymousGrp.groupName().isEmpty(), true);
    anonymousGrp.setGroupName(tr("group1"));
    QCOMPARE(anonymousGrp.groupName(), tr("group1"));

    RibbonGroup grp(tr("group2"));
    QCOMPARE(grp.groupName(), tr("group2"));

    // title visible
    QCOMPARE(grp.titleVisible(), RibbonGroup::titleVisible());
    QCOMPARE(grp.titleVisible(), true);
    RibbonGroup::setTitleVisible(false);
    QCOMPARE(grp.titleVisible(), RibbonGroup::titleVisible());
    QCOMPARE(grp.titleVisible(), false);
    RibbonGroup::setTitleVisible(true);

    // custom
    QCOMPARE(grp.isCanCustomize(), true);
    grp.setCanCustomize(false);
    QCOMPARE(grp.isCanCustomize(), false);
}

void tst_RibbonGroup::mode()
{
    RibbonGroup grp(tr("mode"));

    // mode
    QCOMPARE(grp.groupLayoutMode(), RibbonGroup::ThreeRowMode);
    QCOMPARE(grp.isTwoRow(), false);

    // set mode
    RibbonBar rb;
    RibbonPage *page = rb.addPage(tr("page"));
    RibbonGroup *rg = page->addGroup(tr("group"));

    QCOMPARE(rb.currentRibbonStyle(), RibbonBar::OfficeStyle);
    QCOMPARE(rg->groupLayoutMode(), RibbonGroup::ThreeRowMode);
    QCOMPARE(rg->isTwoRow(), false);
    QCOMPARE(rg->optionActionButtonSize(), QSize(16, 16));

    rb.setRibbonStyle(RibbonBar::OfficeStyleTwoRow);

    QCOMPARE(rb.currentRibbonStyle(), RibbonBar::OfficeStyleTwoRow);
    QCOMPARE(rg->groupLayoutMode(), RibbonGroup::TwoRowMode);
    QCOMPARE(rg->isTwoRow(), true);
    QCOMPARE(rg->optionActionButtonSize(), QSize(12, 12));

    rb.setRibbonStyle(RibbonBar::WpsLiteStyle);

    QCOMPARE(rb.currentRibbonStyle(), RibbonBar::WpsLiteStyle);
    QCOMPARE(rg->groupLayoutMode(), RibbonGroup::ThreeRowMode);
    QCOMPARE(rg->isTwoRow(), false);
    QCOMPARE(rg->optionActionButtonSize(), QSize(16, 16));

    rb.setRibbonStyle(RibbonBar::WpsLiteStyleTwoRow);

    QCOMPARE(rb.currentRibbonStyle(), RibbonBar::WpsLiteStyleTwoRow);
    QCOMPARE(rg->groupLayoutMode(), RibbonGroup::TwoRowMode);
    QCOMPARE(rg->isTwoRow(), true);
    QCOMPARE(rg->optionActionButtonSize(), QSize(12, 12));
}

void tst_RibbonGroup::expanding()
{
    RibbonGroup grp;

    QCOMPARE(grp.isExpanding(), false);
    QCOMPARE(grp.sizePolicy().horizontalPolicy(), QSizePolicy::Preferred);
    QCOMPARE(grp.sizePolicy().verticalPolicy(), QSizePolicy::Preferred);

    grp.setExpanding(true);
    QCOMPARE(grp.isExpanding(), true);
    QCOMPARE(grp.sizePolicy().horizontalPolicy(), QSizePolicy::Expanding);
    QCOMPARE(grp.sizePolicy().verticalPolicy(), QSizePolicy::Fixed);

    grp.setExpanding(false);
    QCOMPARE(grp.isExpanding(), false);
    QCOMPARE(grp.sizePolicy().horizontalPolicy(), QSizePolicy::Preferred);
    QCOMPARE(grp.sizePolicy().verticalPolicy(), QSizePolicy::Fixed);

    grp.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    QCOMPARE(grp.isExpanding(), false);
    grp.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    QCOMPARE(grp.isExpanding(), true);
}

void tst_RibbonGroup::height()
{
    RibbonGroup grp;

    QCOMPARE(grp.titleVisible(), true);

    // title height
    QCOMPARE(grp.groupTitleHeight(), RibbonGroup::groupTitleHeight());
    QCOMPARE(grp.groupTitleHeight(), 21);
    QCOMPARE(grp.titleHeight(), grp.groupTitleHeight());
    RibbonGroup::setGroupTitleHeight(25);
    QCOMPARE(grp.groupTitleHeight(), RibbonGroup::groupTitleHeight());
    QCOMPARE(grp.groupTitleHeight(), 25);
    QCOMPARE(grp.titleHeight(), grp.groupTitleHeight());

    int grpHeight = grp.height() - 1 - 1; // top, bottom margins
    QCOMPARE(grp.largeHeight(), grpHeight - grp.titleHeight());

    RibbonGroup::setTitleVisible(false);
    QCOMPARE(grp.titleVisible(), false);
    QCOMPARE(grp.titleHeight(), 0);
    QCOMPARE(grp.largeHeight(), grpHeight);
}

void tst_RibbonGroup::action()
{
    // addAction return RibbonButton
    {
        RibbonGroup grp;

        QAction *a1 = new QAction(tr("action1"));
        RibbonButton *btn1 = grp.addAction(a1, RibbonGroup::Auto);
        QAction *a2 = new QAction(tr("action2"));
        RibbonButton *btn2 = grp.addAction(a2, RibbonGroup::Large);
        QAction *a3 = new QAction(tr("action3"));
        RibbonButton *btn3 = grp.addAction(a3, RibbonGroup::Medium);
        QAction *a4 = new QAction(tr("action4"));
        RibbonButton *btn4 = grp.addAction(a4, RibbonGroup::Small);

        QCOMPARE(btn1->buttonType(), RibbonButton::SmallButton);
        QCOMPARE(btn2->buttonType(), RibbonButton::LargeButton);
        QCOMPARE(btn3->buttonType(), RibbonButton::SmallButton);
        QCOMPARE(btn4->buttonType(), RibbonButton::SmallButton);

        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a1), RibbonGroup::Auto);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a2), RibbonGroup::Large);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a3), RibbonGroup::Medium);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a4), RibbonGroup::Small);

        QCOMPARE(grp.ribbonButtonForAction(a4), btn4);

        QString titles = "";
        for (RibbonButton *btn : grp.ribbonButtons()) {
            titles += btn->text() + ",";
        }
        QCOMPARE("action1,action2,action3,action4,", titles);

        QCOMPARE(grp.actionIndex(a1), 0);
        QCOMPARE(grp.actionIndex(a4), 3);
        QCOMPARE(grp.actionIndex(nullptr), -1);

        grp.moveAction(3, 1);
        QCOMPARE(grp.actionIndex(a2), 2);
        QCOMPARE(grp.actionIndex(a4), 1);
        QCOMPARE(grp.ribbonButtonForAction(a2), btn2);
        titles = "";
        for (RibbonButton *btn : grp.ribbonButtons()) {
            titles += btn->text() + ",";
        }
        // Moving the index does not affect the ribbonbutton order
        QCOMPARE("action1,action2,action3,action4,", titles);

        grp.moveAction(1, 3);
        QCOMPARE(grp.actionIndex(a2), 1);
        QCOMPARE(grp.actionIndex(a4), 3);
        QCOMPARE(grp.ribbonButtonForAction(a4), btn4);
        titles = "";
        for (RibbonButton *btn : grp.ribbonButtons()) {
            titles += btn->text() + ",";
        }
        QCOMPARE("action1,action2,action3,action4,", titles);
    }

    // addLargeAction, addMediumAction, addSmallAction
    {
        RibbonGroup grp;

        QAction *a1 = new QAction(tr("action1"));
        RibbonButton *btn1 = grp.addLargeAction(a1);
        QAction *a2 = new QAction(tr("action2"));
        RibbonButton *btn2 = grp.addMediumAction(a2);
        QAction *a3 = new QAction(tr("action3"));
        RibbonButton *btn3 = grp.addSmallAction(a3);

        QCOMPARE(btn1->buttonType(), RibbonButton::LargeButton);
        QCOMPARE(btn2->buttonType(), RibbonButton::SmallButton);
        QCOMPARE(btn3->buttonType(), RibbonButton::SmallButton);

        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a1), RibbonGroup::Large);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a2), RibbonGroup::Medium);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a3), RibbonGroup::Small);

        QCOMPARE(grp.actionIndex(a1), 0);
        QCOMPARE(grp.actionIndex(a3), 2);
    }

    // addAction + ribbonButtonForAction
    {
        RibbonGroup grp;

        QAction *a1 = new QAction(tr("action1"));
        grp.addAction(a1, QToolButton::DelayedPopup);
        RibbonButton *btn1 = grp.ribbonButtonForAction(a1);
        QAction *a2 = new QAction(tr("action2"));
        grp.addAction(a2, QToolButton::MenuButtonPopup);
        RibbonButton *btn2 = grp.ribbonButtonForAction(a2);
        QAction *a3 = new QAction(tr("action3"));
        grp.addAction(a3, QToolButton::InstantPopup);
        RibbonButton *btn3 = grp.ribbonButtonForAction(a3);
        QAction *a4 = new QAction(tr("action4"));
        grp.addAction(a4, QToolButton::DelayedPopup, RibbonGroup::Auto);
        RibbonButton *btn4 = grp.ribbonButtonForAction(a4);
        QAction *a5 = new QAction(tr("action5"));
        grp.addAction(a5, QToolButton::DelayedPopup, RibbonGroup::Medium);
        RibbonButton *btn5 = grp.ribbonButtonForAction(a5);
        QAction *a6 = new QAction(tr("action6"));
        grp.addAction(a6, QToolButton::DelayedPopup, RibbonGroup::Small);
        RibbonButton *btn6 = grp.ribbonButtonForAction(a6);

        QCOMPARE(btn1->buttonType(), RibbonButton::LargeButton);
        QCOMPARE(btn2->buttonType(), RibbonButton::LargeButton);
        QCOMPARE(btn3->buttonType(), RibbonButton::LargeButton);
        QCOMPARE(btn4->buttonType(), RibbonButton::SmallButton);
        QCOMPARE(btn5->buttonType(), RibbonButton::SmallButton);
        QCOMPARE(btn6->buttonType(), RibbonButton::SmallButton);

        QCOMPARE(btn2->popupMode(), QToolButton::MenuButtonPopup);
        QCOMPARE(btn5->popupMode(), QToolButton::DelayedPopup);

        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a1), RibbonGroup::Large);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a2), RibbonGroup::Large);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a3), RibbonGroup::Large);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a4), RibbonGroup::Auto);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a5), RibbonGroup::Medium);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a6), RibbonGroup::Small);

        QCOMPARE(grp.actionIndex(a1), 0);
        QCOMPARE(grp.actionIndex(a6), 5);
    }

    // addAction return action
    {
        RibbonGroup grp;

        QAction *a1 = grp.addAction(tr("action1"), QIcon(), QToolButton::DelayedPopup);
        RibbonButton *btn1 = grp.ribbonButtonForAction(a1);
        QAction *a2 = grp.addAction(tr("action2"), QIcon(), QToolButton::MenuButtonPopup);
        RibbonButton *btn2 = grp.ribbonButtonForAction(a2);
        QAction *a3 = grp.addAction(tr("action3"), QIcon(), QToolButton::InstantPopup);
        RibbonButton *btn3 = grp.ribbonButtonForAction(a3);
        QAction *a4 = grp.addAction(tr("action4"), QIcon(), QToolButton::DelayedPopup, RibbonGroup::Auto);
        RibbonButton *btn4 = grp.ribbonButtonForAction(a4);
        QAction *a5 = grp.addAction(tr("action5"), QIcon(), QToolButton::DelayedPopup, RibbonGroup::Medium);
        RibbonButton *btn5 = grp.ribbonButtonForAction(a5);
        QAction *a6 = grp.addAction(tr("action6"), QIcon(), QToolButton::DelayedPopup, RibbonGroup::Small);
        RibbonButton *btn6 = grp.ribbonButtonForAction(a6);

        QCOMPARE(btn1->buttonType(), RibbonButton::LargeButton);
        QCOMPARE(btn2->buttonType(), RibbonButton::LargeButton);
        QCOMPARE(btn3->buttonType(), RibbonButton::LargeButton);
        QCOMPARE(btn4->buttonType(), RibbonButton::SmallButton);
        QCOMPARE(btn5->buttonType(), RibbonButton::SmallButton);
        QCOMPARE(btn6->buttonType(), RibbonButton::SmallButton);

        QCOMPARE(btn2->popupMode(), QToolButton::MenuButtonPopup);
        QCOMPARE(btn5->popupMode(), QToolButton::DelayedPopup);

        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a1), RibbonGroup::Large);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a2), RibbonGroup::Large);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a3), RibbonGroup::Large);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a4), RibbonGroup::Auto);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a5), RibbonGroup::Medium);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a6), RibbonGroup::Small);

        QCOMPARE(grp.actionIndex(a1), 0);
        QCOMPARE(grp.actionIndex(a6), 5);
    }
}

void tst_RibbonGroup::menu()
{
    // addMenu return RibbonButton
    {
        RibbonGroup grp;

        QMenu *m1 = new QMenu(tr("menu1"));
        RibbonButton *btn1 = grp.addMenu(m1, RibbonGroup::Auto);
        QMenu *m2 = new QMenu(tr("menu2"));
        RibbonButton *btn2 = grp.addMenu(m2, RibbonGroup::Large);
        QMenu *m3 = new QMenu(tr("menu3"));
        RibbonButton *btn3 = grp.addMenu(m3, RibbonGroup::Medium);
        QMenu *m4 = new QMenu(tr("menu4"));
        RibbonButton *btn4 = grp.addMenu(m4, RibbonGroup::Small);
        QMenu *m5 = new QMenu(tr("menu5"));
        RibbonButton *btn5 = grp.addMenu(m5, RibbonGroup::Small, QToolButton::MenuButtonPopup);
        QMenu *m6 = new QMenu(tr("menu6"));
        RibbonButton *btn6 = grp.addMenu(m6, RibbonGroup::Small, QToolButton::DelayedPopup);

        QCOMPARE(btn1->buttonType(), RibbonButton::SmallButton);
        QCOMPARE(btn2->buttonType(), RibbonButton::LargeButton);
        QCOMPARE(btn3->buttonType(), RibbonButton::SmallButton);
        QCOMPARE(btn4->buttonType(), RibbonButton::SmallButton);
        QCOMPARE(btn5->buttonType(), RibbonButton::SmallButton);
        QCOMPARE(btn6->buttonType(), RibbonButton::SmallButton);

        QCOMPARE(btn1->popupMode(), QToolButton::InstantPopup);
        QCOMPARE(btn5->popupMode(), QToolButton::MenuButtonPopup);
        QCOMPARE(btn6->popupMode(), QToolButton::DelayedPopup);

        QCOMPARE(RibbonGroup::getActionRowProportionProperty(m1->menuAction()), RibbonGroup::Auto);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(m2->menuAction()), RibbonGroup::Large);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(m3->menuAction()), RibbonGroup::Medium);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(m4->menuAction()), RibbonGroup::Small);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(m5->menuAction()), RibbonGroup::Small);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(m6->menuAction()), RibbonGroup::Small);

        QCOMPARE(grp.ribbonButtonForAction(m1->menuAction()), btn1);

        QString titles = "";
        for (RibbonButton *btn : grp.ribbonButtons()) {
            titles += btn->text() + ",";
        }
        QCOMPARE("menu1,menu2,menu3,menu4,menu5,menu6,", titles);

        QCOMPARE(grp.actionIndex(m1->menuAction()), 0);
        QCOMPARE(grp.actionIndex(m6->menuAction()), 5);
    }

    // addLargeMenu, addSmallMenu
    {
        RibbonGroup grp;

        QMenu *m1 = new QMenu(tr("menu1"));
        RibbonButton *btn1 = grp.addLargeMenu(m1);
        QMenu *m2 = new QMenu(tr("menu2"));
        RibbonButton *btn2 = grp.addLargeMenu(m2, QToolButton::MenuButtonPopup);
        QMenu *m3 = new QMenu(tr("menu3"));
        RibbonButton *btn3 = grp.addLargeMenu(m3, QToolButton::DelayedPopup);
        QMenu *m4 = new QMenu(tr("menu4"));
        RibbonButton *btn4 = grp.addSmallMenu(m4);
        QMenu *m5 = new QMenu(tr("menu5"));
        RibbonButton *btn5 = grp.addSmallMenu(m5, QToolButton::MenuButtonPopup);
        QMenu *m6 = new QMenu(tr("menu6"));
        RibbonButton *btn6 = grp.addSmallMenu(m6, QToolButton::DelayedPopup);

        QCOMPARE(btn1->buttonType(), RibbonButton::LargeButton);
        QCOMPARE(btn2->buttonType(), RibbonButton::LargeButton);
        QCOMPARE(btn3->buttonType(), RibbonButton::LargeButton);
        QCOMPARE(btn4->buttonType(), RibbonButton::SmallButton);
        QCOMPARE(btn5->buttonType(), RibbonButton::SmallButton);
        QCOMPARE(btn6->buttonType(), RibbonButton::SmallButton);

        QCOMPARE(btn1->popupMode(), QToolButton::InstantPopup);
        QCOMPARE(btn2->popupMode(), QToolButton::MenuButtonPopup);
        QCOMPARE(btn3->popupMode(), QToolButton::DelayedPopup);
        QCOMPARE(btn4->popupMode(), QToolButton::InstantPopup);
        QCOMPARE(btn5->popupMode(), QToolButton::MenuButtonPopup);
        QCOMPARE(btn6->popupMode(), QToolButton::DelayedPopup);

        QCOMPARE(RibbonGroup::getActionRowProportionProperty(m1->menuAction()), RibbonGroup::Large);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(m2->menuAction()), RibbonGroup::Large);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(m3->menuAction()), RibbonGroup::Large);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(m4->menuAction()), RibbonGroup::Small);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(m5->menuAction()), RibbonGroup::Small);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(m6->menuAction()), RibbonGroup::Small);

        QCOMPARE(grp.ribbonButtonForAction(m1->menuAction()), btn1);

        QString titles = "";
        for (RibbonButton *btn : grp.ribbonButtons()) {
            titles += btn->text() + ",";
        }
        QCOMPARE("menu1,menu2,menu3,menu4,menu5,menu6,", titles);

        QCOMPARE(grp.actionIndex(m1->menuAction()), 0);
        QCOMPARE(grp.actionIndex(m6->menuAction()), 5);
    }

    // addActionMenu
    {
        RibbonGroup grp;

        QAction *a1 = new QAction(tr("action1"));
        QMenu *m1 = new QMenu(tr("menu1"));
        RibbonButton *btn1 = grp.addActionMenu(a1, m1, RibbonGroup::Auto);
        QAction *a2 = new QAction(tr("action2"));
        QMenu *m2 = new QMenu(tr("menu2"));
        RibbonButton *btn2 = grp.addActionMenu(a2, m2, RibbonGroup::Large);
        QAction *a3 = new QAction(tr("action3"));
        QMenu *m3 = new QMenu(tr("menu3"));
        RibbonButton *btn3 = grp.addActionMenu(a3, m3, RibbonGroup::Medium);
        QAction *a4 = new QAction(tr("action4"));
        QMenu *m4 = new QMenu(tr("menu4"));
        RibbonButton *btn4 = grp.addActionMenu(a4, m4, RibbonGroup::Small);
        QAction *a5 = new QAction(tr("action5"));
        QMenu *m5 = new QMenu(tr("menu5"));
        RibbonButton *btn5 = grp.addLargeActionMenu(a5, m5);

        QCOMPARE(btn1->menu(), m1);
        QCOMPARE(btn2->menu(), m2);
        QCOMPARE(btn3->menu(), m3);
        QCOMPARE(btn4->menu(), m4);
        QCOMPARE(btn5->menu(), m5);

        QCOMPARE(btn1->defaultAction(), a1);
        QCOMPARE(btn2->defaultAction(), a2);
        QCOMPARE(btn3->defaultAction(), a3);
        QCOMPARE(btn4->defaultAction(), a4);
        QCOMPARE(btn5->defaultAction(), a5);

        QCOMPARE(btn1->buttonType(), RibbonButton::SmallButton);
        QCOMPARE(btn2->buttonType(), RibbonButton::LargeButton);
        QCOMPARE(btn3->buttonType(), RibbonButton::SmallButton);
        QCOMPARE(btn4->buttonType(), RibbonButton::SmallButton);
        QCOMPARE(btn5->buttonType(), RibbonButton::LargeButton);

        QCOMPARE(btn1->popupMode(), QToolButton::MenuButtonPopup);
        QCOMPARE(btn5->popupMode(), QToolButton::MenuButtonPopup);

        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a1), RibbonGroup::Auto);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a2), RibbonGroup::Large);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a3), RibbonGroup::Medium);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a4), RibbonGroup::Small);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a5), RibbonGroup::Large);

        QCOMPARE(grp.ribbonButtonForAction(a1), btn1);

        QString titles = "";
        for (RibbonButton *btn : grp.ribbonButtons()) {
            titles += btn->text() + ",";
        }
        QCOMPARE("action1,action2,action3,action4,action5,", titles);

        QCOMPARE(grp.actionIndex(a1), 0);
        QCOMPARE(grp.actionIndex(a5), 4);
    }
}

void tst_RibbonGroup::widget()
{
    // addWidget return QAction
    {
        RibbonGroup grp;

        QWidget *w1 = new QWidget();
        QAction *a1 = grp.addWidget(w1, RibbonGroup::Auto);
        a1->setText(tr("action1"));
        QWidget *w2 = new QWidget();
        QAction *a2 = grp.addWidget(w2, RibbonGroup::Large);
        a2->setText(tr("action2"));
        QWidget *w3 = new QWidget();
        QAction *a3 = grp.addWidget(w3, RibbonGroup::Medium);
        a3->setText(tr("action3"));
        QWidget *w4 = new QWidget();
        QAction *a4 = grp.addWidget(w4, RibbonGroup::Small);
        a4->setText(tr("action4"));

        QCOMPARE(qobject_cast<QWidgetAction *>(a1)->defaultWidget(), w1);
        QCOMPARE(qobject_cast<QWidgetAction *>(a2)->defaultWidget(), w2);
        QCOMPARE(qobject_cast<QWidgetAction *>(a3)->defaultWidget(), w3);
        QCOMPARE(qobject_cast<QWidgetAction *>(a4)->defaultWidget(), w4);

        QCOMPARE(grp.ribbonButtonForAction(a1), nullptr);
        QCOMPARE(grp.ribbonButtonForAction(a2), nullptr);
        QCOMPARE(grp.ribbonButtonForAction(a3), nullptr);
        QCOMPARE(grp.ribbonButtonForAction(a4), nullptr);

        QCOMPARE(grp.ribbonButtons().count(), 0);

        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a1), RibbonGroup::Auto);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a2), RibbonGroup::Large);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a3), RibbonGroup::Medium);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a4), RibbonGroup::Small);

        QCOMPARE(grp.actionIndex(a1), 0);
        QCOMPARE(grp.actionIndex(a4), 3);
    }

    // addLargeWidget, addMediumWidget, addSmallWidget
    {
        RibbonGroup grp;

        QWidget *w1 = new QWidget();
        QAction *a1 = grp.addLargeWidget(w1);
        QWidget *w2 = new QWidget();
        QAction *a2 = grp.addMediumWidget(w2);
        QWidget *w3 = new QWidget();
        QAction *a3 = grp.addSmallWidget(w3);

        QCOMPARE(qobject_cast<QWidgetAction *>(a1)->defaultWidget(), w1);
        QCOMPARE(qobject_cast<QWidgetAction *>(a2)->defaultWidget(), w2);
        QCOMPARE(qobject_cast<QWidgetAction *>(a3)->defaultWidget(), w3);

        QCOMPARE(grp.ribbonButtonForAction(a1), nullptr);
        QCOMPARE(grp.ribbonButtonForAction(a2), nullptr);
        QCOMPARE(grp.ribbonButtonForAction(a3), nullptr);

        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a1), RibbonGroup::Large);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a2), RibbonGroup::Medium);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a3), RibbonGroup::Small);

        QCOMPARE(grp.actionIndex(a1), 0);
        QCOMPARE(grp.actionIndex(a3), 2);
    }
}

void tst_RibbonGroup::option()
{
    RibbonGroup grp;

    QCOMPARE(grp.hasOptionAction(), false);

    QAction *a1 = new QAction(tr("action1"));
    grp.addOptionAction(a1);
    QCOMPARE(grp.hasOptionAction(), true);

    grp.addOptionAction(nullptr);
    QCOMPARE(grp.hasOptionAction(), false);

    grp.addOptionAction(a1);
    QCOMPARE(grp.hasOptionAction(), true);
}

void tst_RibbonGroup::other()
{
    // gallery
    {
        RibbonGroup grp;

        QCOMPARE(grp.isExpanding(), false);

        RibbonGallery *gallery = grp.addGallery();
        QCOMPARE(gallery->parentWidget(), &grp);

        QCOMPARE(grp.isExpanding(), true);
    }

    // separator
    {
        RibbonGroup grp;

        QAction *a1 = grp.addSeparator();
        QAction *a2 = grp.addSeparator();

        QCOMPARE(a1->parentWidget(), &grp);
        QCOMPARE(a2->isSeparator(), true);

        QCOMPARE(grp.ribbonButtonForAction(a1), nullptr);

        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a1), RibbonGroup::Large);
        QCOMPARE(RibbonGroup::getActionRowProportionProperty(a2), RibbonGroup::Large);
    }
}

TEST_ADD(tst_RibbonGroup)

#include "tst_ribbongroup.moc"
