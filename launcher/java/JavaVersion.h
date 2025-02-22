#pragma once

#include <QString>

// NOTE: apparently the GNU C library pollutes the global namespace with these... undef them.
#ifdef major
    #undef major
#endif
#ifdef minor
    #undef minor
#endif

class JavaVersion
{
    friend class JavaVersionTest;
public:
    JavaVersion() {};
    explicit JavaVersion(const QString & rhs);

    JavaVersion & operator=(const QString & rhs);

    bool operator<(const JavaVersion & rhs);
    bool operator==(const JavaVersion & rhs);
    bool operator>(const JavaVersion & rhs);

    bool requiresPermGen();

    QString toString();

    int major() const
    {
        return m_major;
    }
    int minor() const
    {
        return m_minor;
    }
    int security() const
    {
        return m_security;
    }
private:
    QString m_string;
    int m_major = 0;
    int m_minor = 0;
    int m_security = 0;
    bool m_parseable = false;
    QString m_prerelease;
};
