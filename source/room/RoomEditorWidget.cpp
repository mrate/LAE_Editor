#include "RoomEditorWidget.h"
// qt
#include <QMouseEvent>
#include <QDir>
#include <QFile>
#include <QMenu>
// stl
#include <cmath>
// LAE
#include <LAE_Core/cache/ImageCache.h>

using namespace LAE;

RoomEditorWidget::RoomEditorWidget( QWidget* parent, GameRoom& room, QGLContext* context )
	: QGLWidget( /*context,*/ parent )
	, screen( 0 )
	, room( &room )
	, show_zplanes( true )
	, show_items( true )
	, show_boxes( true )
	, show_scales( true )
	, timer( this )
	, scaleDebug( 0 ) {
	setFocusPolicy( Qt::ClickFocus );
	setAcceptDrops( true );
	setMouseTracking( true );

	connect( &timer, SIGNAL( timeout() ), this, SLOT( timerTick() ) );
	timer.start( 100 );
	selection.colorInc = true;
	selection.color = LAE::Color( 50, 0, 0, 255);
	selection.vertex = 0;
}

RoomEditorWidget::~RoomEditorWidget() {
	if( screen ) {
		delete screen;
	}
}

QSize RoomEditorWidget::sizeHint() const {
	return QSize( 4096, 4096 );
}

QSize RoomEditorWidget::minimumSizeHint() const {
	return QSize( 4096, 4096 );
}

void RoomEditorWidget::initializeGL() {
	screen = new Screen( 4096, 4096, 32, false );
}

void RoomEditorWidget::itemsChanged() {
	initZOrder();
	updateGL();
}

