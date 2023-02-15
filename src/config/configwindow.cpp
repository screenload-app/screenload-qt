// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2017-2019 Alejandro Sirgo Rica & Contributors

#include "configwindow.h"
#include "abstractlogger.h"
#include "src/config/configresolver.h"
#include "src/config/filenameeditor.h"
#include "src/config/generalconf.h"
#include "src/config/shortcutswidget.h"
#include "src/config/strftimechooserwidget.h"
#include "src/config/visualseditor.h"
#include "src/utils/colorutils.h"
#include "src/utils/confighandler.h"
#include "src/utils/globalvalues.h"
#include "src/utils/pathinfo.h"
#include <QApplication>
#include <QDialogButtonBox>
#include <QFileSystemWatcher>
#include <QIcon>
#include <QKeyEvent>
#include <QLabel>
#include <QSizePolicy>
#include <QTabBar>
#include <QTextStream>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QProcess>

// ConfigWindow contains the menus where you can configure the application

// TODO: перенести в отдельный файл!
#if defined(Q_OS_MACOS)
#elif defined(Q_OS_WIN)
#else
class CommandResult
{
private:
    QString outString;
    QString errString;

public:
    explicit CommandResult(const QString &outString, const QString &errString)
    {
        this->outString = outString;
        this->errString = errString;
    }

    bool hasError()
    {
        return !errString.isNull() && !errString.isEmpty();
    }

    bool isEmpty()
    {
        return outString.isNull() || outString.isEmpty();
    }

    QString getOutString()
    {
        return outString;
    }

    QString getErrString()
    {
        return errString;
    }

    QString getCleanOutString()
    {
        QString temp = outString;
        temp = temp.trimmed();
        return temp;
    }

    QStringList getStringList()
    {
        QString temp = outString;

        temp = temp.trimmed();

        if (temp.startsWith("@as", Qt::CaseInsensitive))
            temp = temp.remove(0, 3);

        temp = temp.trimmed();

        if (temp.startsWith('['))
        {
            temp = temp.remove(0, 1);
            temp.chop(1);
        }

        QStringList parts = temp.split(',', QString::SkipEmptyParts);

        for (QStringList::iterator it = parts.begin(); it != parts.end(); ++it)
        {
            QString& part = *it;
            part = part.trimmed();
        }

        return parts;
    }
};

static CommandResult shellCommand(const QString& commandText)
{
    QProcess process;
    process.start(commandText);
    process.waitForFinished(-1);

    const QByteArray stdoutBytes = process.readAllStandardOutput();
    const QByteArray stderrBytes = process.readAllStandardError();

    QString outString = QString(stdoutBytes);
    QString errString = QString(stderrBytes);

    return CommandResult(outString, errString);
}

static void setCustomKeybindingsIfNeeded()
{
    CommandResult getKeybindingsCommandResult = shellCommand("gsettings get org.gnome.settings-daemon.plugins.media-keys custom-keybindings");

    QStringList keybindings;

    if (!getKeybindingsCommandResult.hasError() && !getKeybindingsCommandResult.isEmpty())
        keybindings = getKeybindingsCommandResult.getStringList();

    const QString screenloadKeybinding = "'/org/gnome/settings-daemon/plugins/media-keys/custom-keybindings/screenload/'";

    if (!keybindings.isEmpty() && keybindings.contains(screenloadKeybinding, Qt::CaseInsensitive))
        return;

    keybindings.append(screenloadKeybinding);

    QString keybindingsLine = keybindings.join(',');

    QString setKeybindingsCommand = QString("gsettings set org.gnome.settings-daemon.plugins.media-keys custom-keybindings \"[%1]\"").arg(keybindingsLine);
    shellCommand(setKeybindingsCommand);
}

static bool isCustomBindingExists()
{
    QString bindingCommand = QString("gsettings get org.gnome.settings-daemon.plugins.media-keys.custom-keybinding:/org/gnome/settings-daemon/plugins/media-keys/custom-keybindings/screenload/ binding");
    CommandResult bindingCommandResult = shellCommand(bindingCommand);

    if (bindingCommandResult.hasError())
        return false;

    QString cleanOutString = bindingCommandResult.getCleanOutString();

    if (0 == cleanOutString.compare("''", Qt::CaseInsensitive))
        return false;

    return true;
}

