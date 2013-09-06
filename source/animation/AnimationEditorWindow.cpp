#include "AnimationEditorWindow.h"
#include <LAE_Core/cache/SoundCache.h>
// qt
#include <QFileDialog>
#include <QGridLayout>
#include <QMessageBox>
// stl
#include <algorithm>

using namespace LAE;

AnimationEditorWindow::AnimationEditorWindow( QGLContext* context, QWidget* parent )
	: QMainWindow( parent )
	, modified( false )
	, playbackFrame(0)
	, playbackTimer( this )
	, context( context )
	, actionPlay(nullptr)
	, actionStop(nullptr)
	, actionLoop(nullptr)
	, actionNew(nullptr)
	, actionOpen(nullptr)
	, actionSave(nullptr)
	, actionExit(nullptr)
	, actionShowSelection(nullptr)
	, actionShowCrosses(nullptr) {
	ui.setupUi( this );
	//resize(1024, 768);

	// glWidget
	aeWidget = new AnimationEditorWidget( this, animation, context );
	aeWidget->resize( 1024, 1024 );
	ui.scrollArea->setWidget( aeWidget );

	// action
	connectActions();
	createToolBar();
	createMenu();

	newAnimation();

	connect( aeWidget, SIGNAL( loadAnimation( const QString& ) ), this, SLOT( loadAnimation( const QString& ) ) );
	connect( ui.frameListBox, SIGNAL( currentRowChanged( int ) ), this, SLOT( frameListBoxChanged( int ) ) );
	connect( ui.showPrevFrameBox, SIGNAL( stateChanged( int ) ), this, SLOT( showPrevChanged( int ) ) );
	connect( ui.spriteListBox, SIGNAL( currentRowChanged( int ) ), this, SLOT( spriteListBoxChanged( int ) ) );
	connect( ui.addFrameButton, SIGNAL( clicked() ), this, SLOT( addFrame() ) );
	connect( ui.removeFrameButton, SIGNAL( clicked() ), this, SLOT( removeFrame() ) );
	connect( ui.cloneFrameButton, SIGNAL( clicked() ), this, SLOT( cloneFrame() ) );
	connect( ui.moveFrameUpButton, SIGNAL( clicked() ), this, SLOT( moveFrameUp() ) );
	connect( ui.moveFrameDownButton, SIGNAL( clicked() ), this, SLOT( moveFrameDown() ) );
	connect( ui.moveSpriteUpButton, SIGNAL( clicked() ), this, SLOT( moveSpriteUp() ) );
	connect( ui.moveSpriteDownButton, SIGNAL( clicked() ), this, SLOT( moveSpriteDown() ) );
	connect( ui.removeSpriteButton, SIGNAL( clicked() ), this, SLOT( removeSprite() ) );
	connect( ui.soundCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( soundComboChanged( int ) ) );
	connect( ui.refreshSoundsButton, SIGNAL( clicked() ), this, SLOT( refreshSounds() ) );

	connect( ui.delaySpinBox, SIGNAL( valueChanged( int ) ), this, SLOT( delayChanged( int ) ) );
	connect( ui.xPosBox, SIGNAL( valueChanged( int ) ), this, SLOT( xPosChanged( int ) ) );
	connect( ui.yPosBox, SIGNAL( valueChanged( int ) ), this, SLOT( yPosChanged( int ) ) );

	connect( aeWidget, SIGNAL( xPosChanged( int ) ), this, SLOT( xPosChangedMouse( int ) ) );
	connect( aeWidget, SIGNAL( yPosChanged( int ) ), this, SLOT( yPosChangedMouse( int ) ) );
	connect( aeWidget, SIGNAL( frameUpdate() ), this, SLOT( frameUpdate() ) );

	playbackTimer.setSingleShot( true );
	connect( &playbackTimer, SIGNAL( timeout() ), this, SLOT( playbackTimerTick() ) );

	refreshSounds();
}

AnimationEditorWindow::~AnimationEditorWindow() {

}

