#ifndef BOOKMARK_H
#define BOOKMARK_H
#include <QObject>
#include <QDateTime>
#include <QString>

struct Bookmark
{
    Q_GADGET
    QString m_name;
    QDateTime m_time;
    quint64 m_duration;

public:
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QDateTime time READ time WRITE setTime)
    Q_PROPERTY(quint64 duration READ duration WRITE setDuration)
    QString name() const;
    QDateTime time() const;
    quint64 duration() const;

public slots:
    void setName(QString name);
    void setTime(QDateTime time);
    void setDuration(quint64 duration);
};

Q_DECLARE_METATYPE(Bookmark)

#endif // BOOKMARK_H
