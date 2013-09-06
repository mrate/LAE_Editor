#ifndef ROOMEDITORWIDGET_H
#define ROOMEDITORWIDGET_H

#include <QGLWidget>
#include <QTimer>

#include "ObjectIdentifier.h"

#include <LAE_Core/screens/GameRoom.h>
#include <LAE_Core/graphics/AnimatedSprite.h>
#include <LAE_Core/graphics/Screen.h>

#include <map>

typedef std::multimap<int, ObjectIdentifier>	TZOrderMap;

/// Struktura vyberu objektu
struct SObjectSelection {
	bool			active;			///< vyber aktivni
	int				index;			///< index objektu
	int				vertex;			///< vrchol objektu
	std::string		strIndex;		///< nazev objektu
	ObjectIdentifier::EObjectType		type;			///< typ

	LAE::AnimatedSprite*	sprite;	///<

	// highlight objektu
	bool			colorInc;
	LAE::Color		color;

	SObjectSelection() : active(false), index(0), vertex(0), type(ObjectIdentifier::NONE), sprite(nullptr), colorInc(0), color(0, 0, 0, 255) {}
};

class RoomEditorWidget : public QGLWidget {
	Q_OBJECT

public:
	RoomEditorWidget( QWidget* parent, LAE::GameRoom& room, QGLContext* context );
	~RoomEditorWidget();

	void showZPlanes( bool show );
	void showBoxes( bool show );
	void showItems( bool show );
	void showScales( bool show );

	void selectZPlane( int index, LAE::AnimatedSprite* sprite );
	void selectItem( const std::string& name, LAE::AnimatedSprite* sprite );
	void selectBox( int index );
	void selectScale( int index );
	void selectLight( int index );
	void selectNone();

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

public slots:
	void itemsChanged();

signals:
	void itemChanged( const std::string& name );
	void zplaneChanged( int index );
	void boxChanged( int index );
	void scaleChanged( int index );
	void lightChanged( int index );

	void newZplane( QString* );
	void newItem( QString* );

private:
	LAE::Screen*		screen;
	LAE::GameRoom*	room;

	QTimer		timer;

	TZOrderMap	zorder;

	// selection
	SObjectSelection	selection;

	// edit
	int		lastX;
	int		lastY;

	int			scaleDebug;

	// view
	bool		show_zplanes;
	bool		show_boxes;
	bool		show_items;
	bool		show_scales;

	void drawNicePoly( const LAE::Point* p, const LAE::Color& color ) const;
	void initZOrder();
	void enableLights();
	void disableLights();

private slots:
	void timerTick();

protected:
	void initializeGL();
	void paintGL();
	void resizeGL( int width, int height );
	void mousePressEvent( QMouseEvent* event );
	void mouseMoveEvent( QMouseEvent* event );
	void keyPressEvent( QKeyEvent* event );

	void dragEnterEvent( QDragEnterEvent* event );
	void dropEvent( QDropEvent* event );
};

#endif // ROOMEDITORWIDGET_H