void RoomEditorWidget::paintGL() {
	screen->clear();
	screen->setEffect( Screen::EFFECT_NONE );

	// bg
	if( room->getBgImage() ) {
		screen->drawImage( room->getBgImage().get(), 0, 0 );
	}

	// vykresleni objektu podle poradi Z
	for( TZOrderMap::const_reverse_iterator i = zorder.rbegin(); i != zorder.rend(); ++i ) {
		const ObjectIdentifier& ident = i->second;

		switch( ident.type ) {
		case ObjectIdentifier::ZPLANE: {
			const AnimatedSprite* sprite = room->getZPlanes()[ident.index];
			screen->setEffect( sprite->getEffect() );
			if( sprite->isLightable() ) {
				enableLights();
			}
			sprite->render( *screen, 0, 0 );
			if( selection.active && selection.type == ObjectIdentifier::ZPLANE && selection.index == ident.index && sprite->getEffect() == Screen::EFFECT_NONE ) {
				glBlendFunc( GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA );
				sprite->render( *screen, 0, 0, selection.color.a );
				screen->setEffect( Screen::EFFECT_NONE );
			}
			if( sprite->isLightable() ) {
				disableLights();
			}
		}
		break;
		case ObjectIdentifier::ITEM: {
			const Item& i = room->getItems()[ident.strIndex];
			screen->setEffect( i.getEffect() );
			if( i.isLightable() ) {
				enableLights();
			}
			i.render( *screen, 0, 0 );
			if( selection.active && selection.type == ObjectIdentifier::ITEM && selection.strIndex == ident.strIndex ) {
				if( i.getEffect() == Screen::EFFECT_NONE ) {
					glBlendFunc( GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA );
					i.render( *screen, 0, 0, selection.color.a );
					screen->setEffect( Screen::EFFECT_NONE );
				}

				screen->drawRectangle( 
					i.getX() + i.getActiveRect().left(), 
					i.getY() + i.getActiveRect().top(), 
					i.getX() + i.getActiveRect().right(), 
					i.getY() + i.getActiveRect().bottom(), 
					selection.color );

				if( selection.vertex >= 0 ) {
					screen->drawRectangle( 
						i.getX() + i.getActiveRect().right() - 5, 
						i.getY() + i.getActiveRect().bottom() - 5, 
						i.getX() + i.getActiveRect().right() + 5, 
						i.getY() + i.getActiveRect().bottom() + 5, 
						selection.color );
				}
			}
			if( i.isLightable() ) {
				disableLights();
			}
		}
		break;
		case ObjectIdentifier::BOX:
			screen->setEffect( Screen::EFFECT_NONE );

			if( selection.active && selection.type == ObjectIdentifier::BOX && selection.index == ident.index ) {
				// aktivni box
				const WalkingBox& box = room->getBoxes()[ident.index];

				drawNicePoly( box.p, selection.color );
				if( selection.vertex >= 0 ) {
					const Point& p = box.p[selection.vertex];
					screen->drawRectangle( p.x - 5, p.y - 5, p.x + 5, p.y + 5, selection.color );
				}

				// vykresleni sousedu
				//if(room->neighbour)
				{
					for( size_t j = 0, size = room->getBoxes().size(); j < size; ++j ) {
						if( box.hasNeighbour(j) ) {
							Point p1 = box.center;
							Point p2 = room->getBoxes()[j].center;
							screen->drawLine( p1.x, p1.y, p2.x, p2.y, LAE::Color( 0, 255, 255 ) );

							const Segment& s = box.inter.find(j)->second;
							screen->drawLine( s, LAE::Color( 0, 255, 0 ) );
						}
					}
				}

			} else { // ostatni boxy
				const WalkingBox& box = room->getBoxes()[ident.index];
				drawNicePoly( box.p, Color( 0, 0, 200 ) );
			}

			if( scaleDebug == 1 || ( scaleDebug == 2 && selection.type == ObjectIdentifier::BOX && selection.index == ident.index ) ) {
				// testovani zvetseni
				int scaleIndex = room->getBoxes()[ident.index].scale;
				if( scaleIndex < 0 || scaleIndex >= room->getScales().size() ) {
					continue;
				}

				const Scale& scale = room->getScales()[scaleIndex];

				float scale_v;
				float w, h;
				PImage img = ImageManager::i()->createObject("gfx//walk_left01.png" );
				for( int i = 0; i < 4; ++i ) {
					const Point& p = room->getBoxes()[ident.index].p[i];

					/*
					float dist = std::sqrt(std::pow((float)(p.x - scale.p1.x), 2) + std::pow((float)(p.y - scale.p1.y), 2));
					scale_v = ((float)scale.scale1 / 255.0f) * (dist / 255.0f);
					if(scale_v > 1.0f) scale_v = 1.0f;
					*/

					if( p.y <= scale.p0.y ) {
						scale_v = scale.scale1;
					}
					if( p.y >= scale.p1.y ) {
						scale_v = scale.scale2;
					}
					if( p.y > scale.p0.y && p.y < scale.p1.y ) {
						float scl_size = scale.p1.y - scale.p0.y;
						float scl_y = p.y - scale.p0.y;
						float scl_div = scl_y / scl_size;
						float scl_int = scl_div * ( scale.scale2 - scale.scale1 );
						scale_v = scale.scale1 + scl_int;
					}
					scale_v = scale_v / 255.0f;

					w = img->width() * scale_v;
					h = img->height() * scale_v;

					enableLights();
					screen->drawScaleImage( img.get(), p.x - w / 2, p.y - h, w, h, 180 );
					disableLights();
				}
			}
			break;
		}
	}

	screen->setEffect( Screen::EFFECT_NONE );
	unsigned char c;
	Point p[4];
	int index = 0;

	// vykresleni meritek
	for( int i = 0; i < room->getScales().size(); ++i ) {
		const Scale& scale = room->getScales()[i];
		c = ( selection.index == i && selection.type == ObjectIdentifier::SCALE ? selection.color.r : 255 );
		screen->drawLine( scale.p1.x - 15, scale.p1.y - 15, scale.p1.x + 15, scale.p1.y + 15, Color( c, 0, 0 ) );
		screen->drawLine( scale.p1.x + 15, scale.p1.y - 15, scale.p1.x - 15, scale.p1.y + 15, Color( c, 0, 0 ) );
	}

	// vykresleni zdroju svetla
	for( Lights::const_iterator i = room->getLights().begin(); i != room->getLights().end(); ++i ) {
		const LightSource& light = *i;
		c = ( selection.type == ObjectIdentifier::LIGHT && selection.index == index++ ? selection.color.r : 255 );
		screen->drawLine( light.x - 15, light.y, light.x + 15, light.y, Color( c, 0, 0 ) );
		screen->drawLine( light.x, light.y - 15, light.x, light.y + 15, Color( c, 0, 0 ) );
		p[0].x = light.x - 8;
		p[0].y = light.y;
		p[1].x = light.x;
		p[1].y = light.y - 8;
		p[2].x = light.x + 8;
		p[2].y = light.y;
		p[3].x = light.x;
		p[3].y = light.y + 8;
		screen->drawPoly( p, 4, Color( c, 0, 0 ) );
	}
}

