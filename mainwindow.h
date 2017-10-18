#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QThread>
#include <QMainWindow>
#include <QMessageBox>
#include <QTextStream>
#include "shared/commonthings.h"
#include "anLogger/src/anlogger.h"
#include "CanProtocol/src/canprotocol.h"
#include "CanBusWorker/src/canbusworker.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
signals:
    void ToCanBusWorker(const GlobalSignal &);
public slots:
    void FromCanBusWorker(const GlobalSignal &aGlobalSignal);
private slots:
    void on_pushButtonClose_clicked();

    void on_pushButtonForward_clicked();

    void on_pushButtonBackward_clicked();

    void on_pushButtonEditRFID_clicked();
private:
    bool setCPNo(quint8 anIndex);
    void displayCurrentCP();

    Ui::MainWindow *ui;
    QList<CanProtocol> stations;
    CanProtocol * currentCP = nullptr;
    quint8 currentCPIndex;
};

#endif // MAINWINDOW_H
