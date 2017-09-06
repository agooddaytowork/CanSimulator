#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    stations(new QList<CanProtocol*>())
{
    ui->setupUi(this);
    invoker.setParent(this);
    invoker.setSingleShot(false);
    invoker.setInterval(0);
    QString tmpRFID;
    QByteArray tmpTail;
    for (quint8 i=0;i<=8;++i)
    {
        for (quint8 j=0;j<=6;++j)
        {
            tmpRFID.clear();
            tmpRFID ="a" + QString::number(i) + "b" + QString::number(j)
                    +"c" + QString::number(i) + "d" + QString::number(j)
                    +"e" + QString::number(i);
            tmpTail.clear();
            tmpTail=QByteArray::fromHex(tmpRFID.toLocal8Bit());
            tmpTail<<quint16(0x0000)<<quint8((i%2)<<2);
            stations->append(new CanProtocol((i<<4)|j,tmpTail));
        }
    }
    setCPNo(0);
    displayCurrentCP();

    QThread * aThread = new QThread();
    CanBusWorker * aCommunicator = new CanBusWorker();
    aCommunicator->moveToThread(aThread);
    QObject::connect(aCommunicator, &CanBusWorker::Out, this, &MainWindow::FromCanBusWorker,
                     CanBusWorkerDB::uniqueQtConnectionType);
    QObject::connect(this, &MainWindow::ToCanBusWorker,aCommunicator, &CanBusWorker::In,
                     CanBusWorkerDB::uniqueQtConnectionType);
    QObject::connect(aThread, &QThread::started, aCommunicator, &CanBusWorker::start,
                     CanBusWorkerDB::uniqueQtConnectionType);
    QObject::connect(&invoker, &QTimer::timeout, this, [&](){
        qApp->processEvents();
        while (!pendingSend.isEmpty())
        {
            emit ToCanBusWorker(QVariant::fromValue(CanBusWorkerDB::addAFrameIntoPendingFrameList),
                                QVariant::fromValue(pendingSend.takeFirst()));
        }
    });
    aThread->start();
    invoker.start();
}

MainWindow::~MainWindow()
{
    delete stations;
    delete ui;
}

void MainWindow::FromCanBusWorker(QVariant enumVar, QVariant dataVar)
{
    anTrk("From Can Bus");
    QString enumVarTypeName(enumVar.typeName());
    if (enumVarTypeName == QStringLiteral("CanBusWorkerDB::Data"))
    {
        anAck(enumVarTypeName);
        switch (enumVar.toInt()) {
        case CanBusWorkerDB::requestPluginAndInterface:
        {
            anInfo("requestPluginAndInterface");
            anAck("Plugin And Interface Info Replied !");
            CanBusWorkerDB::PluginNameAndInterfaceName tmp;
            tmp.first = QStringLiteral("socketcan");
            tmp.second = QStringLiteral("can0");
            anInfo("plg:"<< tmp.first <<",itf:"<< tmp.second);
            emit ToCanBusWorker(QVariant::fromValue(CanBusWorkerDB::replyPluginAndInterface),
                                QVariant::fromValue(tmp));
            break;
        }
        case CanBusWorkerDB::replyCanFrameWithTimeStamp:
        {
            anInfo("replyCanFrameWithTimeStamp");
            CanBusWorkerDB::CanBusFrameWithTimeStamp tmp =
                    dataVar.value<CanBusWorkerDB::CanBusFrameWithTimeStamp>();
            CanProtocol tmpCP(tmp.first);
            ui->textEditReception->append(tmp.second + " : " + tmpCP.getMsgStr());
            QString tmpCPMsgMean=tmpCP.getMsgMean();
            anAck("Can Message Classified !");
            anInfo(tmpCPMsgMean);
            if (tmpCPMsgMean==QStringLiteral("Presence request"))
            {
                for (quint8 i=0;i<=8;++i)
                {
                    anAck("Sdcs " << i << " Presence Response Sent !");
                    pendingSend.append(CanProtocol::PresenceResponse(i).getMsg());
                }
            }
            else if (tmpCPMsgMean==QStringLiteral("Data request"))
            {
                quint8 currentSdcsIdInDataRequest = tmpCP.getSdcsId();
                foreach (CanProtocol *tmpCanProtocol, *stations)
                {
                    if (tmpCanProtocol->getSdcsId()==currentSdcsIdInDataRequest)
                    {
                        pendingSend.append(tmpCanProtocol->getMsg());
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
    else if (enumVarTypeName == QStringLiteral("CanBusWorkerDB::Notification"))
    {
        anAck(enumVarTypeName);
        switch (enumVar.toInt()) {
        case CanBusWorkerDB::CanFrameTransmitted:
        {
            anInfo("CanFrameTransmitted");
            QCanBusFrame tmp = dataVar.value<QCanBusFrame>();
            ui->textEditTransmission->append(CanProtocol(tmp).getMsgStr());
            break;
        }
        case CanBusWorkerDB::DeviceCreated:
        {
            anInfo("DeviceCreated");
            ui->textEditTransmission->append("=> Device Created !");
            break;
        }
        case CanBusWorkerDB::DeviceConnected:
        {
            anInfo("DeviceConnected");
            ui->textEditTransmission->append("=> Device Connected !");
            break;
        }
        default:
            break;
        }
    }
    else if (enumVarTypeName == QStringLiteral("CanBusWorkerDB::Error"))
    {
        anAck(enumVarTypeName);
        ui->textEditReception->append(QString(CanBusWorkerDB::ErrorMetaEnum.valueToKey(enumVar.toInt())));
        if (!dataVar.isNull())
        {
            ui->textEditReception->append(dataVar.value<QString>());
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
    if (anIndex<=(stations->size()-1))
    {
        currentCPIndex=anIndex;
        currentCP=stations->at(anIndex);
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
        setCPNo(stations->size()-1);
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
        if (ui->plainTextEditRFID->toPlainText().size()<=10)
        {
            anAck("RFID Changed !");
            anInfo("Old Info: " << currentCP->getMsgStr());
            currentCP->setRFID(QByteArray::fromHex(ui->plainTextEditRFID->toPlainText().toLocal8Bit()));
            anInfo("New Info: " << currentCP->getMsgStr());
            anAck("RFID Change Notification Queued !");
            pendingSend.append(currentCP->getMsg());
        }
        else
        {
            QMessageBox::information(this,"ERROR","Invalid RFID");
        }
        ui->pushButtonEditRFID->setText("Edit RFID");
        ui->plainTextEditRFID->setReadOnly(true);
    }
}
