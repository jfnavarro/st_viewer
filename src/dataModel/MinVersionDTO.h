#ifndef MINVERSIONDTO_H
#define MINVERSIONDTO_H

#include <QObject>
#include <array>

// DTOs (Data transform objects) provides the bridge between the client
// specific data model objects and the server data. This is accomplished by
// defining dynamic properties that enable automated serialization and
// deserialization of server data.

// this DTO parses the min version supported endpoint

// TODO move definitions to CPP and/or consider removing DTOs
class MinVersionDTO : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString minSupportedClientVersion READ minSupportedVersion WRITE minSupportedVersion)

public:
    explicit MinVersionDTO(QObject* parent = 0)
        : QObject(parent)
    {
    }
    ~MinVersionDTO() {}

    // binding
    const QString& minSupportedVersion() const { return m_minVersion; }

    std::array<qulonglong, 3> minVersionAsNumber() const
    {
        const QStringList minversion_numbers_as_strings = m_minVersion.split(".");
        if (minversion_numbers_as_strings.size() != 3) {
            Q_ASSERT_X(false, "minSupportedVersion", "Error parsing min version String");
        }
        bool ok1 = false;
        bool ok2 = false;
        bool ok3 = false;
        std::array<qulonglong, 3> minversion_numbers_as_qulonglong
            = {minversion_numbers_as_strings[0].toULongLong(&ok1),
               minversion_numbers_as_strings[1].toULongLong(&ok2),
               minversion_numbers_as_strings[2].toULongLong(&ok3)};
        if (!ok1 || !ok2 || !ok3) {
            Q_ASSERT_X(false, "minSupportedVersion", "Error parsing min version String");
        }

        return minversion_numbers_as_qulonglong;
    }

    // getters
    void minSupportedVersion(const QString& minVersion) { m_minVersion = minVersion; }

private:
    QString m_minVersion;
};

#endif // MINVERSIONDTO_H
