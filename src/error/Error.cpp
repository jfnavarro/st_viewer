#include "Error.h"

Error::Error(QObject* parent)
    : QObject(parent)
    , m_name()
    , m_description()

{
}

Error::Error(const QString& name, const QString& description, QObject* parent)
    : QObject(parent)
    , m_name(name)
    , m_description(description)
{
}

Error::~Error()
{
}

const QString Error::name() const
{
    return m_name;
}

const QString Error::description() const
{
    return m_description;
}

void Error::name(const QString& name)
{
    m_name = name;
}

void Error::description(const QString& description)
{
    m_description = description;
}
