#include "bookmarkmodel.h"
#include <QDebug>
#include <random>
#include <algorithm>
#include <QString>

static auto cmp = [](const Bookmark& left, const Bookmark& right) -> bool {
    return left.time() < right.time();
};

BookmarkModel::BookmarkModel(QObject *parent)
    : QAbstractListModel(parent)
{
    qRegisterMetaType<std::shared_ptr<Bookmarks> >();

    m_worker = new BookmarkModelWorker;
    m_worker->moveToThread(&m_thread);
    m_thread.start();
    connect(m_worker, &BookmarkModelWorker::done,
            this, &BookmarkModel::refresh);
    connect(this, &BookmarkModel::generateOnWorker,
            m_worker, &BookmarkModelWorker::generate);
}

BookmarkModel::~BookmarkModel(){
    m_thread.quit();
    m_thread.wait();
}

void BookmarkModel::generate(QDateTime begin, QDateTime end, quint64 count)
{
    emit generateOnWorker(begin, end, count);
}

bool BookmarkModel::setFilterRangeDateTime(QDateTime begin, QDateTime end)
{
    if (begin < end){
        setFilterMinimumDateTime(begin);
        setFilterMaximumDateTime(end);
        selectRange();
        return true;
    }
    return false;
}


void BookmarkModel::selectRange(){
    Bookmark leftValue;
    leftValue.setTime(m_filterMinimumDateTime);
    m_left = std::lower_bound(
                m_bookmarks.begin(),
                m_bookmarks.end(),
                leftValue,
                cmp);
    Bookmark rightValue;
    rightValue.setTime(m_filterMaximumDateTime);
    m_right = std::upper_bound(
                m_left,
                m_bookmarks.end(),
                rightValue,
                cmp);
}

void BookmarkModel::regroup(){
    auto it = m_left;
    auto end = m_right;
    m_bookmarkGroups.reserve(m_bookmarks.size());
    m_bookmarkGroups.clear();
    if (it == end){
        return;
    }
    QDateTime startTimeGroup;
    GroupedBookmark currentGroup(it);
    ++it;
    for (; it != end; ++it){
        auto diffTime = currentGroup.left->time().msecsTo(it->time());
        if (diffTime <= m_durationOnPart){
            currentGroup.duration = std::max(
                        currentGroup.duration,
                        diffTime + it->duration());
            currentGroup.right = it;
        } else {
            m_bookmarkGroups.push_back(currentGroup);
            currentGroup = GroupedBookmark(it);
        }
    }
    m_bookmarkGroups.push_back(currentGroup);
}

QVariantList BookmarkModel::makeLimitedGroup(Bookmarks::iterator left, Bookmarks::iterator right, int limit) const
{
    QVariantList list;
    int groupSize = std::distance(left, right);
    int count = std::min(limit, groupSize);
    list.reserve(count);
    auto end = left + count;
    QVariant value;
    for (auto it = left; it <= end; ++it){
        value.setValue(*it);
        list << value;
    }
    if (groupSize > limit){
        Bookmark last;
        last.setName(
                    QStringLiteral("+ %1 other bookmarks")
                    .arg(groupSize - limit));
        value.setValue(last);
        list << value;
    }
    return list;
}

void BookmarkModel::setFilterMinimumDateTime(QDateTime filterMinimumDateTime)
{
    if (m_filterMinimumDateTime == filterMinimumDateTime)
        return;
    m_filterMinimumDateTime = filterMinimumDateTime;
    emit filterMinimumDateTimeChanged(m_filterMinimumDateTime);
}

void BookmarkModel::setFilterMaximumDateTime(QDateTime filterMaximumDateTime)
{
    if (m_filterMaximumDateTime == filterMaximumDateTime)
        return;
    m_filterMaximumDateTime = filterMaximumDateTime;
    emit filterMaximumDateTimeChanged(m_filterMaximumDateTime);
}

QDateTime BookmarkModel::filterMinimumDateTime() const
{
    return m_filterMinimumDateTime;
}

