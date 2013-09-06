#include "SpriteSelectorWindow.h"
#include <QImage>
#include <QDir>

SpriteSelectorWindow::SpriteSelectorWindow( QWidget* parent )
	: QMainWindow( parent ) {
	ui.setupUi( this );

	previewLabel = new QLabel();
	previewLabel->setAlignment( Qt::AlignCenter );
	ui.scrollArea->setWidget( previewLabel );

	connect( ui.refreshButton, SIGNAL( clicked() ), this, SLOT( refreshSprites() ) );
	connect( ui.spriteListBox, SIGNAL( currentRowChanged( int ) ), this, SLOT( spriteChanged( int ) ) );

	// fill file list
	refreshSprites();
}

SpriteSelectorWindow::~SpriteSelectorWindow() {

}

void SpriteSelectorWindow::refreshSprites() {
	ui.spriteListBox->clear();

	QDir dir( "gfx\\" );
	QStringList filter( "*.png" );
	QStringList files = dir.entryList( filter );

	for( int i = 0; i < files.size(); ++i ) {
		ui.spriteListBox->addItem( files.at( i ) );
	}

	//ui.spriteListBox->addItem("m1_bg.png");
}

void SpriteSelectorWindow::spriteChanged( int row ) {
	if( row < 0 ) {
		return ;
	}
	QImage image( "gfx\\" + ui.spriteListBox->currentItem()->text() );
	previewLabel->setPixmap( QPixmap::fromImage( image ) );
	previewLabel->resize( image.width(), image.height() );
}
