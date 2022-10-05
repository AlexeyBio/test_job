#ifndef BOOKMARKMODEL_H
#define BOOKMARKMODEL_H
#include <QObject>
#include <QAbstractListModel>
#include <QThread>
#include <vector>
#include <memory>
#include "bookmark.h"

using Bookmarks = std::vector<Bookmark>;
struct GroupedBookmark{
    GroupedBookmark(Bookmarks::iterator iterBookmark);
    GroupedBookmark() = default;
    Bookmarks::iterator left{};
    Bookmarks::iterator right{};
    quint64 duration{};
    bool isGrouped() const;
};
using BookmarkGroups = std::vector<GroupedBookmark>;

class BookmarkModelWorker : public QObject
{
    Q_OBJECT
public:
    explicit BookmarkModelWorker(QObject* parent = nullptr);
    virtual ~BookmarkModelWorker();
signals:
    void done(std::shared_ptr<Bookmarks>);

public slots:
    void generate(QDateTime begin, QDateTime end, size_t count);
private:
    std::vector<Bookmark> m_bookmarks;
};

class BookmarkModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        NameRole = Qt::UserRole,
        TimeRole,
        DurationRole,
        IsGroupRole,
        GroupModelRole //TODO make proxy model
    };

    Q_PROPERTY(QVariantMap roles READ roles CONSTANT)
    Q_PROPERTY(QDateTime frontDateTime READ frontDateTime NOTIFY frontDateTimeChanged)
    Q_PROPERTY(QDateTime backDateTime READ backDateTime NOTIFY backDateTimeChanged)
    Q_PROPERTY(QDateTime filterMinimumDateTime
               READ filterMinimumDateTime
               NOTIFY filterMinimumDateTimeChanged)
    Q_PROPERTY(QDateTime filterMaximumDateTime
               READ filterMaximumDateTime
               NOTIFY filterMaximumDateTimeChanged)

    Q_PROPERTY(quint64 durationOnPart
               READ durationOnPart
               WRITE setDurationOnPart
               NOTIFY durationOnPartChanged)


    Q_INVOKABLE void generate(QDateTime begin, QDateTime end, quint64 count);
    Q_INVOKABLE bool setFilterRangeDateTime(QDateTime begin, QDateTime end);

    void setFilterMinimumDateTime(QDateTime filterMinimumDateTime);
    void setFilterMaximumDateTime(QDateTime filterMaximumDateTime);

    QDateTime filterMinimumDateTime() const;
    QDateTime filterMaximumDateTime() const;

    explicit BookmarkModel(QObject *parent = nullptr);
    virtual ~BookmarkModel();
    quint64 durationOnPart() const;
    QVariantMap roles() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:
    void generateOnWorker(QDateTime begin, QDateTime end, quint64 count);
    void frontDateTimeChanged();
    void backDateTimeChanged();
    void filterMinimumDateTimeChanged(QDateTime filterMinimumDateTime);
    void filterMaximumDateTimeChanged(QDateTime filterMaximumDateTime);
    void durationOnPartChanged(quint64 durationOnPart);

public slots:
    void setDurationOnPart(quint64 durationOnPart);
    QDateTime frontDateTime() const;
    QDateTime backDateTime() const;
    void refresh(std::shared_ptr<Bookmarks> bookmarks);
private:
    void selectRange();
    void regroup();
    QVariantList makeLimitedGroup(Bookmarks::iterator left, Bookmarks::iterator right, int limit = 14) const;
private:
    BookmarkModelWorker* m_worker;
    Bookmarks m_bookmarks;
    Bookmarks::iterator m_left;
    Bookmarks::iterator m_right;
    BookmarkGroups m_bookmarkGroups;
    QThread m_thread;
    QDateTime m_filterMinimumDateTime;
    QDateTime m_filterMaximumDateTime;
    quint64 m_durationOnPart;
};

Q_DECLARE_METATYPE(std::shared_ptr<Bookmarks>)
#endif // BOOKMARKMODEL_H
