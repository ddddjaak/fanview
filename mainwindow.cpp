#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QDateTime>

QString buf;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //ui->setupUi(this);
    //查找可用的串口
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);
        if(serial.open(QIODevice::ReadWrite))
        {
            ui->PortBox->addItem(serial.portName());
            serial.close();
        }
    }
    ui->sendButton->setEnabled(false);
    ui->clearButton->setEnabled(false);
    ui->pushButton_save->setEnabled(false);
    //设置波特率下拉菜单默认显示第三项
    //->BaudBox->setCurrentIndex(3);
    //关闭发送按钮的使能
    //ui->sendButton->setEnabled(false);
    qDebug() << tr("界面设定成功！");
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_openButton_clicked()
{
    if(ui->openButton->text()==tr("打开串口"))
    {

        ui->openButton->setText(tr("关闭串口"));
        serial = new QSerialPort;
        //设置串口名
        serial->setPortName(ui->PortBox->currentText());
        //打开串口
        serial->open(QIODevice::ReadWrite);
        //serial->setBaudRate(115200);
        serial->setBaudRate(ui->BaudBox->currentText().toInt());
        //设置波特率
        //serial->setBaudRate(ui->BaudBox->currentText().toInt());
        //设置数据位数
        switch(ui->BitNumBox->currentText().toInt())
        {
            case 8: serial->setDataBits(QSerialPort::Data8); break;
            default: break;
        }
        serial->setDataBits(QSerialPort::Data8);
        //设置奇偶校验
        switch(ui->ParityBox->currentText().toInt())
        {
            case 0: serial->setParity(QSerialPort::NoParity); qDebug() << tr("无奇偶校验NoParity  "); break;
            default: break;
        }
        //设置停止位
        //qDebug("   %d",ui->StopBox->currentText().toStdString());// << tr("asd");
        switch(ui->StopBox->currentText().toInt())
        {
            case 1: serial->setStopBits(QSerialPort::OneStop); qDebug() << tr("设置了1个停止位！  "); break;
            case 2: serial->setStopBits(QSerialPort::TwoStop); qDebug() << tr("设置了2个停止位！  "); break;
            default:
            qDebug() << tr("设置停止位fail！  ");
            break;
        }
        //设置流控制
        serial->setFlowControl(QSerialPort::NoFlowControl);
        //关闭设置菜单使能
        ui->PortBox->setEnabled(false);
        ui->BaudBox->setEnabled(false);
        ui->BitNumBox->setEnabled(false);
        ui->ParityBox->setEnabled(false);
        ui->StopBox->setEnabled(false);
        ui->openButton->setText(tr("关闭串口"));
        ui->sendButton->setEnabled(true);
        ui->sendButton->setEnabled(true);
        ui->clearButton->setEnabled(true);
        ui->pushButton_save->setEnabled(true);
        //连接信号槽
        QObject::connect(serial, &QSerialPort::readyRead, this, &MainWindow::ReadData);
        //QObject::connect(serial, &QSerialPort::readyRead, this, &MainWindow::Read_Data);
    }
    else
    {
        ui->openButton->setText(tr("打开串口"));
        serial->clear();
        serial->close();
        serial->deleteLater();
        //恢复设置使能
        ui->PortBox->setEnabled(true);
        ui->BaudBox->setEnabled(true);
        ui->BitNumBox->setEnabled(true);
        ui->ParityBox->setEnabled(true);
        ui->StopBox->setEnabled(true);
        ui->sendButton->setEnabled(false);
        ui->sendButton->setEnabled(false);
        ui->clearButton->setEnabled(false);
        ui->pushButton_save->setEnabled(false);
    }
}


void MainWindow::ReadData()
{
    buf = serial->readAll();
    qDebug() << buf;
    //buf += "\n";
    ui->receiveEdit->append(buf);
}

void MainWindow::on_sendButton_clicked()
{
    QString buff = ui->sendEdit->toPlainText();
    QByteArray by2 = buff.toLocal8Bit();
    serial->write(by2);
    qDebug() << tr("发送数据OK");
}




void MainWindow::on_clearButton_clicked()
{
    ui->receiveEdit->clear();
}




void MainWindow::on_pushButton_save_clicked()
{
    QString log_info = ui->receiveEdit->toPlainText();
    QByteArray log_ba = log_info.toLocal8Bit();
    ota_log(log_ba.data(), log_ba.length());
}

void MainWindow::ota_log(char *msg,quint32 len)
{
    QString log_path = "logs";
    QDir dir;
    if (!dir.exists(log_path))
    {
        bool res = dir.mkpath(log_path);
        if (res != true)
        {
            ui->textEdit_logger->setPlainText("【提示】log 文件夹创建失败。\r\n");
            return;
        }
    }
    QDateTime date_now = QDateTime::currentDateTime(); //获取系统现在的时间
    QString str_now = date_now.toString("yyyy_MM_dd_hh_mm_ss");
    QString filename = log_path + "\\" + "ota_" + str_now + ".log";
    QFile file(filename);
    file.open(QIODevice::ReadWrite|QIODevice::Text);
    file.write((const char *)msg,len);
    file.close();
    ui->textEdit_logger->insertPlainText("【提示】日志保存成功。\r\n");
}
