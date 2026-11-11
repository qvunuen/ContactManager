#include "MainWindow.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QStringList>
#include <string>

namespace {

std::string resolveDatabasePath() {
    QStringList candidates = {
        QStringLiteral("data"),
        QStringLiteral("../data")
    };

    // Walk up from executable dir to handle bin/, bin/<Config>/, and macOS .app bundles
    QDir probe(QCoreApplication::applicationDirPath());
    for (int depth = 0; depth < 4; ++depth) {
        if (!probe.cdUp()) {
            break;
        }
        candidates.append(probe.filePath(QStringLiteral("data")));
    }

    for (const QString& candidate : candidates) {
        const QFileInfo info(candidate);
        if (info.exists() && info.isDir()) {
            return QDir(info.absoluteFilePath()).filePath(QStringLiteral("contacts.db")).toStdString();
        }
    }

    const QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (!appDataPath.isEmpty()) {
        QDir().mkpath(appDataPath);
        return QDir(appDataPath).filePath(QStringLiteral("contacts.db")).toStdString();
    }

    return "contacts.db";
}

} // namespace

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("PhoneBook Manager");
    app.setApplicationVersion("1.0");

    MainWindow window(resolveDatabasePath());
    window.show();

    return app.exec();
}
