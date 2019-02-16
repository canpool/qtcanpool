#ifndef CHILDINTERFACE_H
#define CHILDINTERFACE_H

class QStatusBar;
namespace Qtitan 
{
    class RibbonBar; 
    class RibbonGroup;
    class RibbonPage;
}


/* ChildInterface */
class ChildInterface
{
public:
    virtual ~ChildInterface() {}
    enum TypeDocument
    {
        td_None,
        td_Text,
        td_Sketch,
    };
public:
    virtual TypeDocument typeDocument() const = 0;
    virtual void buildUserInterface(Qtitan::RibbonBar* pRib, QStatusBar* pStatusBar, Qtitan::RibbonPage* pageHome) = 0;
    virtual void removeUserInterface(Qtitan::RibbonBar* pRib, QStatusBar* pStatusBar, Qtitan::RibbonPage* pageHome) = 0;

    virtual bool save() = 0;
    virtual bool saveAs() = 0;

    virtual QString userFriendlyCurrentFile() = 0;
    virtual QString currentFile() const = 0;

    virtual bool hasSelection() const = 0;
};
Q_DECLARE_INTERFACE(ChildInterface, "com.qtitan.ribbonmdisample.ChildInterface")



#endif // INTERFACECHILD_H