void AnimationEditorWindow::closeEvent( QCloseEvent* event ) {
	if( modified ) {
		event->ignore();
	} else {
		event->accept();
	}
}

void AnimationEditorWindow::connectActions() {
	actionPlay = new QAction( QIcon( ":/images/play" ), "Play", this );
	actionPlay->setToolTip( "Play" );
	actionPlay->setStatusTip( "Start playback" );
	connect( actionPlay, SIGNAL( triggered() ), this, SLOT( playbackStart() ) );

	actionStop = new QAction( QIcon( ":/images/stop" ), "Stop", this );
	actionStop->setToolTip( "Stop" );
	actionStop->setStatusTip( "Stop playback" );
	connect( actionStop, SIGNAL( triggered() ), this, SLOT( playbackStop() ) );

	actionLoop = new QAction( QIcon( ":/images/repeat" ), "Loop", this );
	actionLoop->setToolTip( "Loop" );
	actionLoop->setStatusTip( "Loop playback" );
	actionLoop->setCheckable( true );

	actionNew = new QAction( QIcon( ":/images/film" ), "New animation", this );
	actionNew->setToolTip( "New" );
	actionNew->setStatusTip( "Create new animation" );

	actionOpen = new QAction( QIcon( ":/images/open" ), "Open animation", this );
	actionOpen->setToolTip( "Open" );
	actionOpen->setStatusTip( "Open animation" );

	actionSave = new QAction( QIcon( ":/images/save" ), "Save animation", this );
	actionSave->setToolTip( "Save" );
	actionSave->setStatusTip( "Save animation" );

	actionExit = new QAction( QIcon( ":/images/exit" ), "Close", this );
	actionExit->setToolTip( "Exit" );
	actionExit->setStatusTip( "Close" );

	connect( actionNew, SIGNAL( triggered() ), this, SLOT( newAnimation() ) );
	connect( actionOpen, SIGNAL( triggered() ), this, SLOT( loadAnimation() ) );
	connect( actionSave, SIGNAL( triggered() ), this, SLOT( saveAnimation() ) );
	connect( actionExit, SIGNAL( triggered() ), this, SLOT( closeWindow() ) );

	actionShowSelection = new QAction( "Show selection", this );
	actionShowSelection->setToolTip( "Show selection" );
	actionShowSelection->setStatusTip( "Show selection" );
	actionShowSelection->setCheckable( true );
	actionShowSelection->setChecked( true );

	actionShowCrosses = new QAction( "Show crosses", this );
	actionShowCrosses->setToolTip( "Show crosses" );
	actionShowCrosses->setStatusTip( "Show crosses" );
	actionShowCrosses->setCheckable( true );
	actionShowCrosses->setChecked( true );

	connect( actionShowSelection, SIGNAL( triggered() ), this, SLOT( showSelection() ) );
	connect( actionShowCrosses, SIGNAL( triggered() ), this, SLOT( showCrosses() ) );
}

void AnimationEditorWindow::createToolBar() {
	ui.mainToolBar->addAction( actionNew );
	ui.mainToolBar->addAction( actionOpen );
	ui.mainToolBar->addAction( actionSave );

	QToolBar* playbackToolbar = addToolBar( "Playback" );
	playbackToolbar->addAction( actionPlay );
	playbackToolbar->addAction( actionStop );
	playbackToolbar->addAction( actionLoop );

	QToolBar* viewToolbar = addToolBar( "View" );
	viewToolbar->addAction( actionShowCrosses );
	viewToolbar->addAction( actionShowSelection );
}

void AnimationEditorWindow::createMenu() {
	QMenu* fileMenu = menuBar()->addMenu( tr( "&File" ) );
	fileMenu->addAction( actionNew );
	fileMenu->addAction( actionOpen );
	fileMenu->addAction( actionSave );
	fileMenu->addSeparator();
	fileMenu->addAction( actionExit );

	QMenu* viewMenu = menuBar()->addMenu( tr( "&View" ) );
	viewMenu->addAction( actionShowCrosses );
	viewMenu->addAction( actionShowSelection );
}