void RoomEditorWidget::enableLights() {
	for( int i = 0; i < room->getLights().size(); ++i ) {
		screen->enableLight( i, &room->getLights()[i] );
	}
}

void RoomEditorWidget::disableLights() {
	for( int i = 0; i < room->getLights().size(); ++i ) {
		screen->disableLight( i );
	}
}

void RoomEditorWidget::initZOrder() {
	zorder.clear();

	for( int i = 0; i < room->getBoxes().size(); ++i ) {
		zorder.insert( std::make_pair( room->getBoxes()[i].z, ObjectIdentifier( ObjectIdentifier::BOX, i ) ) );
	}

	for( int i = 0; i < room->getZPlanes().size(); ++i ) {
		zorder.insert( std::make_pair( room->getZPlanes()[i]->getZ(), ObjectIdentifier( ObjectIdentifier::ZPLANE, i ) ) );
	}

	for( ItemHashMap::const_iterator i = room->getItems().begin(); i != room->getItems().end(); ++i ) {
		zorder.insert( std::make_pair( i->second.getZ(), ObjectIdentifier( ObjectIdentifier::ITEM, i->first ) ) );
	}

}

void RoomEditorWidget::drawNicePoly( const Point* p, const LAE::Color& color ) const {
	LAE::Color color2;
	color2.r = std::max( color.r - 100, 0 );
	color2.g = std::max( color.g - 100, 0 );
	color2.b = std::max( color.b - 100, 0 );
	color2.a = std::max( color.a - 100, 0 );

	screen->drawFillPoly( p, 4, color2 );
	screen->drawPoly( p, 4, color );
}

void RoomEditorWidget::showZPlanes( bool show ) {
	show_zplanes = show;
	updateGL();
}

void RoomEditorWidget::showBoxes( bool show ) {
	show_boxes = show;
	updateGL();
}

void RoomEditorWidget::showItems( bool show ) {
	show_items = show;
	updateGL();
}

void RoomEditorWidget::showScales( bool show ) {
	show_scales = show;
	updateGL();
}

void RoomEditorWidget::selectZPlane( int index, AnimatedSprite* sprite ) {
	selection.active = true;
	selection.type = ObjectIdentifier::ZPLANE;
	selection.index = index;
	selection.sprite = sprite;
	updateGL();
}

void RoomEditorWidget::selectItem( const std::string& name, AnimatedSprite* sprite ) {
	selection.active = true;
	selection.type = ObjectIdentifier::ITEM;
	selection.strIndex = name;
	selection.sprite = sprite;
	updateGL();
}

void RoomEditorWidget::selectLight( int index ) {
	selection.active = true;
	selection.type = ObjectIdentifier::LIGHT;
	selection.index = index;
	updateGL();
}

void RoomEditorWidget::selectBox( int index ) {
	selection.active = true;
	selection.type = ObjectIdentifier::BOX;
	selection.index = index;
	updateGL();
}

