#ifndef REST_NETWORK_H
#define REST_NETWORK_H

#include <QString>
#include <QStringList>

#include <QByteArray>

#include <QImage>
#include <QBuffer>
#include <QDataStream>

enum run_mode {
  STOP = 0,
  TELEOP = 1,
  SAFE_AUTON = 2,
  FULL_AUTON = 3
};

class rest_network
{
public:
    rest_network();

    void SetLeftJoystick(int);
    void SetRightJoystick(int);
    void SetRunMode(run_mode);

    int GetLeftJoystick() const;
    int GetRightJoystick() const;
    run_mode GetRunMode() const;

    QByteArray ToByteArray() const;
    bool ParseString(QString);


private:
    int left_joystick;
    int right_joystick;
    run_mode current_mode;
};

class rest_network_pic
{
public:
    rest_network_pic();

    void SetImage(QImage&);

    QImage GetImage() const;
    bool HasNewImage() const;

    QByteArray ToByteArray() const;
    bool ParseString(QString);

private:
    QImage image;

    bool new_picture;
};

#endif // REST_NETWORK_H