void AnimationEditorWindow::closeWindow() {
	close();
}

void AnimationEditorWindow::refreshSounds() {
	QDir dir( "sfx" );
	QStringList filter( "*.wav" );
	QStringList files = dir.entryList( filter );

	int index = 0;
	QString str = ui.soundCombo->currentText();
	ui.soundCombo->clear();
	ui.soundCombo->addItem( "<no sound>" );
	for( int i = 0; i < files.size(); ++i ) {
		ui.soundCombo->addItem( files.at( i ) );
		if( files.at( i ) == str ) {
			index = i;
		}
	}
	if( index ) {
		ui.soundCombo->setCurrentIndex( index + 1 );
	}
}

void AnimationEditorWindow::loadAnimation() {
	QString str = QFileDialog::getOpenFileName( this, "Open animation", "", "animation (*.xml)" );
	if( str != "" ) {
		loadAnimation( str );
	}
}

void AnimationEditorWindow::saveAnimation() {
	QString str = QFileDialog::getSaveFileName( this, "Save animation", "", "animation (*.xml)" );
	if( str != "" ) {
		animation.save( str.toStdString() );
		setWindowTitle( str );
	}
}

void AnimationEditorWindow::newAnimation() {
	modified = false;
	setWindowTitle( "untitled.xml" );
}

void AnimationEditorWindow::frameListBoxChanged( int row ) {
	if( row >= 0 && row < animation.getFrameCount() ) {
		ui.spriteListBox->clear();
		AnimationFrame& frame = animation.getFrame( row );
		QString label;
		for( int i = 0, n = frame.sprites.size(); i < n; ++i ) {
			ui.spriteListBox->addItem( frame.sprites[i].getImage()->getFileName().c_str() );
		}

		if( frame.sample ) {
			QString sample( frame.sample->getFileName().c_str() );
			int i = ui.soundCombo->findText( frame.sample->getFileName().c_str(), Qt::MatchExactly );
			if( i >= 0 ) {
				ui.soundCombo->setCurrentIndex( i );
			} else {
				// TODO: message box s chybou
			}
		} else {
			ui.soundCombo->setCurrentIndex( 0 );
		}

		ui.delaySpinBox->setValue( frame.delay );
		if( frame.sprites.size() > 0 ) {
			ui.spriteListBox->setCurrentRow( 0 );
		}
	} else {
		ui.spriteListBox->clear();
	}
	aeWidget->setCurrentFrame( row );
}

void AnimationEditorWindow::showPrevChanged( int state ) {
	aeWidget->showPrevFrame( state == Qt::Checked );
}

void AnimationEditorWindow::spriteListBoxChanged( int row ) {
	if( row >= 0 ) {
		AnimationFrame& frame = animation.getFrame( ui.frameListBox->currentRow() );
		Sprite& sprite = frame.sprites[row];
		ui.xPosBox->setValue( sprite.getX() );
		ui.yPosBox->setValue( sprite.getY() );
	}
	aeWidget->setCurrentSprite( row );
}

void AnimationEditorWindow::addFrame() {
	AnimationFrame frame;
	frame.delay = 0;
	animation.addFrame( frame );
	ui.frameListBox->addItem( "New frame" );
}

void AnimationEditorWindow::removeFrame() {
	int row = ui.frameListBox->currentRow();
	if( row >= 0 && row < animation.getFrameCount() ) {
		animation.removeFrame( row );
		ui.frameListBox->takeItem( row );
		ui.frameListBox->setCurrentRow( row - 1 );
	}
}

void AnimationEditorWindow::cloneFrame() {
	int row = ui.frameListBox->currentRow();
	if( row >= 0 ) {
		AnimationFrame& pframe = animation.getFrame( row );

		AnimationFrame frame;
		frame.delay = pframe.delay;
		frame.sample = pframe.sample;
		//frame.sprites.reserve(pframe.sprites.size());
		frame.sprites.resize( pframe.sprites.size() );
		std::copy( pframe.sprites.begin(), pframe.sprites.end(), frame.sprites.begin() );
		animation.addFrame( frame, row );
		QString label;
		label.sprintf( "Frame" );
		ui.frameListBox->addItem( label );
	}
}