void RoomEditorWidget::selectScale( int index ) {
	selection.active = true;
	selection.type = ObjectIdentifier::SCALE;
	selection.index = index;
	updateGL();
}

void RoomEditorWidget::selectNone() {
	selection.active = false;
	updateGL();
}

void RoomEditorWidget::resizeGL( int width, int height ) {
}

void RoomEditorWidget::mousePressEvent( QMouseEvent* event ) {
	Point mousePoint( event->x(), event->y() );

	switch( event->button() ) {
	case Qt::LeftButton:
		// levy mysitko -> pretahovani objektu
		lastX = mousePoint.x;
		lastY = mousePoint.y;
		break;

	case Qt::RightButton:
		// pravy mysitko
		switch( selection.type ) {
		case ObjectIdentifier::BOX:
			// walking box -> nastaveni sousedu
			for( size_t i = 0, count = room->getBoxes().size(); i < count; ++i ) {
				if( i == selection.index ) {
					continue;
				}
				const WalkingBox& box = room->getBoxes()[i];
				if( pointInBox( box, mousePoint ) ) {
					//box.
				}
			}
			break;
		}
		break;
	}
}

void RoomEditorWidget::mouseMoveEvent( QMouseEvent* event ) {
	Point m;
	m.x = event->x();
	m.y = event->y();

	if( !selection.active ) {
		return ;
	}

	if( selection.type == ObjectIdentifier::ITEM && !( event->buttons() & Qt::LeftButton ) ) {
		// vybrany predmet
		Item& item = room->getItems()[selection.strIndex];
		// vybrani pravyho dolniho rohu
		int lv = selection.vertex;
		if( m.x >= item.getX() + item.getActiveRect().right() - 5 
			&& m.x <= item.getX() + item.getActiveRect().right() + 5 
			&& m.y >= item.getY() + item.getActiveRect().bottom() - 5 
			&& m.y <= item.getY() + item.getActiveRect().bottom() + 5 ) {
			selection.vertex = 1;
		} else {
			selection.vertex = -1;
		}

		if( lv != selection.vertex ) {
			updateGL();
		}
	} else if( selection.type == ObjectIdentifier::BOX ) {
		// vybrany box
		WalkingBox& box = room->getBoxes()[selection.index];
		if( event->buttons() & Qt::LeftButton ) {
			// tahnutim menim vybranou hranu
			if( selection.vertex >= 0 ) {
				box.p[selection.vertex].x -= lastX - m.x;
				box.p[selection.vertex].y -= lastY - m.y;
			} else {
				for( int i = 0; i < 4; ++i ) {
					box.p[i].x -= lastX - m.x;
					box.p[i].y -= lastY - m.y;
				}
			}
			emit boxChanged( selection.index );
			updateGL();
		} else {
			// bez tlacitka vybiram hranu
			int lv = selection.vertex;
			selection.vertex = -1;
			for( int i = 0; i < 4; ++i ) {
				const Point p = box.p[i];
				if( m.x >= p.x - 5 && m.x <= p.x + 5 && m.y >= p.y - 5 && m.y <= p.y + 5 ) {
					selection.vertex = i;
					break ;
				}
			}
			if( selection.vertex != lv ) {
				updateGL();
			}
		}
	} else if( event->buttons() & Qt::LeftButton ) {
		// presouvam predmety
		switch( selection.type ) {
		case ObjectIdentifier::ZPLANE:
		case ObjectIdentifier::ITEM:
			if( selection.vertex >= 0 ) {
				// tahnutim meni sirku, vysku
				Item& item = room->getItems()[selection.strIndex];
				item.getActiveRect().setWidth( item.getActiveRect().width() - ( lastX - m.x ) );
				item.getActiveRect().setHeight( item.getActiveRect().height() - ( lastY - m.y ) );
				emit itemChanged( selection.strIndex );
				break;
			}
			selection.sprite->setX( selection.sprite->getX() - ( lastX - m.x ) );
			selection.sprite->setY( selection.sprite->getY() - ( lastY - m.y ) );
			if( selection.type == ObjectIdentifier::ITEM ) {
				emit itemChanged( selection.strIndex );
			} else {
				emit zplaneChanged( selection.index );
			}
			break;
		case ObjectIdentifier::LIGHT:
			room->getLights()[selection.index].x -= lastX - m.x;
			room->getLights()[selection.index].y -= lastY - m.y;
			emit lightChanged( selection.index );
			break;
		case ObjectIdentifier::SCALE:
			room->getScales()[selection.index].p1.x -= lastX - m.x;
			room->getScales()[selection.index].p1.y -= lastY - m.y;
			emit scaleChanged( selection.index );
			break;
		}
		updateGL();
	}
	lastX = m.x;
	lastY = m.y;
}

