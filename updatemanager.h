#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#include <QObject>
#include <QUrl>

class QFile;
class QNetworkAccessManager;
class QNetworkReply;
class QProgressDialog;
class QWidget;

class UpdateManager : public QObject
{
    Q_OBJECT

public:
    explicit UpdateManager(
        QWidget *dialogParent = nullptr,
        QObject *parent = nullptr
    );

    void checkForUpdates(
        bool showNoUpdateMessage = false
    );

private:
    void handleReleaseReply(
        QNetworkReply *reply,
        bool showNoUpdateMessage
    );

    void beginInstallerDownload(
        const QUrl &downloadUrl,
        const QString &version
    );

    void finishInstallerDownload(
        QNetworkReply *reply
    );

    void showInformation(
        const QString &title,
        const QString &message
    ) const;

    void showWarning(
        const QString &title,
        const QString &message
    ) const;

    QWidget *dialogParent = nullptr;
    QNetworkAccessManager *networkManager = nullptr;
    QProgressDialog *progressDialog = nullptr;
    QFile *downloadFile = nullptr;

    QString downloadedInstallerPath;
    QString downloadingVersion;
};

#endif // UPDATEMANAGER_H
