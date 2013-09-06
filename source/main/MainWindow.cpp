#include "MainWindow.h"
#include "../animation/AnimationEditorWindow.h"
#include "../room/RoomEditorWindow.h"

MainWindow::MainWindow( QWidget* parent, Qt::WFlags flags )
	: QMainWindow( parent, flags ), glContext( 0 ), spriteDlg( this ), animationDlg( this ) {
	ui.setupUi( this );

	glContext = new QGLContext( QGLFormat( QGL::DoubleBuffer | QGL::Rgba ) );

	connectActions();
	createMenu();
}

MainWindow::~MainWindow() {

}

void MainWindow::newAnimationWindow() {
	AnimationEditorWindow* window = new AnimationEditorWindow( glContext, this );
	ui.mdiArea->addSubWindow( window );
	window->show();
}

void MainWindow::newRoomWindow() {
	RoomEditorWindow* window = new RoomEditorWindow( glContext, this );
	ui.mdiArea->addSubWindow( window );
	window->show();
}

void MainWindow::showSpriteDialog() {
	spriteDlg.show();
}

void MainWindow::showAnimationDialog() {
	animationDlg.show();
}

void MainWindow::connectActions() {
	action_animEditor = new QAction( QIcon( ":/images/film" ), "&Animation editor", this );
	action_animEditor->setToolTip( "Animation" );
	action_animEditor->setStatusTip( "Open new animation editor window" );

	connect( action_animEditor, SIGNAL( triggered() ), this, SLOT( newAnimationWindow() ) );

	action_roomEditor = new QAction( QIcon( ":/images/room" ), "&Room editor", this );
	action_roomEditor->setToolTip( "Room" );
	action_roomEditor->setStatusTip( "Open new room editor window" );

	connect( action_roomEditor, SIGNAL( triggered() ), this, SLOT( newRoomWindow() ) );

	action_exit = new QAction( QIcon( ":/images/exit" ), "E&xit", this );
	action_exit->setToolTip( "Exit" );
	action_exit->setStatusTip( "Close editor" );

	action_showSprites = new QAction( "Show sprites", this );
	action_showSprites->setToolTip( "Sprites" );
	action_showSprites->setStatusTip( "Show sprites" );

	connect( action_showSprites, SIGNAL( triggered() ), this, SLOT( showSpriteDialog() ) );

	action_showAnimations = new QAction( "Show animations", this );
	action_showAnimations->setToolTip( "Animations" );
	action_showAnimations->setStatusTip( "Show animations" );

	connect( action_showAnimations, SIGNAL( triggered() ), this, SLOT( showAnimationDialog() ) );
}

void MainWindow::createMenu() {
	QMenu* fileMenu = menuBar()->addMenu( tr( "&Tools" ) );
	fileMenu->addAction( action_animEditor );
	fileMenu->addAction( action_roomEditor );
	fileMenu->addSeparator();
	fileMenu->addAction( action_exit );

	QMenu* view = menuBar()->addMenu( tr( "&View" ) );
	view->addAction( action_showSprites );
	view->addAction( action_showAnimations );
}