void RoomEditorWidget::keyPressEvent( QKeyEvent* event ) {
	if( event->key() == Qt::Key_D ) {
		if( scaleDebug == 2 ) {
			scaleDebug = 0;
		} else {
			scaleDebug = 2;
		}
		updateGL();
		return ;
	}

	if( event->key() == Qt::Key_A ) {
		if( scaleDebug == 1 ) {
			scaleDebug = 0;
		} else {
			scaleDebug = 1;
		}
		updateGL();
		return ;
	}

	if( !selection.active ) {
		return ;
	}

	int x = 0;
	int y = 0;
	switch( event->key() ) {
	case Qt::Key_Left:
		x = -1;
		break;
	case Qt::Key_Right:
		x = 1;
		break;
	case Qt::Key_Up:
		y = -1;
		break;
	case Qt::Key_Down:
		y = 1;
		break;
	}

	switch( selection.type ) {
	case ObjectIdentifier::ZPLANE:
	case ObjectIdentifier::ITEM:
		selection.sprite->setX( selection.sprite->getX() + x );
		selection.sprite->setY( selection.sprite->getY() + y );
		if( selection.type == ObjectIdentifier::ITEM ) {
			emit itemChanged( selection.strIndex );
		} else {
			emit zplaneChanged( selection.index );
		}
		break;
	case ObjectIdentifier::LIGHT:
		room->getLights()[selection.index].x += x;
		room->getLights()[selection.index].y += y;
		break;
	}
	updateGL();
}

void RoomEditorWidget::dragEnterEvent( QDragEnterEvent* event ) {
	const QMimeData* data = event->mimeData();

	if( data->formats().at( 0 ) == "x-application/lae_animation" ) {
		QString text( data->data( "x-application/lae_animation" ) );
		QFile file( "anims\\" + text );
		if( file.exists() ) {
			event->acceptProposedAction();
			return ;
		}
	}
	event->ignore();
}

void RoomEditorWidget::dropEvent( QDropEvent* event ) {
	const QMimeData* data = event->mimeData();
	QString text( data->data( "x-application/lae_animation" ) );

	QAction* newZPlaneA = new QAction( "new z-plane", this );
	QAction* newItemA = new QAction( "new item", this );
	QAction* cancelA = new QAction( "cancel", this );

	QMenu* menu = new QMenu( this );
	menu->addAction( newZPlaneA );
	menu->addAction( newItemA );
	menu->addAction( cancelA );

	QAction* r = menu->exec( mapToGlobal( event->pos() ) );
	if( r == newZPlaneA ) {
		emit newZplane( &text );
	} else if( r == newItemA ) {
		emit newItem( &text );
	}
}

void RoomEditorWidget::timerTick() {
	if( selection.colorInc ) {
		selection.color.r += 25;
		if( selection.color.r == 250 ) {
			selection.colorInc = false;
		}
	} else {
		selection.color.r -= 25;
		if( selection.color.r == 50 ) {
			selection.colorInc = true;
		}
	}
	selection.color.g = selection.color.b = selection.color.r;

	room->update( 100 );
	for( auto i = room->getZPlanes().begin(); i != room->getZPlanes().end(); ++i ) {
		(*i)->update( 100 );
	}

	updateGL();
}
