/********************************************************************************
** Form generated from reading UI file 'ProjectNewReignQtGUI.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROJECTNEWREIGNQTGUI_H
#define UI_PROJECTNEWREIGNQTGUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>
#include "EditDropBox.h"
#include "OriginalDropBox.h"

QT_BEGIN_NAMESPACE

class Ui_ProjectNewReignQtGUIClass
{
public:
    OriginalDropBox *DropBox1;
    EditDropBox *DropBox2;

    void setupUi(QWidget *ProjectNewReignQtGUIClass)
    {
        if (ProjectNewReignQtGUIClass->objectName().isEmpty())
            ProjectNewReignQtGUIClass->setObjectName(QStringLiteral("ProjectNewReignQtGUIClass"));
        ProjectNewReignQtGUIClass->resize(661, 320);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ProjectNewReignQtGUIClass->sizePolicy().hasHeightForWidth());
        ProjectNewReignQtGUIClass->setSizePolicy(sizePolicy);
        ProjectNewReignQtGUIClass->setMinimumSize(QSize(0, 0));
        ProjectNewReignQtGUIClass->setMaximumSize(QSize(661, 320));
        ProjectNewReignQtGUIClass->setAcceptDrops(true);
		ProjectNewReignQtGUIClass->setStyleSheet("background: transparent");

        DropBox1 = new OriginalDropBox(ProjectNewReignQtGUIClass);
        DropBox1->setObjectName(QStringLiteral("DropBox1"));
        DropBox1->setEnabled(true);
        DropBox1->setGeometry(QRect(20, 20, 301, 281));
        DropBox1->setContextMenuPolicy(Qt::CustomContextMenu);
        DropBox1->setAcceptDrops(true);
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::NoBrush);
        DropBox1->setForegroundBrush(brush);
		DropBox1->setStyleSheet("background: transparent");

        DropBox2 = new EditDropBox(ProjectNewReignQtGUIClass);
        DropBox2->setObjectName(QStringLiteral("DropBox2"));
        DropBox2->setEnabled(true);
        DropBox2->setGeometry(QRect(340, 20, 301, 281));
        DropBox2->setContextMenuPolicy(Qt::CustomContextMenu);
        DropBox2->setAcceptDrops(true);
        QBrush brush1(QColor(0, 0, 0, 255));
        brush1.setStyle(Qt::NoBrush);
        DropBox2->setForegroundBrush(brush1);

        retranslateUi(ProjectNewReignQtGUIClass);

        QMetaObject::connectSlotsByName(ProjectNewReignQtGUIClass);
    } // setupUi

    void retranslateUi(QWidget *ProjectNewReignQtGUIClass)
    {
        ProjectNewReignQtGUIClass->setWindowTitle(QApplication::translate("ProjectNewReignQtGUIClass", "ProjectNewReignQtGUI", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ProjectNewReignQtGUIClass: public Ui_ProjectNewReignQtGUIClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROJECTNEWREIGNQTGUI_H
