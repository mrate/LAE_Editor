#include "qspritelistbox.h"
#include <QDrag>
#include <QMimeData>

QSpriteListBox::QSpriteListBox( QWidget* parent )
	: QListWidget( parent ) {

}

QSpriteListBox::~QSpriteListBox() {

}

void QSpriteListBox::startDrag( Qt::DropActions actions ) {
	QMimeData* mimeData = new QMimeData;
	mimeData->setData( "x-application/lae_sprite", currentItem()->text().toLocal8Bit() );
	QDrag* drag = new QDrag( this );
	drag->setMimeData( mimeData );
	drag->exec( Qt::CopyAction );
}
