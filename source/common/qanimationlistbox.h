#ifndef QANIMATIONLISTBOX_H
#define QANIMATIONLISTBOX_H

#include <QListWidget>

class QAnimationListBox : public QListWidget {
	Q_OBJECT

public:
	QAnimationListBox( QWidget* parent = 0 );
	~QAnimationListBox();

private:

protected:
	void startDrag( Qt::DropActions actions );

};

#endif // QANIMATIONLISTBOX_H
