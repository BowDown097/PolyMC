#pragma once

#include <QFile>
#include <QtNetwork/QtNetwork>
#include <memory>
#include "tasks/Task.h"

typedef shared_qobject_ptr<class SkinUpload> SkinUploadPtr;

class SkinUpload : public Task
{
    Q_OBJECT
public:
    enum Model
    {
        STEVE,
        ALEX
    };

    // Note this class takes ownership of the file.
    SkinUpload(QObject *parent, const QString &token, const QByteArray &skin, Model model = STEVE)
        : Task(parent), m_model(model), m_skin(skin), m_token(token) {}
    virtual ~SkinUpload() {}

private:
    Model m_model;
    QByteArray m_skin;
    QString m_token;
    shared_qobject_ptr<QNetworkReply> m_reply;
protected:
    virtual void executeTask();

public slots:

    void downloadError(QNetworkReply::NetworkError);

    void downloadFinished();
};