void AnimationEditorWindow::moveFrameUp() {
	int row = ui.frameListBox->currentRow();
	if( row > 0 ) {
		AnimationFrame& frame = animation.getFrame( row );
		animation.addFrame( frame, row - 1 );
		animation.removeFrame( row + 1 );
		ui.frameListBox->setCurrentRow( row - 1 );
	}
}

void AnimationEditorWindow::moveFrameDown() {
	int row = ui.frameListBox->currentRow();
	if( row < animation.getFrameCount() - 1 ) {
		AnimationFrame& frame = animation.getFrame( row );
		animation.addFrame( frame, row + 2 );
		animation.removeFrame( row );
		ui.frameListBox->setCurrentRow( row + 1 );
	}
}

void AnimationEditorWindow::moveSpriteUp() {
	int frIndex = ui.frameListBox->currentRow();
	int sprIndex = ui.spriteListBox->currentRow();
	if( frIndex < 0 || frIndex >= animation.getFrameCount() ) {
		return;
	}

	AnimationFrame& frame = animation.getFrame( frIndex );
	if( sprIndex < 1 || sprIndex >= frame.sprites.size() ) {
		return ;
	}

	Sprite& sprite = frame.sprites[sprIndex];
	int index = 0;
	for( Sprites::iterator it = frame.sprites.begin(); it != frame.sprites.end(); ++it ) {
		if( ++index == sprIndex ) {
			it = frame.sprites.insert( it, frame.sprites[index] );
			++it;
			frame.sprites.erase( ++it );
			break;
		}
	}
	ui.spriteListBox->setCurrentRow( sprIndex - 1 );
	frameUpdate();
	aeWidget->updateGL();
}

void AnimationEditorWindow::moveSpriteDown() {
	int frIndex = ui.frameListBox->currentRow();
	int sprIndex = ui.spriteListBox->currentRow();
	if( frIndex < 0 || frIndex >= animation.getFrameCount() ) {
		return;
	}

	AnimationFrame& frame = animation.getFrame( frIndex );
	if( sprIndex < 0 || sprIndex >= frame.sprites.size() - 1 ) {
		return ;
	}

	Sprite& sprite = frame.sprites[sprIndex];
	int index = 0;
	for( Sprites::iterator it = frame.sprites.begin(); it != frame.sprites.end(); ++it ) {
		if( ++index == sprIndex + 1 ) {
			it = frame.sprites.insert( it, frame.sprites[index] );
			++it;
			frame.sprites.erase( ++it );
			break;
		}
	}
	frameUpdate();
	aeWidget->updateGL();
}

void AnimationEditorWindow::removeSprite() {
	int frame = ui.frameListBox->currentRow();
	int sprite = ui.spriteListBox->currentRow();
	if( frame < 0 || frame >= animation.getFrameCount() ) {
		return ;
	}
	AnimationFrame& frm = animation.getFrame( frame );
	if( sprite < 0 || sprite >= frm.sprites.size() ) {
		return ;
	}

	int index = 0;
	for( Sprites::iterator it = frm.sprites.begin(); it != frm.sprites.end(); ++it ) {
		if( index++ == sprite ) {
			frm.sprites.erase( it );
			frameUpdate();
			break;
		}
	}
	aeWidget->updateGL();
}

void AnimationEditorWindow::soundComboChanged( int index ) {
	int frame = ui.frameListBox->currentRow();
	if( frame < 0 || frame >= animation.getFrameCount() ) {
		return ;
	}

	AnimationFrame& fr = animation.getFrame( frame );
	if( index == 0 ) {
		fr.sample.reset();
	} else {
		fr.sample = SoundManager::i()->createObject( ui.soundCombo->currentText().toStdString() );
	}
}

