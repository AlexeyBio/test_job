#include "bookmark.h"

QString Bookmark::name() const
{
    return m_name;
}

QDateTime Bookmark::time() const
{
    return m_time;
}

quint64 Bookmark::duration() const
{
    return m_duration;
}

void Bookmark::setName(QString name)
{
    m_name = name;
}

void Bookmark::setTime(QDateTime time)
{
    m_time = time;
}

void Bookmark::setDuration(quint64 duration)
{
    m_duration = duration;
}
