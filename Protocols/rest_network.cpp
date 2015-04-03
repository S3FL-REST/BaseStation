#include "rest_network.h"

////////////////////////////////////////////////////////////////////////////////
/// Network2Rover
////////////////////////////////////////////////////////////////////////////////

Network2Rover::Network2Rover() : left_joystick(0), right_joystick(0), current_mode(STOP),
                                 bin_actuator(L_STOP), scoop_actuator(L_STOP), rate(0) {
    //Empty Constructor
}

void Network2Rover::SetLeftJoystick(int lJoy) {
    left_joystick = lJoy;
}

void Network2Rover::SetRightJoystick(int rJoy) {
    right_joystick = rJoy;
}

void Network2Rover::SetRunMode(run_mode newMode) {
    current_mode = newMode;
}

void Network2Rover::SetBinActuator(linear_actuator l) {
    bin_actuator = l;
}

void Network2Rover::SetScoopActuator(linear_actuator l) {
    scoop_actuator = l;
}

void Network2Rover::SetArmRate(int r) {
    rate = r;
}

int Network2Rover::GetLeftJoystick() const {
    return left_joystick;
}

int Network2Rover::GetRightJoystick() const {
    return right_joystick;
}

Network2Rover::run_mode Network2Rover::GetRunMode() const {
    return current_mode;
}

Network2Rover::linear_actuator Network2Rover::GetBinActuator() const {
    return bin_actuator;
}

Network2Rover::linear_actuator Network2Rover::GetScoopActuator() const {
    return scoop_actuator;
}

int Network2Rover::GetArmRate() const {
    return rate;
}

QByteArray Network2Rover::ToByteArray() const {
    return QString("%1:%2:%3\n").arg(QString::number(left_joystick),
                                     QString::number(right_joystick),
                                     QString::number(static_cast<int>(current_mode)),
                                     QString::number(static_cast<int>(bin_actuator)),
                                     QString::number(static_cast<int>(scoop_actuator)),
                                     QString::number(rate)).toUtf8();
}

bool Network2Rover::ParseData(QByteArray &data) {
    QStringList list = QString(data).split(":");

    if (list.size() < NUM_PARAMS) return false;

    left_joystick = list.at(0).toInt();
    right_joystick = list.at(1).toInt();
    current_mode = static_cast<run_mode>(list.at(2).toInt());
    bin_actuator = static_cast<Network2Rover::linear_actuator>(list.at(3).toInt());
    scoop_actuator = static_cast<Network2Rover::linear_actuator>(list.at(4).toInt());
    rate = list.at(5).toInt();

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// Network2Base
////////////////////////////////////////////////////////////////////////////////

Network2Base::Network2Base() {

}

void Network2Base::SetImage(QImage &image_in) {
    image = image_in;
}

QImage Network2Base::GetImage() const {
    return image;
}

QByteArray Network2Base::ToByteArray() const {
    QByteArray byteArray;

    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);

    image.save(&buffer, "JPG");

    byteArray.push_back('\n');

    return byteArray;
}

bool Network2Base::ParseData(QByteArray &data) {
    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);

    return image.load(&buffer, "JPG");
}


