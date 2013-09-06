#include "AnimationSelectorWindow.h"
#include <QDir>

AnimationSelectorWindow::AnimationSelectorWindow( QWidget* parent )
	: QMainWindow( parent ) {
	ui.setupUi( this );

	connect( ui.refreshButton, SIGNAL( clicked() ), this, SLOT( refresh() ) );

	refresh();
}

AnimationSelectorWindow::~AnimationSelectorWindow() {

}

void AnimationSelectorWindow::refresh() {
	QDir dir( "anims" );
	QStringList filter( "*.xml" );
	QStringList files = dir.entryList( filter );

	ui.animationListBox->clear();
	for( int i = 0; i < files.size(); ++i ) {
		ui.animationListBox->addItem( files.at( i ) );
	}
}
