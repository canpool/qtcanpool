#include <qglobal.h>
#include <QIcon>

#include "ribbonimages.h"

QIcon qtnIcon(ImagesId id, bool onlyLarge)
{
    QIcon icon;
    switch (id)
    {
        case Image_New:
                icon.addPixmap(QPixmap(":/res/new32x32.png"));
            break;
        case Image_Open:
                if (!onlyLarge)
                    icon.addPixmap(QPixmap(":/res/open16x16.png"));
                icon.addPixmap(QPixmap(":/res/open32x32.png"));
            break;
        case Image_Save:
            if (!onlyLarge)
                icon.addPixmap(QPixmap(":/res/save16x16.png"));
            icon.addPixmap(QPixmap(":/res/save32x32.png"));
            break;
        case Image_SaveAs:
                if (!onlyLarge)
                    icon.addPixmap(QPixmap(":/res/saveasfile16x16.png"));
                icon.addPixmap(QPixmap(":/res/saveasfile32x32.png"));
            break;
        case Image_Clipboard:
                if (!onlyLarge)
                    icon.addPixmap(QPixmap(":/res/clipboard16x16.png"));
                icon.addPixmap(QPixmap(":/res/clipboard32x32.png"));
            break;
        case Image_Close:
                if (!onlyLarge)
                    icon.addPixmap(QPixmap(":/res/close16x16.png"));
                icon.addPixmap(QPixmap(":/res/close32x32.png"));
            break;
        case Image_Cut:
                icon.addPixmap(QPixmap(":/res/cut16x16.png"));
            break;
        case Image_Undo :
                icon.addPixmap(QPixmap(":/res/undo16x16.png"));
            break;
        case Image_Redo :
                icon.addPixmap(QPixmap(":/res/redo16x16.png"));
            break;
        case Image_Print :
                if (!onlyLarge)
                    icon.addPixmap(QPixmap(":/res/print16x16.png"));
                icon.addPixmap(QPixmap(":/res/print32x32.png"));
            break;

        default:
                Q_ASSERT(false);
            break;
    }
    return icon;
}

QIcon qtnIcon(const QString& smallIcon, const QString& largeIcon)
{
  QIcon icon;
  if (!smallIcon.isEmpty())
    icon.addPixmap(QPixmap(smallIcon));
  if (!largeIcon.isEmpty())
    icon.addPixmap(QPixmap(largeIcon));
  return icon;
}
