#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/qmessagebox.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets//qboxlayout.h>
#include "ui_ProjectNewReignQtGUI.h"

class ProjectNewReignQtGUI : public QWidget
{
	Q_OBJECT

public:
	ProjectNewReignQtGUI(QWidget *parent = Q_NULLPTR);

private:
	Ui::ProjectNewReignQtGUIClass ui;
};
