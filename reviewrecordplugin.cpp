#include "reviewrecordplugin.h"
#include "reviewrecordconstants.h"

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/editormanager/ieditor.h>

#include <QAction>
#include <QMessageBox>
#include <QMainWindow>
#include <QMenu>
#include <QDebug>
#include <QDir>
#include <QInputDialog>
#include <QLineEdit>

namespace ReviewRecord {
namespace Internal {

ReviewRecordPlugin::ReviewRecordPlugin()
{
    // Create your members
    recordFilePath = QDir::homePath() + "/Desktop/ReviewRecord.csv";
}

ReviewRecordPlugin::~ReviewRecordPlugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members
}

bool ReviewRecordPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    // Register objects in the plugin manager's object pool
    // Load settings
    // Add actions to menus
    // Connect to other plugins' signals
    // In the initialize function, a plugin can be sure that the plugins it
    // depends on have initialized their members.

    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    auto action = new QAction(tr("ReviewRecord Action"), this);
    Core::Command *cmd = Core::ActionManager::registerAction(action, Constants::ACTION_ID,
                                                             Core::Context(Core::Constants::C_GLOBAL));
    cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+Shift+A")));
    connect(action, &QAction::triggered, this, &ReviewRecordPlugin::triggerAction);

    Core::ActionContainer *menu = Core::ActionManager::createMenu(Constants::MENU_ID);
    menu->menu()->setTitle(tr("ReviewRecord"));
    menu->addAction(cmd);
    Core::ActionManager::actionContainer(Core::Constants::M_TOOLS)->addMenu(menu);

    return true;
}

void ReviewRecordPlugin::extensionsInitialized()
{
    // Retrieve objects from the plugin manager's object pool
    // In the extensionsInitialized function, a plugin can be sure that all
    // plugins that depend on it are completely initialized.
}

ExtensionSystem::IPlugin::ShutdownFlag ReviewRecordPlugin::aboutToShutdown()
{
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

void ReviewRecordPlugin::triggerAction()
{
    Core::IEditor *iEditor = Core::EditorManager::currentEditor();
    if (iEditor == Q_NULLPTR) {
        qCritical() << "iEditor is nullptr";
        return;
    }

    Core::IDocument *iDocument = Core::EditorManager::currentEditor()->document();
    if (iDocument == Q_NULLPTR) {
        qCritical() << "iDocument is nullptr";
        return;
    }

    bool ok = false;
    QString content = QInputDialog::getMultiLineText(nullptr, tr("code review"),
                                                  tr("error description:"), "", &ok);
    if (ok && content.isEmpty()) {
        qCritical() << "content is empty";
        return;
    }

    content.insert(0, "\"");
    content.append("\"");
    QString filename = iDocument->displayName();
    QString currentLine = QString::number(iEditor->currentLine());
    QString filepath = iDocument->filePath().toString();

    if (filename.isEmpty() || currentLine.isEmpty() || content.isEmpty() || filepath.isEmpty()){
        qCritical() << "parameter list: " << filename << ":" << currentLine << ":" << content << ":" << filepath;
        return;
    }
    writeLine(filename, currentLine, content, filepath);
}

QString ReviewRecordPlugin::createFile(const QString &filePath)
{
    QFile file(filePath);
    if (file.exists()) {
        return filePath;
    }

    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)){
        qCritical() << "file open failed,path:" << filePath;
        return QString();
    }

    QTextStream stream(&file);
    stream.seek(file.size());
    QString text;
    text.append("文件名,行数,问题描述,文件路径");
    text.remove("\n");
    stream << text << "\n";
    file.close();
    return filePath;
}

void ReviewRecordPlugin::writeLine(const QString &name, const QString &line, const QString &content, const QString &Path)
{
    QString path = createFile(recordFilePath);
    QFile file(path);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)){
        qCritical() << "file open failed,path:" << path;
        return ;
    }
    QTextStream stream(&file);
    stream.seek(file.size());

    QString text;
    text.append(name);
    text.append(",");
    text.append(line);
    text.append(",");
    text.append(content);
     text.append(",");
    text.append(Path);
//    text.remove("\n");
    stream << text << "\n";

    file.close();
}

} // namespace Internal
} // namespace ReviewRecord