QDateTime BookmarkModel::filterMaximumDateTime() const
{
    return m_filterMaximumDateTime;
}

QDateTime BookmarkModel::frontDateTime() const {
    QDateTime result;
    if (m_left != m_right)
        result = m_left->time();
    return result;
}

QDateTime BookmarkModel::backDateTime() const {
    QDateTime result;
    if (m_left != m_right)
        result = m_right->time();
    return result;
}

void BookmarkModel::refresh(std::shared_ptr<Bookmarks> bookmarks)
{
    beginResetModel();
    m_bookmarks.clear();
    std::swap(m_bookmarks, *bookmarks);
    selectRange();
    regroup();
    endResetModel();
    emit frontDateTimeChanged();
    emit backDateTimeChanged();
}

void BookmarkModel::setDurationOnPart(quint64 durationOnPart)
{
    if (m_durationOnPart == durationOnPart)
        return;

    m_durationOnPart = durationOnPart;
    beginResetModel();
    regroup();
    endResetModel();
    emit frontDateTimeChanged();
    emit backDateTimeChanged();
    emit durationOnPartChanged(m_durationOnPart);
}

int BookmarkModel::rowCount(const QModelIndex &parent) const
{
    return m_bookmarkGroups.size();
}

QVariant BookmarkModel::data(const QModelIndex &index, int role) const
{
    QVariant result;
    if (!index.isValid()
            || index.row() >= rowCount())
        return result;
    const auto& it = m_bookmarkGroups[index.row()];
    switch (role) {
    case Qt::DisplayRole:
    case Roles::NameRole:
        result = it.isGrouped()
                ? QString::number(std::distance(it.left, it.right) + 1)
                : it.left->name();
        break;
    case Roles::TimeRole:
        result = it.left->time();
        break;
    case Roles::DurationRole:
        result = it.duration;
        break;
    case Roles::IsGroupRole:
        result = it.isGrouped();
        break;
    case Roles::GroupModelRole:
        result = makeLimitedGroup(it.left, it.right);
        break;
    }
    return result;
}

QHash<int, QByteArray> BookmarkModel::roleNames() const{
    static QHash<int, QByteArray> roles{
        { Roles::NameRole, "name" },
        { Roles::TimeRole, "time" },
        { Roles::DurationRole, "duration" },
        { Roles::IsGroupRole, "isGroup" },
        { Roles::GroupModelRole, "groupModel" }
    };
    return roles;
}

quint64 BookmarkModel::durationOnPart() const
{
    return m_durationOnPart;
}

QVariantMap BookmarkModel::roles() const
{
    QVariantMap nameToRole;
    auto roles = roleNames();
    for (auto it = roles.begin(); it != roles.end(); ++it){
        nameToRole.insert(it.value(), it.key());
    }
    return nameToRole;
}

BookmarkModelWorker::BookmarkModelWorker(QObject *parent)
    : QObject(parent) {}

void BookmarkModelWorker::generate(QDateTime begin, QDateTime end, size_t count)
{
    m_bookmarks.clear();
    m_bookmarks.reserve(count);
    const auto timeDiff = begin.msecsTo(end);
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<uint64_t> dist(0, timeDiff);
    QDateTime datetime(begin);
    for (size_t i = 0; i < count; ++i){
        Bookmark bookmark;
        uint64_t time = dist(mt);
        bookmark.setTime(begin.addMSecs(time));
        bookmark.setName(QStringLiteral("Bookmark %1").arg(i + 1));
        bookmark.setDuration(3 * 60 * 60 * 1000); // 3 h
        m_bookmarks.push_back(bookmark);
    }
    std::sort(m_bookmarks.begin(), m_bookmarks.end(), cmp);

    emit done(std::make_shared<Bookmarks>(std::move(m_bookmarks)));
}

BookmarkModelWorker::~BookmarkModelWorker() {}

GroupedBookmark::GroupedBookmark(Bookmarks::iterator iterBookmark){
    left = right = iterBookmark;
    duration = iterBookmark->duration();
}

bool GroupedBookmark::isGrouped() const {
    return left != right;
}
