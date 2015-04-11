#include "mainwindow.h"
#include "ui_mainwindow.h"

const int JOYSTICK_NUMBER = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    js(JOYSTICK_NUMBER), left(0.0), right(0.0),
    binActuator(Network2Rover::L_STOP), scoopActuator(Network2Rover::L_STOP), suspensionActuator(Network2Rover::L_STOP)
{
    ui->setupUi(this);

    connect(ui->StopButton, SIGNAL(clicked()), this, SLOT(StopPressed()));
    connect(ui->TeleopButton, SIGNAL(clicked()), this, SLOT(TeleopPressed()));
    connect(ui->SemiAutonButton, SIGNAL(clicked()), this, SLOT(SemiAutonPressed()));
    connect(ui->FullAutonButton, SIGNAL(clicked()), this, SLOT(FullAutonPressed()));

    connect(ui->ConnectButton, SIGNAL(clicked()), this, SLOT(ConnectPressed()));
    connect(ui->DisconnectButton, SIGNAL(clicked()), this, SLOT(DisconnectPressed()));

    connect(&joystickTimer, SIGNAL(timeout()), this, SLOT(JoystickUpdate()));

    connect(&socket, SIGNAL(readyRead()), this, SLOT(DataReady()));

    if (js.isFound()) {
        joystickTimer.start(10);
    } else {
        qDebug() << "Joystick not connected";
        QMessageBox::information(this, "Joystick", "No Joystick Connected");
    }

    connect(&networkTimer, SIGNAL(timeout()), this, SLOT(NetworkUpdate()));
}

void MainWindow::JoystickUpdate() {
    double limit = SHRT_MAX;

    JoystickEvent event;

    while (js.sample(&event)) {
        if (event.isInitialState()) continue;

        if (event.isAxis()) {
            if (event.number == 4) right = (floor(10 * event.value / limit) / 10.0);
            else if (event.number == 1) left = (floor(10 * event.value / limit) / 10.0);

            //Arm Rate -> Event number and rates need updating and validating
            else if (event.number == 3) armRate = 100 * event.value / limit;

            if (abs(right * 255.0) < 30) right = 0;
            if (abs(left * 255.0) < 30) left = 0;
        } if (event.isButton()) {

            //Bin Actuator -> Event numbers need changing
            if (event.number == 1)
                binActuator = (event.value == 0) ? Network2Rover::L_STOP : Network2Rover::L_FORWARD;
            else if (event.number == 2)
                binActuator = (event.value == 0) ? Network2Rover::L_STOP : Network2Rover::L_REVERSE;

            //Scoop Actuator -> Event numbers need changing
            if (event.number == 3)
                scoopActuator = (event.value == 0) ? Network2Rover::L_STOP : Network2Rover::L_FORWARD;
            else if (event.number == 4)
                scoopActuator = (event.value == 0) ? Network2Rover::L_STOP : Network2Rover::L_REVERSE;

            //Suspension Actuator -> Event numbers need changing
            if (event.number == 5 && event.value)
                suspensionActuator = Network2Rover::L_FORWARD;
            else if (event.number == 6 && event.value)
                suspensionActuator = Network2Rover::L_REVERSE;
        }
    }
}

void MainWindow::NetworkUpdate() {
    networkData.SetLeftJoystick(static_cast<int>(left * 255));
    networkData.SetRightJoystick(static_cast<int>(right * 255));
    networkData.SetBinActuator(binActuator);
    networkData.SetScoopActuator(scoopActuator);
    networkData.SetSuspension(suspensionActuator);
    networkData.SetArmRate(armRate);

    if (socket.state() != QAbstractSocket::ConnectedState) {
        qDebug() << "Socket Not Connected";
        networkTimer.stop();
    }

    socket.write(networkData.ToByteArray());
    socket.flush();
}

//Button Code

void MainWindow::StopPressed() {
    networkData.SetRunMode(Network2Rover::STOP);
}

void MainWindow::TeleopPressed() {
    networkData.SetRunMode(Network2Rover::TELEOP);
}

void MainWindow::SemiAutonPressed() {
    networkData.SetRunMode(Network2Rover::SAFE_AUTON);
}

void MainWindow::FullAutonPressed() {
    networkData.SetRunMode(Network2Rover::FULL_AUTON);
}

void MainWindow::DisconnectPressed() {
    if (networkTimer.isActive()) {
        networkTimer.stop();
        socket.disconnectFromHost();

        ui->imageViewLabel->clear();
    }
}

void MainWindow::ConnectPressed() {
    if (!networkTimer.isActive()) {
        networkData.SetLeftJoystick(0);
        networkData.SetRightJoystick(0);

        if (networkData.GetRunMode() != Network2Rover::FULL_AUTON)
            networkData.SetRunMode(Network2Rover::STOP);

        socket.connectToHost(ui->IPAddress->text(), 3141);

        if (!socket.waitForConnected()) {
            QMessageBox::information(this, "Connection", "Unable to Connnect to " + ui->IPAddress->text());
        } else {
            networkTimer.start(100);
        }
    }
}

void MainWindow::DataReady() {
    QByteArray data = socket.readAll();

    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);

    QImage image;
    image.load(&buffer, "JPG");

    ui->imageViewLabel->setPixmap(QPixmap::fromImage(image));
}

MainWindow::~MainWindow() {
    delete ui;
}
