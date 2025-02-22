#pragma once

#include <modplatform/legacy_ftb/PackHelpers.h>
#include <RWStorage.h>

#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <QThreadPool>
#include <QIcon>
#include <QStyledItemDelegate>

#include <functional>

namespace LegacyFTB {

typedef QMap<QString, QIcon> FTBLogoMap;
typedef std::function<void(QString)> LogoCallback;

class FilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit FilterModel(QObject* parent = nullptr);
    enum Sorting {
        ByName,
        ByGameVersion
    };
    const QMap<QString, Sorting> getAvailableSortings();
    QString translateCurrentSorting();
    void setSorting(Sorting sorting);
    Sorting getCurrentSorting();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    QMap<QString, Sorting> sortings;
    Sorting currentSorting;

};

class ListModel : public QAbstractListModel
{
    Q_OBJECT
private:
    ModpackList modpacks;
    QStringList m_failedLogos;
    QStringList m_loadingLogos;
    FTBLogoMap m_logoMap;
    QMap<QString, LogoCallback> waitingCallbacks;

    void requestLogo(QString file);
    QString translatePackType(PackType type) const;


private slots:
    void logoFailed(QString logo);
    void logoLoaded(QString logo, QIcon out);

public:
    explicit ListModel(QObject *parent);
    ~ListModel();
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void fill(ModpackList modpacks);
    void addPack(Modpack modpack);
    void clear();
    void remove(int row);

    Modpack at(int row);
    void getLogo(const QString &logo, LogoCallback callback);
};

}
