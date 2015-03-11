#include "rest_network.h"

rest_network::rest_network() : left_joystick(0), right_joystick(0), current_mode(STOP) {
    //Empty Constructor
}

void rest_network::SetLeftJoystick(int lJoy) {
    left_joystick = lJoy;
}

void rest_network::SetRightJoystick(int rJoy) {
    right_joystick = rJoy;
}

void rest_network::SetRunMode(run_mode newMode) {
    current_mode = newMode;
}

int rest_network::GetLeftJoystick() const {
    return left_joystick;
}

int rest_network::GetRightJoystick() const {
    return right_joystick;
}

run_mode rest_network::GetRunMode() const {
    return current_mode;
}

QByteArray rest_network::ToByteArray() const {
    return QString("%1:%2:%3\n").arg(QString::number(left_joystick), QString::number(right_joystick), QString::number(static_cast<int>(current_mode))).toUtf8();
    //return QString.sprintf("%d:%d:%d\n", left_joystick, right_joystick, static_cast<int>(current_mode));
}

bool rest_network::ParseString(QString data) {
    QStringList list = data.split(":");

    if (list.size() < 3) return false;

    left_joystick = list.at(0).toInt();
    right_joystick = list.at(1).toInt();
    current_mode = static_cast<run_mode>(list.at(2).toInt());

    return true;
}

rest_network_pic::rest_network_pic() : new_picture(false) {

}

void rest_network_pic::SetImage(QImage &image_in) {
    image = image_in;
}

QImage rest_network_pic::GetImage() const {
    return image;
}

bool rest_network_pic::HasNewImage() const {
    return new_picture;
}

QByteArray rest_network_pic::ToByteArray() const {
    QByteArray byteArray;

    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);

    image.save(&buffer, "JPG");

    byteArray.push_back('\n');

    return byteArray;
}

bool rest_network_pic::ParseString(QString data) {
    QByteArray byteArray = data.toUtf8();

    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::ReadOnly);

    return new_picture = image.load(&buffer, "JPG");
}


