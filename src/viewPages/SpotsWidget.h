#ifndef SPOTSWIDGET_H
#define SPOTSWIDGET_H

#include <QDockWidget>
#include <QIcon>

class QPushButton;
class QLineEdit;
class Dataset;
class SpotItemModel;
class SpotsTableView;
class QSortFilterProxyModel;
class QColorDialog;

// This widget is componsed of the spots table
// a search field and the select and action menus so the user can
// select/deselect spots and change their color.
// Every action will affect what spots are shown in the cell view.
// Here when a dataset is opened its unique spots will be shown
// so the user can interact with them to visualize them in the cell view
class SpotsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SpotsWidget(QWidget *parent = 0);
    virtual ~SpotsWidget();

    // clear focus/status and selections
    void clear();

    // forces an update of the table
    void updateModelTable();

signals:

    // signals emitted when the user selects or change colors of spots
    void signalSpotsUpdated();

public slots:
    // the user has opened a dataset and the spots must be updated
    // spots is a reference to the spots of the dataset
    void slotLoadDataset(const Dataset &dataset);

    // the user is loading a file with spot colors
    void slotLoadSpotColorsFile(const QString &filename);

    // the user wants to load a set colors
    void slotLoadSpotColors(const QVector<QColor> &colors);

private slots:

    // slots triggered by the show/color controls in the gene table
    void slotSetColorAllSelected(const QColor &color);
    void slotSetVisibilityForSelectedRows(bool visible);
    void slotHideAllSelected();
    void slotShowAllSelected();

private:
    // internal function to configure created buttons
    // to avoid code duplication
    // TODO better approach would be to have factories somewhere else
    void configureButton(QPushButton *button, const QIcon &icon, const QString &tooltip);

    // internal function to retrieve the model of the table
    QSortFilterProxyModel *getProxyModel();
    SpotItemModel *getModel();

    // some references needed to UI elements
    QScopedPointer<QLineEdit> m_lineEdit;
    QScopedPointer<SpotsTableView> m_spots_tableview;
    QScopedPointer<QColorDialog> m_colorList;

    Q_DISABLE_COPY(SpotsWidget)

};

#endif // SPOTSWIDGET_H
