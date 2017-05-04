#pragma once

#ifndef AUDIO_DRIVER_DIALOG_H
#define AUDIO_DRIVER_DIALOG_H

#include <QDialog>
#include "ui_AudioDriverDialog.h"

class AudioDriverDialog : public QDialog, public Ui::DriverDialog
{
  Q_OBJECT

public:
  AudioDriverDialog(QWidget * parent = 0);
};

#endif