static void setCustomBinding()
{
    shellCommand("gsettings reset org.gnome.settings-daemon.plugins.media-keys screenshot");
    shellCommand("gsettings reset org.gnome.shell.keybindings show-screenshot-ui");

    CommandResult commandResult = shellCommand("gsettings get org.gnome.shell.keybindings show-screenshot-ui"); // Ubuntu 22

    if (commandResult.hasError()) // Ubuntu 20 or 18
    {
        commandResult = shellCommand("gsettings get org.gnome.settings-daemon.plugins.media-keys screenshot");

        QString outString = commandResult.getOutString();
        QString value = outString.replace("Print", ""); // формат отличается для Ubuntu 20 и Ubuntu 18, используем формат ОС и производим замену значения.

        QString setCommand = QString("gsettings set org.gnome.settings-daemon.plugins.media-keys screenshot \"%1\"").arg(value);
        shellCommand(setCommand);
    }
    else
        shellCommand("gsettings set org.gnome.shell.keybindings show-screenshot-ui \"['']\""); // Ubuntu 22

    setCustomKeybindingsIfNeeded();

    shellCommand("gsettings set org.gnome.settings-daemon.plugins.media-keys.custom-keybinding:/org/gnome/settings-daemon/plugins/media-keys/custom-keybindings/screenload/ name 'screenload'");
    shellCommand("gsettings set org.gnome.settings-daemon.plugins.media-keys.custom-keybinding:/org/gnome/settings-daemon/plugins/media-keys/custom-keybindings/screenload/ command \"/usr/bin/screenload gui\"");
    shellCommand("gsettings set org.gnome.settings-daemon.plugins.media-keys.custom-keybinding:/org/gnome/settings-daemon/plugins/media-keys/custom-keybindings/screenload/ binding 'Print'");
}

static void resetCustomBinding()
{
    shellCommand("gsettings reset org.gnome.settings-daemon.plugins.media-keys.custom-keybinding:/org/gnome/settings-daemon/plugins/media-keys/custom-keybindings/screenload/ binding");

    shellCommand("gsettings reset org.gnome.settings-daemon.plugins.media-keys screenshot");
    shellCommand("gsettings reset org.gnome.shell.keybindings show-screenshot-ui");
}
#endif

