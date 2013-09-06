#ifndef ANIMATIONEDITORWINDOW_H
#define ANIMATIONEDITORWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QTimer>

#include <set>
#include <string>

#include "ui_AnimationEditorWindow.h"
#include "AnimationEditorWidget.h"

// LAE_Core
#include <LAE_Core/graphics/Animation.h>

typedef std::set<std::string>	TStringSet;

class AnimationEditorWindow : public QMainWindow {
	Q_OBJECT

public:
	AnimationEditorWindow( QGLContext* context, QWidget* parent = 0 );
	~AnimationEditorWindow();

protected:
	void closeEvent( QCloseEvent* event );

private:
	void connectActions();
	void createMenu();
	void createToolBar();

	Ui::AnimationEditorWindowClass ui;
	AnimationEditorWidget*	aeWidget;

	LAE::Animation	animation;
	bool		modified;

	TStringSet		sprites;

	int		playbackFrame;
	QTimer	playbackTimer;

	QGLContext* context;

	QAction*		actionPlay;
	QAction*		actionStop;
	QAction*		actionLoop;

	QAction*		actionNew;
	QAction*		actionOpen;
	QAction*		actionSave;
	QAction*		actionExit;

	QAction*		actionShowSelection;
	QAction*		actionShowCrosses;

private slots:
	void closeWindow();
	void loadAnimation();
	void saveAnimation();
	void newAnimation();

	void frameListBoxChanged( int row );
	void spriteListBoxChanged( int row );
	void showPrevChanged( int state );

	void addFrame();
	void removeFrame();
	void cloneFrame();
	void moveFrameUp();
	void moveFrameDown();

	void moveSpriteUp();
	void moveSpriteDown();
	void removeSprite();
	void soundComboChanged( int index );

	void delayChanged( int delay );
	void xPosChanged( int x );
	void yPosChanged( int y );
	void xPosChangedMouse( int x );
	void yPosChangedMouse( int y );

	void playbackTimerTick();
	void playbackStart();
	void playbackStop();

	void frameUpdate();

	void showSelection();
	void showCrosses();

	void refreshSounds();

	void loadAnimation( const QString& str );
};

#endif // ANIMATIONEDITORWINDOW_H
