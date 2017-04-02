#pragma once

#ifndef COMPONENT_DIALOG_H
#define COMPONENT_DIALOG_H

#include <QDialog>
#include "ui_ComponentDialog.h"

class ComponentDialog : public QDialog, public Ui::Dialog
{
  Q_OBJECT

public:
  ComponentDialog(QWidget * parent = 0);
};

#endif