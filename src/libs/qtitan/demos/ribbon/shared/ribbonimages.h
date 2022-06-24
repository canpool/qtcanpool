#ifndef RIBBONIMAGES_H
#define RIBBONIMAGES_H


enum ImagesId
{
    Image_New,
    Image_Open,
    Image_Save,
    Image_SaveAs,
    Image_Clipboard,
    Image_Close,
    Image_Cut,
    Image_Undo,
    Image_Redo,
    Image_Print,
};

QIcon qtnIcon(ImagesId id, bool onlyLarge = false);
QIcon qtnIcon(const QString& smallIcon, const QString& largeIcon);

#endif // RIBBONIMAGES_H
