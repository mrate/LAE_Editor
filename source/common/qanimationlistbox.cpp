#include "qanimationlistbox.h"
#include <QMimeData>
#include <QDrag>

QAnimationListBox::QAnimationListBox( QWidget* parent )
	: QListWidget( parent ) {

}

QAnimationListBox::~QAnimationListBox() {

}

void QAnimationListBox::startDrag( Qt::DropActions actions ) {
	QMimeData* mimeData = new QMimeData;
	mimeData->setData( "x-application/lae_animation", currentItem()->text().toLocal8Bit() );
	QDrag* drag = new QDrag( this );
	drag->setMimeData( mimeData );
	drag->exec( Qt::CopyAction );
}
