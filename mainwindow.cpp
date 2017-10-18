#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QString tmpRFID;
    QByteArray tmpTail;
    for (quint8 i=0;i<=8;++i)
    {
        for (quint8 j=0;j<=6;++j)
        {
            tmpRFID.clear();
            tmpRFID ="a" + QString::number(i) + "b" + QString::number(j)
                    +"c" + QString::number(i) + "d" + QString::number(j);
            tmpTail.clear();
            tmpTail=QByteArray::fromHex(tmpRFID.toLocal8Bit());
            tmpTail<<quint16(0x0000)<<quint8(0x00)<<quint8((i%2)<<2);
            stations.append(CanProtocol((i<<4)|j,tmpTail));
        }
    }
    setCPNo(0);
    displayCurrentCP();

    QThread * aThread = new QThread();
    CanBusWorker * aCommunicator = new CanBusWorker();
    aCommunicator->setObjectName(QStringLiteral(""));
    aCommunicator->moveToThread(aThread);
    QObject::connect(aCommunicator, &CanBusWorker::Out, this, &MainWindow::FromCanBusWorker,
                     uniqueQtConnectionType);
    QObject::connect(this, &MainWindow::ToCanBusWorker,aCommunicator, &CanBusWorker::In,
                     uniqueQtConnectionType);
    QObject::connect(aThread, &QThread::started, aCommunicator, &CanBusWorker::start,
                     uniqueQtConnectionType);
    aThread->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::FromCanBusWorker(const GlobalSignal &aGlobalSignal)
{
    QString aGlobalSignalTypeTypeName = aGlobalSignal.Type.typeName();
    if (aGlobalSignalTypeTypeName == QStringLiteral("CanBusWorkerBasis::Data"))
    {
        switch (aGlobalSignal.Type.toInt()) {
        case CanBusWorkerBasis::replyFrameWithTimeStamp:
        {
            anAck("CanBusWorkerBasis::replyFrameWithTimeStamp");
            CanProtocol tmpCP(aGlobalSignal.Data.value<QCanBusFrame>());
            ui->textEditReception->append(aGlobalSignal.TimeStamp + " : " + tmpCP.getMsgStr());
            QString tmpCPMsgMean=tmpCP.getMsgMean();
            anInfo(tmpCPMsgMean);
            if (tmpCPMsgMean==QStringLiteral("Presence request"))
            {
                for (quint8 i=0;i<=8;++i)
                {
                    anAck("Sdcs " << i << " Presence Response Sent !");
                    GlobalSignal sendPresenceResponse;
                    sendPresenceResponse.Type = QVariant::fromValue(CanBusWorkerBasis::requestFrameTransmission);
                    sendPresenceResponse.Data = QVariant::fromValue(CanProtocol::PresenceResponse(i).getMsg());
                    emit ToCanBusWorker(sendPresenceResponse);
                }
            }
            else if (tmpCPMsgMean==QStringLiteral("Data request"))
            {
                quint8 currentSdcsIdInDataRequest = tmpCP.getSdcsId();
                QString SdcsIdStr = QString::number(currentSdcsIdInDataRequest);
                foreach (CanProtocol tmpCanProtocol, stations)
                {
                    if (tmpCanProtocol.getSdcsId()==currentSdcsIdInDataRequest)
                    {
                        anAck("Sdcs " + SdcsIdStr +
                              " Channel " + QString::number(tmpCanProtocol.getChId()) +
                              " Data Response Sent !");
                        GlobalSignal sendDataResponse;
                        sendDataResponse.Type = QVariant::fromValue(CanBusWorkerBasis::requestFrameTransmission);
                        sendDataResponse.Data = QVariant::fromValue(tmpCanProtocol.getMsg());
                        emit ToCanBusWorker(sendDataResponse);
                    }
                }
            }
            else
            {
                anWarn("Unexpected Can Message Received !");
                anInfo(tmpCP.getMsgStr());
            }
            break;
        }
        default:
            break;
        }
    }
    else if (aGlobalSignalTypeTypeName == QStringLiteral("CanBusWorkerBasis::Error"))
    {
        anError(QString(CanBusWorkerBasis::ErrorMetaEnum.valueToKey(aGlobalSignal.Type.toInt())));
        ui->textEditReception->append(aGlobalSignal.Data.toString());
        ui->statusBar->showMessage(QString(CanBusWorkerBasis::ErrorMetaEnum.valueToKey(aGlobalSignal.Type.toInt())));
    }
    else if (aGlobalSignalTypeTypeName == QStringLiteral("CanBusWorkerBasis::Notification"))
    {
        switch (aGlobalSignal.Type.toInt()) {
        case CanBusWorkerBasis::readyToWork:
        {
            anAck("CanBusWorkerBasis::readyToWork");
            ui->textEditReception->append(aGlobalSignal.Data.toString() + " Is Ready To Work !");
            ui->statusBar->showMessage(aGlobalSignal.Data.toString() + " Is Ready To Work !");
            break;
        }
        case CanBusWorkerBasis::FrameWritten:
        {
            anAck("CanBusWorkerBasis::FrameWritten");
            QCanBusFrame tmp = aGlobalSignal.Data.value<QCanBusFrame>();
            ui->textEditTransmission->append(CanProtocol(tmp).getMsgStr());
            break;
        }
        default:
            break;
        }
    }
}

