#ifndef SPRITESELECTORWINDOW_H
#define SPRITESELECTORWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "ui_SpriteSelectorWindow.h"

class SpriteSelectorWindow : public QMainWindow {
	Q_OBJECT

public:
	SpriteSelectorWindow( QWidget* parent = 0 );
	~SpriteSelectorWindow();

private:
	Ui::SpriteSelectorWindowClass ui;
	QLabel* previewLabel;

private slots:
	void refreshSprites();
	void spriteChanged( int row );
};

#endif // SPRITESELECTORWINDOW_H
