#include "ProjectNewReignQtGUI.h"
#include <QGraphicsPixmapItem>

ProjectNewReignQtGUI::ProjectNewReignQtGUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	QImage image("image.gif");
	QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(image));

}
