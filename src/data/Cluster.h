#ifndef CLUSTER_H
#define CLUSTER_H

#include <QVector>
#include <QColor>

// This class represents a cluster of spots either loaded by the user
// or generated with the AnalysisClustering Widget.
// The cluster has properties such as color, visible and name so users
// can interact with them
class Cluster
{

public:

    typedef QList<QString> ClusterType;

    Cluster();
    explicit Cluster(const Cluster &other);
    ~Cluster();

    Cluster &operator=(const Cluster &other);
    bool operator==(const Cluster &other) const;

    // the spot ids contained in the cluster
    const ClusterType &spots() const;
    // the cluster name/info
    const QString name() const;
    // the cluster's color
    const QColor color() const;
    // true if the spot is visible
    bool visible() const;
    // the number of elements in the cluster
    int size() const;

    // Setters
    void spots(const ClusterType &spots);
    void name(const QString name);
    void color(const QColor color);
    void visible(const bool visible);

private:

    ClusterType m_spots;
    QString m_name;
    QColor m_color;
    bool m_visible;
};

#endif // CLUSTER_H
