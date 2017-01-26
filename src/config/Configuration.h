#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QSettings>
#include <QString>
#include <QPointer>

// Configuration is a convenience class that simplifies the access of
// statically stored configuration data by providing a globally
// accessible instance thanks to the fact that the data
// is stored in QSettings
class Configuration
{

public:
    Configuration();
    ~Configuration();

    // TODO if more settings are added let each object accessing the
    //     configurations store the access string literals as static values
    //     and make readSetting public instead. doesn't make sense to provide
    //     each setting as a function.

    // True if the QSettings object is initilized and valid
    bool is_valid() const;

private:
    // reads the setting stored in the key given and returns
    // its value or empty string if there was a problem
    const QString readSetting(const QString &key) const;

    QScopedPointer<QSettings> m_settings;

    Q_DISABLE_COPY(Configuration)
};

#endif /* CONFIGURATION_H */
