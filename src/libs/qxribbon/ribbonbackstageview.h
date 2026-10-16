/**
 * Copyleft (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"

#include <QWidget>

class QAction;

QX_RIBBON_BEGIN_NAMESPACE

class RibbonBar;
class RibbonBackstageViewPrivate;

/**
 * @brief The full-takeover panel the application button opens, Office's backstage
 *
 * The backstage is what a ribbon application shows instead of its ribbon while the
 * user works on the document as a whole - save, open, print, options - rather than
 * on its contents. While it is open it covers the area the ribbon bar lives in,
 * which in the standard shell is the whole window: the ribbon, the pages and the
 * document all sit behind it.
 *
 * The panel is two parts. A column on the left holds the entries, each of them a
 * plain QToolButton: commands (@ref addAction), separators (@ref addSeparator) and
 * page entries (@ref addPage). The rest holds the active page, a widget of the
 * host's own; selecting a page entry shows its widget and hides the one before it,
 * which is how the panel carries the host's content without knowing anything about
 * it. The column starts with a back button that closes the panel, and so does Esc,
 * unless @ref setClosePrevented has been asked to keep it open.
 *
 * Nothing here paints: the panel is built from stock widgets that carry the
 * library's object names, so a theme styles it the way it styles the application
 * button. The view adds no visual semantics of its own.
 *
 * The view is constructed with the ribbon bar it belongs to and installs itself
 * over that bar's parent widget; it also opens when the bar's application button
 * is clicked, and closes again when the button is clicked a second time. A host
 * that wants different behaviour disconnects @ref RibbonBar::applicationButtonClicked.
 *
 * @note The area the view covers is the ribbon bar's parent widget, so a ribbon bar
 * embedded in a container rather than in a window gets a backstage over that
 * container, not over the whole window.
 */
class QX_RIBBON_EXPORT RibbonBackstageView : public QWidget
{
    Q_OBJECT
public:
    explicit RibbonBackstageView(RibbonBar *ribbonBar);
    virtual ~RibbonBackstageView();

    /**
     * @brief Whether Esc and the back button are refused
     *
     * A host that has something to ask first - a page with unsaved changes, say -
     * prevents the close, deals with what it has to deal with, and closes the
     * panel itself. The default is false.
     */
    bool isClosePrevented() const;
    void setClosePrevented(bool prevent);

    /**
     * @brief Adds a command entry to the column
     *
     * The returned action is owned by the view; connect to its @ref QAction::triggered
     * signal to run the command. Clicking the entry triggers the action.
     */
    QAction *addAction(const QIcon &icon, const QString &text);

    /// Adds a separator between two entries
    QAction *addSeparator();

    /**
     * @brief Adds a page entry and its content widget to the panel
     *
     * The widget is reparented to the view and starts hidden; it is shown while it
     * is the active page. The entry takes its text from the widget's window title,
     * so set that before adding; the entry is checkable and checked while its page
     * is the active one. Selecting the first page entry added does not happen on
     * its own - call @ref setActivePage for the page that should be up when the
     * panel opens.
     */
    QAction *addPage(QWidget *widget);

    /**
     * @brief Shows a page added with @ref addPage and hides the one before it
     *
     * A widget that is not one of the added pages is ignored. Removing the active
     * page's action leaves the panel with no active page until another one is
     * selected or the first remaining one is picked for it.
     */
    void setActivePage(QWidget *widget);

    /// The page currently shown, or nullptr when none is
    QWidget *activePage() const;

    /**
     * @brief The geometry of an entry's button within the view
     *
     * For a separator or an unknown action the rectangle is null.
     */
    QRect actionGeometry(QAction *action) const;

    // make the addAction of QWidget visible in the subclass
    using QWidget::addAction;

public Q_SLOTS:
    /// Shows the panel over its host; the panel takes the keyboard focus
    void open();
Q_SIGNALS:
    /// Emitted after the panel has been shown and laid out
    void aboutToShow();
    /// Emitted after the panel has been hidden
    void aboutToHide();
protected:
    virtual void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    virtual void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    virtual void hideEvent(QHideEvent *event) Q_DECL_OVERRIDE;
    virtual bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;
    virtual void actionEvent(QActionEvent *event) Q_DECL_OVERRIDE;
private:
    friend class RibbonBackstageViewPrivate;

    void requestClose();
private:
    QX_DECLARE_PRIVATE(RibbonBackstageView)
};

QX_RIBBON_END_NAMESPACE