ConfigWindow::ConfigWindow(QWidget* parent)
  : QWidget(parent)
{
    // We wrap QTabWidget in a QWidget because of a Qt bug
    auto* layout = new QVBoxLayout(this);
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->tabBar()->setUsesScrollButtons(false);
    layout->addWidget(m_tabWidget);

    setAttribute(Qt::WA_DeleteOnClose);
    setWindowIcon(QIcon(GlobalValues::iconPath()));
    setWindowTitle(tr("Configuration"));

    connect(ConfigHandler::getInstance(),
            &ConfigHandler::fileChanged,
            this,
            &ConfigWindow::updateChildren);

    QColor background = this->palette().window().color();
    bool isDark = ColorUtils::colorIsDark(background);
    QString modifier =
      isDark ? PathInfo::whiteIconPath() : PathInfo::blackIconPath();

    // visuals
    m_visuals = new VisualsEditor();
    m_visualsTab = new QWidget();
    auto* visualsLayout = new QVBoxLayout(m_visualsTab);
    m_visualsTab->setLayout(visualsLayout);
    visualsLayout->addWidget(m_visuals);
    m_tabWidget->addTab(
      m_visualsTab, QIcon(modifier + "graphics.svg"), tr("Interface"));

    // filename
    m_filenameEditor = new FileNameEditor();
    m_filenameEditorTab = new QWidget();
    auto* filenameEditorLayout = new QVBoxLayout(m_filenameEditorTab);
    m_filenameEditorTab->setLayout(filenameEditorLayout);
    filenameEditorLayout->addWidget(m_filenameEditor);
    m_tabWidget->addTab(m_filenameEditorTab,
                        QIcon(modifier + "name_edition.svg"),
                        tr("Filename Editor"));

    // general
    m_generalConfig = new GeneralConf();
    m_generalConfigTab = new QWidget();
    auto* generalConfigLayout = new QVBoxLayout(m_generalConfigTab);
    m_generalConfigTab->setLayout(generalConfigLayout);
    generalConfigLayout->addWidget(m_generalConfig);
    m_tabWidget->addTab(
      m_generalConfigTab, QIcon(modifier + "config.svg"), tr("General"));

    // shortcuts
    m_shortcuts = new ShortcutsWidget();
    m_shortcutsTab = new QWidget();
    auto* shortcutsLayout = new QVBoxLayout(m_shortcutsTab);
    m_shortcutsTab->setLayout(shortcutsLayout);
    shortcutsLayout->addWidget(m_shortcuts);

#if defined(Q_OS_MACOS)
#elif defined(Q_OS_WIN)
#else
    // MarketKernel [Print Screen]
    QCheckBox* reassignPrintScreenCheckBox = new QCheckBox(tr("Use ScreenLoad as [Print Screen] handler instead of system tool"), this);
    reassignPrintScreenCheckBox->setChecked(isCustomBindingExists());
    reassignPrintScreenCheckBox->setToolTip(tr("Use ScreenLoad to capture screenshots by [Print Screen] shortcut. Unchecking this checkbox will restore the system's default behavior."));

    connect(reassignPrintScreenCheckBox, &QCheckBox::stateChanged, [=](int state) {
        if (state == Qt::Checked)
            setCustomBinding();
        else
            resetCustomBinding();
    });

    shortcutsLayout->addWidget(reassignPrintScreenCheckBox);
#endif

    m_tabWidget->addTab(
      m_shortcutsTab, QIcon(modifier + "shortcut.svg"), tr("Shortcuts"));

    // connect update sigslots
    connect(this,
            &ConfigWindow::updateChildren,
            m_filenameEditor,
            &FileNameEditor::updateComponents);
    connect(this,
            &ConfigWindow::updateChildren,
            m_visuals,
            &VisualsEditor::updateComponents);
    connect(this,
            &ConfigWindow::updateChildren,
            m_generalConfig,
            &GeneralConf::updateComponents);

    // Error indicator (this must come last)
    initErrorIndicator(m_visualsTab, m_visuals);
    initErrorIndicator(m_filenameEditorTab, m_filenameEditor);
    initErrorIndicator(m_generalConfigTab, m_generalConfig);
    initErrorIndicator(m_shortcutsTab, m_shortcuts);
}

void ConfigWindow::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Escape) {
        close();
    }
}

void ConfigWindow::initErrorIndicator(QWidget* tab, QWidget* widget)
{
    auto* label = new QLabel(tab);
    auto* btnResolve = new QPushButton(tr("Resolve"), tab);
    auto* btnLayout = new QHBoxLayout();

    // Set up label
    label->setText(tr(
      "<b>Configuration file has errors. Resolve them before continuing.</b>"));
    label->setStyleSheet(QStringLiteral(":disabled { color: %1; }")
                           .arg(qApp->palette().color(QPalette::Text).name()));
    label->setVisible(ConfigHandler().hasError());

    // Set up "Show errors" button
    btnResolve->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    btnLayout->addWidget(btnResolve);
    btnResolve->setVisible(ConfigHandler().hasError());

    widget->setEnabled(!ConfigHandler().hasError());

    // Add label and button to the parent widget's layout
    auto* layout = static_cast<QBoxLayout*>(tab->layout());
    if (layout != nullptr) {
        layout->insertWidget(0, label);
        layout->insertLayout(1, btnLayout);
    } else {
        widget->layout()->addWidget(label);
        widget->layout()->addWidget(btnResolve);
    }

    // Sigslots
    connect(ConfigHandler::getInstance(), &ConfigHandler::error, widget, [=]() {
        widget->setEnabled(false);
        label->show();
        btnResolve->show();
    });
    connect(ConfigHandler::getInstance(),
            &ConfigHandler::errorResolved,
            widget,
            [=]() {
                widget->setEnabled(true);
                label->hide();
                btnResolve->hide();
            });
    connect(btnResolve, &QPushButton::clicked, this, [this]() {
        ConfigResolver().exec();
    });
}