void MainWindow::on_pushButtonClose_clicked()
{
    qApp->quit();
}

void MainWindow::on_pushButtonForward_clicked()
{
    if (!setCPNo(currentCPIndex+1))
        setCPNo(0);
    displayCurrentCP();
}

bool MainWindow::setCPNo(quint8 anIndex)
{
    if (anIndex<=(stations.size()-1))
    {
        currentCPIndex=anIndex;
        currentCP=&(stations[anIndex]);
        return true;
    }
    return false;
}

void MainWindow::displayCurrentCP()
{
    ui->spinBoxSdcsID->setValue(currentCP->getSdcsId());
    ui->spinBoxChID->setValue(currentCP->getChId());
    ui->plainTextEditRFID->setPlainText(QString(currentCP->getRFID().toHex()));
    ui->checkBoxValveOpen->setChecked(currentCP->getValveStatus()>0?true:false);
    ui->pushButtonEditRFID->setText("Edit RFID");
}

void MainWindow::on_pushButtonBackward_clicked()
{
    if (!setCPNo(currentCPIndex-1))
        setCPNo(stations.size()-1);
    displayCurrentCP();
}

void MainWindow::on_pushButtonEditRFID_clicked()
{
    if (ui->pushButtonEditRFID->text()=="Edit RFID")
    {
        ui->pushButtonEditRFID->setText("Save");
        ui->plainTextEditRFID->setReadOnly(false);
    }
    else if (ui->pushButtonEditRFID->text()=="Save")
    {
        if (ui->plainTextEditRFID->toPlainText().size()<=8)
        {
            anWarn("RFID Changed !");
            anInfo("Old Info: " << currentCP->getMsgStr());
            currentCP->setRFID(QByteArray::fromHex(ui->plainTextEditRFID->toPlainText().toLocal8Bit()));
            anInfo("New Info: " << currentCP->getMsgStr());
            anAck("RFID Change Notification Queued !");
            GlobalSignal notifyRFIDChange;
            notifyRFIDChange.Type = QVariant::fromValue(CanBusWorkerBasis::requestFrameTransmission);
            notifyRFIDChange.Data = QVariant::fromValue(currentCP->getMsg());
            notifyRFIDChange.Priority = 10;
            emit ToCanBusWorker(notifyRFIDChange);
        }
        else
        {
            ui->plainTextEditRFID->setPlainText(QString(currentCP->getRFID().toHex()));
            QMessageBox::information(this,"ERROR","Invalid RFID");
        }
        ui->pushButtonEditRFID->setText("Edit RFID");
        ui->plainTextEditRFID->setReadOnly(true);
    }
}
