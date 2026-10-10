#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>
#include <QElapsedTimer>
#include <QPixmap>
#include <QStyle>
#include <QStyleFactory>
#include <QTimer>

// 重定向qdebug的打印
void log_out_put(QtMsgType type, const QMessageLogContext &context, const QString &msg);

/**
 * @brief 重定向qdebug的打印
 * @param type
 * @param context
 * @param msg
 */
void log_out_put(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();

    switch (type) {
    case QtDebugMsg:
        fprintf(stdout, "[Debug] %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtInfoMsg:
        fprintf(stdout, "[Info] %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        fprintf(stdout, "[Warning] %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line,
                context.function);
        break;
    case QtCriticalMsg:
        fprintf(stdout, "[Critical] %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line,
                context.function);
        break;
    case QtFatalMsg:
        fprintf(stdout, "[Fatal] %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
        break;
    default:
        fprintf(stdout, "[Debug] %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    }
#ifndef QT_NO_DEBUG_OUTPUT
    fflush(stdout);
#endif
}

int main(int argc, char *argv[])
{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
#ifdef Q_OS_WINDOWS
    // When tested on Windows11 and Qt6.8.1, the default style is windows11,
    // and the qss of ribbon does not work well, so for compatibility, set the style to windowsvista
    QApplication::setStyle("windowsvista");
    qDebug() << "Current style: " << QApplication::style()->objectName();
    qDebug() << "Supported styles: " << QStyleFactory::keys();
#endif
#endif

    QApplication a(argc, argv);
    qInstallMessageHandler(log_out_put);
    QFont f = a.font();

    f.setFamily("Microsoft YaHei");
    a.setFont(f);

    // "--screenshot <file>" turns the demo into a headless renderer. It is what
    // the visual regression test drives, and it is also the quickest way for a
    // human to capture what the ribbon looks like on this machine.
    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("RibbonDemo"));
    parser.addHelpOption();
    QCommandLineOption screenshotOption(
        QStringLiteral("screenshot"), QStringLiteral("Save a screenshot to <file> and exit."), QStringLiteral("file"));
    QCommandLineOption delayOption(QStringLiteral("screenshot-delay"),
                                   QStringLiteral("Wait <ms> after showing the window before "
                                                  "capturing."),
                                   QStringLiteral("ms"), QStringLiteral("1500"));
    parser.addOption(screenshotOption);
    parser.addOption(delayOption);

    // parse(), not process(): the demo is routinely started with Qt's own
    // switches (-platform, -style, ...), which the parser would reject as
    // unknown options and turn into a hard exit.
    parser.parse(QCoreApplication::arguments());
    if (parser.isSet(QStringLiteral("help"))) {
        parser.showHelp(0);
    }

    const QString screenshotFile = parser.value(screenshotOption);

    QElapsedTimer cost;

    cost.start();

    MainWindow w;

    if (screenshotFile.isEmpty()) {
        w.showMaximized();
    } else {
        // A fixed size keeps the capture comparable between runs: there is no
        // window manager to maximise it, and the golden image would otherwise
        // depend on whatever the offscreen screen happens to be.
        w.resize(1280, 800);
        w.show();
    }

    qDebug() << "window build cost:" << cost.elapsed() << " ms";

    if (!screenshotFile.isEmpty()) {
        const int delay = parser.value(delayOption).toInt();
        QTimer::singleShot(delay, &w, [&w, &a, screenshotFile]() {
            const QPixmap shot = w.grab();
            const bool saved = shot.save(screenshotFile);
            qInfo() << "screenshot:" << screenshotFile << shot.size() << (saved ? "ok" : "FAILED");
            a.exit(saved ? 0 : 1);
        });
    }

    return (a.exec());
}
