#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QThread>
#include <QMainWindow>
#include <QMessageBox>
#include "CanBusWorker/canbusworker.h"
#include <QTextStream>
#include "canprotocol.h"
#include "anlogger.h"
#include "commonthings.h"
#include <QTimer>

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
    void ToCanBusWorker(QVariant, QVariant = QVariant());
public slots:
    void FromCanBusWorker(QVariant enumVar, QVariant dataVar = QVariant());
private slots:
    void on_pushButtonClose_clicked();

    void on_pushButtonForward_clicked();

    void on_pushButtonBackward_clicked();

    void on_pushButtonEditRFID_clicked();
private:
    bool setCPNo(quint8 anIndex);
    void displayCurrentCP();

    Ui::MainWindow *ui;
    QList<CanProtocol*> * stations;
    CanProtocol * currentCP = Q_NULLPTR;
    quint8 currentCPIndex;

    QList<QCanBusFrame> pendingSend;

    QTimer invoker;
};

#endif // MAINWINDOW_H
