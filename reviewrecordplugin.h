#ifndef REVIEWRECORD_H
#define REVIEWRECORD_H

#include "reviewrecord_global.h"

#include <QFile>
#include <QString>

#include <extensionsystem/iplugin.h>

namespace ReviewRecord {
namespace Internal {

class ReviewRecordPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "ReviewRecord.json")

public:
    ReviewRecordPlugin();
    ~ReviewRecordPlugin() override;

    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override;
    ShutdownFlag aboutToShutdown() override;

private:
    void triggerAction();
    QString createFile(const QString &filePath);
    void writeLine(const QString &name, const QString &ine, const QString &content, const QString &Path);

private:
    QString recordFilePath;
};

} // namespace Internal
} // namespace ReviewRecord

#endif // REVIEWRECORD_H
