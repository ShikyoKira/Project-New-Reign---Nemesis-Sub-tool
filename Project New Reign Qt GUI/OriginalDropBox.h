#pragma once

#include <qmessagebox.h>
#include <QtWidgets/QGraphicsView>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QList>

class OriginalDropBox : public QGraphicsView
{
public:
	OriginalDropBox(QWidget* widget) : QGraphicsView(widget){}

protected:
	void dragEnterEvent(QDragEnterEvent *e)
	{
		if (e->mimeData()->hasUrls()) {
			e->acceptProposedAction();
		}
	}

	void dropEvent(QDropEvent *e)
	{
		foreach(const QUrl &url, e->mimeData()->urls()) {
			QString fileName = url.toLocalFile();
			QMessageBox msgbox;
			msgbox.setWindowTitle("Message Title");
			msgbox.setText(fileName);
			msgbox.exec();
		}
	}

};
