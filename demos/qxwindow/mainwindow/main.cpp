#include <QtWidgets/QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[]) {
    qputenv("QT_WIN_DEBUG_CONSOLE", "attach");
    qputenv("QSG_INFO", "1");
#if 0
    qputenv("QT_WIDGETS_RHI", "1");
    qputenv("QSG_RHI_BACKEND", "d3d12");
    qputenv("QSG_RHI_HDR", "scrgb");
    qputenv("QT_QPA_DISABLE_REDIRECTION_SURFACE", "1");
#endif

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
#endif

    QApplication a(argc, argv);

#if 0 && defined(Q_OS_WINDOWS) && QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QApplication::setFont([]() {
        QFont f("Microsoft YaHei");
        f.setStyleStrategy(QFont::PreferAntialias);
        f.setPixelSize(15);
        return f;
    }());
#endif

    MainWindow w;
    w.show();
    return a.exec();
}
