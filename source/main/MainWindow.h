#ifndef LAE_TOOLS_H
#define LAE_TOOLS_H

// LAE
#include "ui_MainWindow.h"
#include "../common/SpriteSelectorWindow.h"
#include "../common/AnimationSelectorWindow.h"
// Qt
#include <QGLContext>
#include <QtGui/QMainWindow>

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	MainWindow( QWidget* parent = 0, Qt::WFlags flags = 0 );
	~MainWindow();

private:
	Ui::MainWindow ui;

	QGLContext*	glContext;

	QAction*		action_animEditor;
	QAction*		action_roomEditor;
	QAction*		action_exit;

	QAction*		action_showSprites;
	QAction*		action_showAnimations;

	void connectActions();
	void createMenu();

	SpriteSelectorWindow	spriteDlg;
	AnimationSelectorWindow	animationDlg;

private slots:
	void newAnimationWindow();
	void newRoomWindow();
	void showSpriteDialog();
	void showAnimationDialog();
};

#endif // LAE_TOOLS_H
