#ifndef QSPRITELISTBOX_H
#define QSPRITELISTBOX_H

#include <QListWidget>

class QSpriteListBox : public QListWidget {
	Q_OBJECT

public:
	QSpriteListBox( QWidget* parent = 0 );
	~QSpriteListBox();

private:

protected:
	void startDrag( Qt::DropActions actions );

};

#endif // QSPRITELISTBOX_H
