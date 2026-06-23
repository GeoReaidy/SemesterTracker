#include "updatemanager.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QProgressDialog>
#include <QPushButton>
#include <QStandardPaths>
#include <QVersionNumber>

namespace
{
const char *LatestReleaseUrl =
    "https://api.github.com/repos/GeoReaidy/SemesterTracker/releases/latest";

const char *ExpectedInstallerName =
    "SemesterTrackerSetup.exe";

QString normalizedVersion(QString version)
{
    version = version.trimmed();

    if (version.startsWith('v', Qt::CaseInsensitive))
    {
        version.removeFirst();
    }

    return version;
}
}

UpdateManager::UpdateManager(
    QWidget *dialogParent,
    QObject *parent)
    : QObject(parent),
      dialogParent(dialogParent),
      networkManager(new QNetworkAccessManager(this))
{
}

void UpdateManager::checkForUpdates(
    bool showNoUpdateMessage)
{
    QNetworkRequest request{
        QUrl(QString::fromLatin1(LatestReleaseUrl))
    };

    request.setHeader(
        QNetworkRequest::UserAgentHeader,
        "SemesterTracker/" APP_VERSION
    );

    request.setRawHeader(
        "Accept",
        "application/vnd.github+json"
    );

    request.setRawHeader(
        "X-GitHub-Api-Version",
        "2022-11-28"
    );

    request.setAttribute(
        QNetworkRequest::RedirectPolicyAttribute,
        QNetworkRequest::NoLessSafeRedirectPolicy
    );

    QNetworkReply *reply =
        networkManager->get(request);

    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply, showNoUpdateMessage]()
        {
            handleReleaseReply(
                reply,
                showNoUpdateMessage
            );

            reply->deleteLater();
        }
    );
}

void UpdateManager::handleReleaseReply(
    QNetworkReply *reply,
    bool showNoUpdateMessage)
{
    const int statusCode =
        reply->attribute(
            QNetworkRequest::HttpStatusCodeAttribute
        ).toInt();

    if (reply->error() != QNetworkReply::NoError)
    {
        if (showNoUpdateMessage)
        {
            if (statusCode == 404)
            {
                showInformation(
                    "No Published Release",
                    "No public Semester Tracker release has been published yet."
                );
            }
            else
            {
                showWarning(
                    "Update Check Failed",
                    "Semester Tracker could not check for updates.\n\n"
                    "Check your internet connection and try again."
                );
            }
        }

        return;
    }

    QJsonParseError parseError;
    const QJsonDocument document =
        QJsonDocument::fromJson(
            reply->readAll(),
            &parseError
        );

    if (parseError.error != QJsonParseError::NoError ||
        !document.isObject())
    {
        if (showNoUpdateMessage)
        {
            showWarning(
                "Update Check Failed",
                "GitHub returned an invalid release response."
            );
        }

        return;
    }

    const QJsonObject release =
        document.object();

    if (release.value("draft").toBool() ||
        release.value("prerelease").toBool())
    {
        if (showNoUpdateMessage)
        {
            showInformation(
                "No Stable Update",
                "There is no newer stable release available."
            );
        }

        return;
    }

    const QString latestTag =
        release.value("tag_name").toString();

    const QString latestVersionText =
        normalizedVersion(latestTag);

    const QString currentVersionText =
        normalizedVersion(
            QStringLiteral(APP_VERSION)
        );

    const QVersionNumber latestVersion =
        QVersionNumber::fromString(
            latestVersionText
        );

    const QVersionNumber currentVersion =
        QVersionNumber::fromString(
            currentVersionText
        );

    if (latestVersion.isNull())
    {
        if (showNoUpdateMessage)
        {
            showWarning(
                "Update Check Failed",
                "The latest GitHub release has an invalid version tag."
            );
        }

        return;
    }

    if (QVersionNumber::compare(
            latestVersion,
            currentVersion) <= 0)
    {
        if (showNoUpdateMessage)
        {
            showInformation(
                "Semester Tracker Is Up to Date",
                QString(
                    "You are using the latest version: %1"
                ).arg(currentVersionText)
            );
        }

        return;
    }

    QUrl installerUrl;

    const QJsonArray assets =
        release.value("assets").toArray();

    for (const QJsonValue &assetValue : assets)
    {
        const QJsonObject asset =
            assetValue.toObject();

        if (asset.value("name").toString() ==
            QString::fromLatin1(
                ExpectedInstallerName
            ))
        {
            installerUrl = QUrl(
                asset.value(
                    "browser_download_url"
                ).toString()
            );

            break;
        }
    }

    if (!installerUrl.isValid())
    {
        showWarning(
            "Update Available",
            QString(
                "Version %1 is available, but the release does not contain "
                "%2."
            )
                .arg(latestVersionText)
                .arg(
                    QString::fromLatin1(
                        ExpectedInstallerName
                    )
                )
        );

        return;
    }

    const QString releaseNotes =
        release.value("body")
            .toString()
            .trimmed();

    QMessageBox updateBox(
        QMessageBox::Information,
        "Semester Tracker Update",
        QString(
            "A new version of Semester Tracker is available.\n\n"
            "Current version: %1\n"
            "New version: %2"
        )
            .arg(currentVersionText)
            .arg(latestVersionText),
        QMessageBox::NoButton,
        dialogParent
    );

    if (!releaseNotes.isEmpty())
    {
        updateBox.setDetailedText(releaseNotes);
    }

    QPushButton *installButton =
        updateBox.addButton(
            "Download and Install",
            QMessageBox::AcceptRole
        );

    updateBox.addButton(
        "Later",
        QMessageBox::RejectRole
    );

    updateBox.exec();

    if (updateBox.clickedButton() !=
        installButton)
    {
        return;
    }

    beginInstallerDownload(
        installerUrl,
        latestVersionText
    );
}

