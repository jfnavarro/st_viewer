#include "User.h"

#include <QDate>

static const QString ROLE_USER = QStringLiteral("ROLE_USER");
static const QString ROLE_CM = QStringLiteral("ROLE_CM");
static const QString ROLE_ADMIN = QStringLiteral("ROLE_ADMIN");

User::User()
    : m_id()
    , m_username()
    , m_instituion()
    , m_firstName()
    , m_address()
    , m_postcode(0)
    , m_city()
    , m_country()
    , m_password()
    , m_role()
    , m_enabled(false)
    , m_grantedDatasets()
    , m_created(QDate::currentDate().toString())
    , m_lastMofidied(QDate::currentDate().toString())
{
}

User::User(const User &other)
    : m_id(other.m_id)
    , m_username(other.m_username)
    , m_instituion(other.m_instituion)
    , m_firstName(other.m_firstName)
    , m_address(other.m_address)
    , m_postcode(other.m_postcode)
    , m_city(other.m_city)
    , m_country(other.m_country)
    , m_password(other.m_password)
    , m_role(other.m_role)
    , m_enabled(other.m_enabled)
    , m_grantedDatasets(other.m_grantedDatasets)
    , m_created(other.m_created)
    , m_lastMofidied(other.m_lastMofidied)
{
}

User::~User()
{
}

User &User::operator=(const User &other)
{
    m_id = other.m_id;
    m_username = other.m_username;
    m_instituion = other.m_instituion;
    m_firstName = other.m_firstName;
    m_address = other.m_address;
    m_postcode = other.m_postcode;
    m_city = other.m_city;
    m_country = other.m_country;
    m_password = other.m_password;
    m_role = other.m_role;
    m_enabled = other.m_enabled;
    m_grantedDatasets = other.m_grantedDatasets;
    m_created = other.m_created;
    m_lastMofidied = other.m_lastMofidied;
    return (*this);
}

bool User::operator==(const User &other) const
{
    return (m_id == other.m_id && m_username == other.m_username
            && m_instituion == other.m_instituion && m_firstName == other.m_firstName
            && m_address == other.m_address && m_postcode == other.m_postcode
            && m_city == other.m_city && m_country == other.m_country
            && m_password == other.m_password && m_role == other.m_role
            && m_enabled == other.m_enabled && m_grantedDatasets == other.m_grantedDatasets
            && m_created == other.m_created && m_lastMofidied == other.m_lastMofidied);
}

const QString User::id() const
{
    return m_id;
}

const QString User::username() const
{
    return m_username;
}

const QString User::institution() const
{
    return m_instituion;
}

const QString User::firstName() const
{
    return m_firstName;
}

const QString User::secondName() const
{
    return m_secondName;
}

const QString User::address() const
{
    return m_address;
}

unsigned User::postcode() const
{
    return m_postcode;
}

const QString User::city() const
{
    return m_city;
}

const QString User::country() const
{
    return m_country;
}

const QString User::password() const
{
    return m_password;
}

const QString User::role() const
{
    return m_role;
}

bool User::enabled() const
{
    return m_enabled;
}

const QVector<QString> User::grantedDatasets() const
{
    return m_grantedDatasets;
}

const QString User::created() const
{
    return m_created;
}

const QString User::lastModified() const
{
    return m_lastMofidied;
}

void User::id(const QString &id)
{
    m_id = id;
}

void User::username(const QString &username)
{
    m_username = username;
}

void User::institution(const QString &institution)
{
    m_instituion = institution;
}

void User::firstName(const QString &firstName)
{
    m_firstName = firstName;
}

void User::secondName(const QString &secondName)
{
    m_secondName = secondName;
}

void User::address(const QString &address)
{
    m_address = address;
}

void User::postcode(unsigned postCode)
{
    m_postcode = postCode;
}

void User::city(const QString &city)
{
    m_city = city;
}

void User::country(const QString &country)
{
    m_country = country;
}

void User::password(const QString &password)
{
    m_password = password;
}

void User::role(const QString &role)
{
    m_role = role;
}

void User::enabled(bool enabled)
{
    m_enabled = enabled;
}

void User::grantedDatasets(const QVector<QString> &grantedDatasets)
{
    m_grantedDatasets = grantedDatasets;
}

void User::created(const QString &created)
{
    m_created = created;
}

void User::lastModified(const QString &lastModified)
{
    m_lastMofidied = lastModified;
}

bool User::hasSpecialRole() const
{
    return m_role == ROLE_CM || m_role == ROLE_ADMIN;
}