void AnimationEditorWindow::xPosChanged( int x ) {
	int frame = ui.frameListBox->currentRow();
	int sprite = ui.spriteListBox->currentRow();
	if( frame < 0 || frame >= animation.getFrameCount() ) {
		return ;
	}
	AnimationFrame& frm = animation.getFrame( frame );
	if( sprite < 0 || sprite >= frm.sprites.size() ) {
		return ;
	}

	Sprite& spr = frm.sprites[sprite];
	spr.setX( x );
	aeWidget->updateGL();
}

void AnimationEditorWindow::yPosChanged( int y ) {
	int frame = ui.frameListBox->currentRow();
	int sprite = ui.spriteListBox->currentRow();
	if( frame < 0 || frame >= animation.getFrameCount() ) {
		return ;
	}
	AnimationFrame& frm = animation.getFrame( frame );
	if( sprite < 0 || sprite >= frm.sprites.size() ) {
		return ;
	}

	Sprite& spr = frm.sprites[sprite];
	spr.setY( y );
	aeWidget->updateGL();
}

void AnimationEditorWindow::xPosChangedMouse( int x ) {
	ui.xPosBox->setValue( x );
}

void AnimationEditorWindow::yPosChangedMouse( int y ) {
	ui.yPosBox->setValue( y );
}

void AnimationEditorWindow::playbackTimerTick() {
	ui.frameListBox->setCurrentRow( playbackFrame );
	if( ++playbackFrame < animation.getFrameCount() ) {
		playbackTimer.start( animation.getFrame( playbackFrame ).delay );
	} else if( actionLoop->isChecked() ) {
		playbackFrame = 0;
		playbackTimer.start( animation.getFrame( playbackFrame ).delay );
	}
}

void AnimationEditorWindow::playbackStart() {
	if( animation.getFrameCount() < 2 ) {
		return ;
	}
	playbackFrame = 1;
	ui.frameListBox->setCurrentRow( 0 );
	playbackTimer.start( animation.getFrame( playbackFrame ).delay );
}

void AnimationEditorWindow::playbackStop() {
	playbackTimer.stop();
}

void AnimationEditorWindow::showSelection() {
	aeWidget->setShowSelection( actionShowSelection->isChecked() );
}

void AnimationEditorWindow::showCrosses() {
	aeWidget->setShowCrosses( actionShowCrosses->isChecked() );
}

void AnimationEditorWindow::frameUpdate() {
	int index = ui.frameListBox->currentRow();
	if( index < 0 || index >= animation.getFrameCount() ) {
		return ;
	}

	ui.spriteListBox->clear();
	AnimationFrame& frame = animation.getFrame( index );
	QString label;
	for( int i = 0, n = frame.sprites.size(); i < n; ++i ) {
		ui.spriteListBox->addItem( QString::fromStdString( frame.sprites[i].getImage()->getFileName() ) );
	}
	ui.delaySpinBox->setValue( frame.delay );
	if( frame.sprites.size() > 0 ) {
		ui.spriteListBox->setCurrentRow( frame.sprites.size() - 1 );
	}
}

void AnimationEditorWindow::delayChanged( int delay ) {
	int index = ui.frameListBox->currentRow();
	if( index < 0 || index >= animation.getFrameCount() ) {
		return ;
	}

	animation.getFrame( index ).delay = delay;
}

void AnimationEditorWindow::loadAnimation( const QString& str ) {
	try {
		animation.load( str.toStdString() );
		ui.frameListBox->clear();
		QString label;
		sprites.clear();
		for( int i = 0, n = animation.getFrameCount(); i < n; ++i ) {
			label.sprintf( "Frame %d", i );
			ui.frameListBox->addItem( label );
			AnimationFrame& frame = animation.getFrame( i );
			for( auto it = frame.sprites.begin(); it != frame.sprites.end(); ++it ) {
				sprites.insert( it->getImage()->getFileName() );
			}
		}
		if( animation.getFrameCount() > 0 ) {
			ui.frameListBox->setCurrentRow( 0 );
		}

		aeWidget->updateGL();
	} catch( std::exception& e ) {
		QMessageBox::critical( this, "Error", QString( "Error loading animation: %1" ).arg( e.what() ) );
	}
}
