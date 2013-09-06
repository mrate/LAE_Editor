#include "AnimationEditorWidget.h"
#include <QMouseEvent>
#include <QFile>

#include <LAE_Core/cache/ImageCache.h>

using namespace LAE;

AnimationEditorWidget::AnimationEditorWidget( QWidget* parent, Animation& animation, QGLContext* context )
	: QGLWidget( /*context,*/ parent )
	, screen( nullptr )
	, anim( &animation )
	, currentFrame( -1 )
	, currentSprite( -1 )
	, showPrev( false )
	, lastX( 0 )
	, lastY( 0 )
	, showCrosses( true )
	, showSelection( true )
	, crossColor( 50, 50, 50, 255 )
	, crossColorInc( true )
	, crossColorTimer( this ) {

	setFocusPolicy( Qt::ClickFocus );
	setAcceptDrops( true );
	connect( &crossColorTimer, SIGNAL( timeout() ), this, SLOT( crossColorChange() ) );
	crossColorTimer.start( 100 );
}

AnimationEditorWidget::~AnimationEditorWidget() {
	if( screen ) {
		delete screen;
	}
}

void AnimationEditorWidget::initializeGL() {
	screen = new Screen( 1024, 1024, 32, false );
}

void AnimationEditorWidget::paintGL() {
	screen->clear();

	if( showCrosses ) {
		screen->drawLine( 512, 0, 512, 1024, crossColor );
		screen->drawLine( 0, 512, 1024, 512, crossColor );
	}

	if( showPrev ) {
		int prevFrame;
		if( currentFrame == 0 ) {
			prevFrame = anim->getFrameCount() - 1;
		} else {
			prevFrame = currentFrame - 1;
		}
		anim->renderFrame( *screen, 512, 512, prevFrame, ( unsigned char )75 );
	}
	anim->renderFrame( *screen, 512, 512, currentFrame );

	if( currentSprite >= 0 && showSelection ) {
		Sprite& sprite = anim->getFrame( currentFrame ).sprites[currentSprite];
		const Image* img = sprite.getImage().get();
		LAE::Color selectionColor = crossColor;
		selectionColor.g = selectionColor.b = 0;
		screen->drawRectangle( 512 + sprite.getX(), 512 + sprite.getY(), 512 + sprite.getX() + img->width(), 512 + sprite.getY() + img->height(), selectionColor );
	}
}

void AnimationEditorWidget::resizeGL( int width, int height ) {
}

void AnimationEditorWidget::mousePressEvent( QMouseEvent* event ) {
	lastX = event->x();
	lastY = event->y();
}

void AnimationEditorWidget::mouseMoveEvent( QMouseEvent* event ) {
	if( event->buttons() & Qt::LeftButton && currentSprite >= 0 ) {
		Sprite& sprite = anim->getFrame( currentFrame ).sprites[currentSprite];
		sprite.setX( sprite.getX() - ( lastX - event->x() ) );
		sprite.setY( sprite.getY() - ( lastY - event->y() ) );
		lastX = event->x();
		lastY = event->y();

		emit xPosChanged( sprite.getX() );
		emit yPosChanged( sprite.getY() );

		updateGL();
	}
}

void AnimationEditorWidget::keyPressEvent( QKeyEvent* event ) {
	if( currentSprite >= 0 ) {
		Sprite& sprite = anim->getFrame( currentFrame ).sprites[currentSprite];

		if( event->key() == Qt::Key_Left ) {
			sprite.setX( sprite.getX() - 1 );
		}
		if( event->key() == Qt::Key_Right ) {
			sprite.setX( sprite.getX() + 1 );
		}
		if( event->key() == Qt::Key_Up ) {
			sprite.setY( sprite.getY() - 1 );
		}
		if( event->key() == Qt::Key_Down ) {
			sprite.setY( sprite.getY() + 1 );
		}

		emit xPosChanged( sprite.getX() );
		emit yPosChanged( sprite.getY() );

		updateGL();
	}
}

void AnimationEditorWidget::setCurrentFrame( int index ) {
	currentFrame = index;
	updateGL();
}

void AnimationEditorWidget::setCurrentSprite( int sprite ) {
	currentSprite = sprite;
}

void AnimationEditorWidget::showPrevFrame( bool show ) {
	showPrev = show;
	updateGL();
}

void AnimationEditorWidget::crossColorChange() {
	if( crossColorInc ) {
		crossColor.r += 25;
		if( crossColor.r == 250 ) {
			crossColorInc = false;
		}
	} else {
		crossColor.r -= 25;
		if( crossColor.r == 50 ) {
			crossColorInc = true;
		}
	}
	crossColor.g = crossColor.b = crossColor.r;		
	updateGL();
}

void AnimationEditorWidget::setShowSelection( bool show ) {
	showSelection = show;
}

void AnimationEditorWidget::setShowCrosses( bool show ) {
	showCrosses = show;
}

void AnimationEditorWidget::dragEnterEvent( QDragEnterEvent* event ) {
	const QMimeData* data = event->mimeData();

	if( currentFrame >= 0 && currentFrame < anim->getFrameCount() && data->formats().at( 0 ) == "x-application/lae_sprite" ) {
		QString text( data->data( "x-application/lae_sprite" ) );
		QFile file( "gfx/" + text );
		if( file.exists() ) {
			event->acceptProposedAction();
			return ;
		}
	} else if( data->formats().at( 0 ) == "x-application/lae_animation" ) {
		QString text( data->data( "x-application/lae_animation" ) );
		QFile file( "anims/" + text );
		if( file.exists() ) {
			event->acceptProposedAction();
			return ;
		}
	}
	event->ignore();
}

void AnimationEditorWidget::dropEvent( QDropEvent* event ) {
	const QPoint& pos = event->pos();
	const QMimeData* data = event->mimeData();
	QString fmt = data->formats().at( 0 );
	if( fmt == "x-application/lae_sprite" ) {
		QString str( data->data( fmt ) );
		Sprite sprite;
		sprite.setX( pos.x() - 512 );
		sprite.setY( pos.y() - 512 );
		sprite.setImage( ImageManager::i()->createObject( str.toStdString() ) );
		anim->getFrame( currentFrame ).sprites.push_back( sprite );

		emit frameUpdate();
	} else if( fmt == "x-application/lae_animation" ) {
		emit loadAnimation( QString( "anims/%1" ).arg( ( const char* )data->data( fmt ) ) );
	}
}

QSize AnimationEditorWidget::sizeHint() const {
	return QSize( 1024, 1024 );
}

QSize AnimationEditorWidget::minimumSizeHint() const {
	return QSize( 1024, 1024 );
}
