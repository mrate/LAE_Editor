#ifndef ANIMATIONEDITORWIDGET_H
#define ANIMATIONEDITORWIDGET_H

#include <QTimer>
#include <QGLWidget>
#include <LAE_Core/graphics/Screen.h>
#include <LAE_Core/graphics/Animation.h>

class AnimationEditorWidget : public QGLWidget {
	Q_OBJECT

public:
	AnimationEditorWidget( QWidget* parent, LAE::Animation& animation, QGLContext* context );
	~AnimationEditorWidget();

	void setCurrentFrame( int frame );
	void setCurrentSprite( int sprite );
	void showPrevFrame( bool show );

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

private:
	LAE::Screen*		screen;
	LAE::Animation*		anim;
	int			currentFrame;
	int			currentSprite;
	bool		showPrev;

	// edit
	int		lastX;
	int		lastY;

	bool	showCrosses;
	bool	showSelection;

	LAE::Color	crossColor;
	bool		crossColorInc;
	QTimer		crossColorTimer;

private slots:
	void crossColorChange();

protected:
	void initializeGL();
	void paintGL();
	void resizeGL( int width, int height );
	void mousePressEvent( QMouseEvent* event );
	void mouseMoveEvent( QMouseEvent* event );
	void keyPressEvent( QKeyEvent* event );

	void dragEnterEvent( QDragEnterEvent* event );
	void dropEvent( QDropEvent* event );

public slots:
	void setShowSelection( bool show );
	void setShowCrosses( bool show );

signals:
	void xPosChanged( int x );
	void yPosChanged( int y );
	void frameUpdate();

	void loadAnimation( const QString& );

};

#endif // ANIMATIONEDITORWIDGET_H