void UpdateManager::beginInstallerDownload(
    const QUrl &downloadUrl,
    const QString &version)
{
    const QString temporaryDirectory =
        QStandardPaths::writableLocation(
            QStandardPaths::TempLocation
        );

    if (temporaryDirectory.isEmpty())
    {
        showWarning(
            "Update Download Failed",
            "Windows did not provide a temporary download folder."
        );
        return;
    }

    downloadedInstallerPath =
        QDir(temporaryDirectory).filePath(
            QString(
                "SemesterTrackerSetup-%1.exe"
            ).arg(version)
        );

    delete downloadFile;
    downloadFile =
        new QFile(
            downloadedInstallerPath,
            this
        );

    if (!downloadFile->open(
            QIODevice::WriteOnly |
            QIODevice::Truncate))
    {
        showWarning(
            "Update Download Failed",
            "The installer could not be written to the temporary folder."
        );

        downloadFile->deleteLater();
        downloadFile = nullptr;
        return;
    }

    downloadingVersion = version;

    delete progressDialog;
    progressDialog =
        new QProgressDialog(
            "Downloading the Semester Tracker update…",
            "Cancel",
            0,
            100,
            dialogParent
        );

    progressDialog->setWindowTitle(
        "Downloading Update"
    );

    progressDialog->setWindowModality(
        Qt::WindowModal
    );

    progressDialog->setMinimumDuration(0);
    progressDialog->setValue(0);
    progressDialog->show();

    QNetworkRequest request(downloadUrl);

    request.setHeader(
        QNetworkRequest::UserAgentHeader,
        "SemesterTracker/" APP_VERSION
    );

    request.setAttribute(
        QNetworkRequest::RedirectPolicyAttribute,
        QNetworkRequest::NoLessSafeRedirectPolicy
    );

    QNetworkReply *reply =
        networkManager->get(request);

    connect(
        reply,
        &QNetworkReply::readyRead,
        this,
        [this, reply]()
        {
            if (downloadFile)
            {
                downloadFile->write(
                    reply->readAll()
                );
            }
        }
    );

    connect(
        reply,
        &QNetworkReply::downloadProgress,
        this,
        [this](qint64 received, qint64 total)
        {
            if (!progressDialog || total <= 0)
            {
                return;
            }

            const int percentage =
                static_cast<int>(
                    (received * 100) / total
                );

            progressDialog->setValue(
                percentage
            );
        }
    );

    connect(
        progressDialog,
        &QProgressDialog::canceled,
        reply,
        &QNetworkReply::abort
    );

    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply]()
        {
            finishInstallerDownload(reply);
            reply->deleteLater();
        }
    );
}

void UpdateManager::finishInstallerDownload(
    QNetworkReply *reply)
{
    if (downloadFile)
    {
        downloadFile->write(
            reply->readAll()
        );

        downloadFile->close();
    }

    if (progressDialog)
    {
        progressDialog->close();
        progressDialog->deleteLater();
        progressDialog = nullptr;
    }

    const bool downloadSucceeded =
        reply->error() ==
            QNetworkReply::NoError &&
        downloadFile &&
        downloadFile->size() > 0;

    if (downloadFile)
    {
        downloadFile->deleteLater();
        downloadFile = nullptr;
    }

    if (!downloadSucceeded)
    {
        QFile::remove(
            downloadedInstallerPath
        );

        if (reply->error() !=
            QNetworkReply::OperationCanceledError)
        {
            showWarning(
                "Update Download Failed",
                "The update installer could not be downloaded."
            );
        }

        return;
    }

    const QMessageBox::StandardButton installNow =
        QMessageBox::question(
            dialogParent,
            "Install Update",
            QString(
                "Version %1 has been downloaded.\n\n"
                "Semester Tracker will close and the installer will start."
            ).arg(downloadingVersion),
            QMessageBox::Yes |
            QMessageBox::Cancel,
            QMessageBox::Yes
        );

    if (installNow != QMessageBox::Yes)
    {
        return;
    }

    const bool installerStarted =
        QProcess::startDetached(
            downloadedInstallerPath,
            {
                "/SILENT",
                "/CLOSEAPPLICATIONS",
                "/NORESTART"
            }
        );

    if (!installerStarted)
    {
        showWarning(
            "Update Installation Failed",
            "Windows could not start the downloaded installer."
        );
        return;
    }

    QApplication::quit();
}

void UpdateManager::showInformation(
    const QString &title,
    const QString &message) const
{
    QMessageBox::information(
        dialogParent,
        title,
        message
    );
}

void UpdateManager::showWarning(
    const QString &title,
    const QString &message) const
{
    QMessageBox::warning(
        dialogParent,
        title,
        message
    );
}
