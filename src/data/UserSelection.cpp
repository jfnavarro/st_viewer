#include "UserSelection.h"

UserSelection::UserSelection()
    : m_name()
    , m_dataset()
    , m_sliced_data()
    , m_type()
    , m_comment()
    , m_tissueSnapShot()
{
}

UserSelection::~UserSelection()
{
}

const QString UserSelection::name() const
{
    return m_name;
}

const QString UserSelection::dataset() const
{
    return m_dataset;
}

const STData::Matrix &UserSelection::slicedData() const
{
    return m_sliced_data;
}

const QString UserSelection::comment() const
{
    return m_comment;
}

UserSelection::Type UserSelection::type() const
{
    return m_type;
}

const QByteArray UserSelection::tissueSnapShot() const
{
    return m_tissueSnapShot;
}

void UserSelection::name(const QString &name)
{
    m_name = name;
}

void UserSelection::dataset(const QString &dataset)
{
    m_dataset = dataset;
}

void UserSelection::slicedData(const STData::Matrix &data)
{
    m_sliced_data = data;
}

void UserSelection::type(const Type &type)
{
    m_type = type;
}

void UserSelection::comment(const QString &comment)
{
    m_comment = comment;
}

void UserSelection::tissueSnapShot(const QByteArray &tissueSnapShot)
{
    m_tissueSnapShot = tissueSnapShot;
}

const QString UserSelection::typeToQString(const UserSelection::Type &type)
{
    switch (type) {
    case Rubberband:
        return QString("Rubberband");
    case Lazo:
        return QString("Lazo");
    case Segmented:
        return QString("Segmented");
    case Console:
        return QString("Console");
    case Cluster:
        return QString("Cluster");
    case Other:
        return QString("Other");
    default:
        Q_ASSERT_X(true, "UserSelection", "Invalid selection type!");
    }
    Q_ASSERT(false); // Should never arrive here
    return QString();
}

UserSelection::Type UserSelection::QStringToType(const QString &type)
{
    const QString lower_type = type.toLower();
    if (lower_type == "rubberband") {
        return Rubberband;
    } else if (lower_type == "lazo") {
        return Lazo;
    } else if (lower_type == "segmented") {
        return Segmented;
    } else if (lower_type == "console") {
        return Console;
    } else if (lower_type == "cluster") {
        return Cluster;
    } else if (lower_type == "other") {
        return Other;
    }
    // Should never arrive here
    Q_ASSERT_X(true, "UserSelection", "Invalid selection type!");
    return Other;
}
