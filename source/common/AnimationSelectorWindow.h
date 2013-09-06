#ifndef ANIMATIONSELECTORWINDOW_H
#define ANIMATIONSELECTORWINDOW_H

#include <QMainWindow>
#include "ui_AnimationSelectorWindow.h"

class AnimationSelectorWindow : public QMainWindow {
	Q_OBJECT

public:
	AnimationSelectorWindow( QWidget* parent = 0 );
	~AnimationSelectorWindow();

private:
	Ui::AnimationSelectorWindowClass ui;

private slots:
	void refresh();
};

#endif // ANIMATIONSELECTORWINDOW_H
