#include "RoomEditorWindow.h"
// qt
#include <QFileDialog>
#include <QTreeWidgetItem>
#include <QMessageBox>
// stl
#include <string>
// LAE
#include <LAE_Core/graphics/Animation.h>
#include <LAE_Core/cache/AnimationCache.h>
#include <LAE_Core/cache/ImageCache.h>

using namespace LAE;

QString		style_zplane = "QTableView { background-color: #ffcccc; alternate-background-color: #ffaaaa; }";
QString		style_item = "QTableView { background-color: #ccffcc; alternate-background-color: #aaffaa; }";
QString		style_box = "QTableView { background-color: #ccccff; alternate-background-color: #aaaaff; }";
QString		style_scale = "QTableView { background-color: #ffccff; alternate-background-color: #ffaaff; }";
QString		style_light = "QTableView { background-color: #ccffff; alternate-background-color: #aaffff; }";
QString		style_room = "QTableView { background-color: #ffffcc; alternate-background-color: #ffffaa; }";

RoomEditorWindow::RoomEditorWindow( QGLContext* context, QWidget* parent )
	: QMainWindow( parent )
	, context( context ) 
	, roomBgImageSetter_( &room )
	{
	ui.setupUi( this );

	reWidget = new RoomEditorWidget( this, room, context );
	reWidget->resize( 4096, 4096 );
	ui.scrollArea->setWidget( reWidget );
	//reWidget->

	//roomConfig.setRoom( &room );
	createRoomConfig( roomConfig, &room );

	tableDelegate = new RoomObjectTableDelegate( room );
	ui.tableWidget->setItemDelegate( tableDelegate );

	connect( tableDelegate, SIGNAL( zorderChanged() ), reWidget, SLOT( itemsChanged() ) );
	connect( tableDelegate, SIGNAL( itemNameChanged( QString, QString ) ), this, SLOT( itemNameChanged( QString, QString ) ) );

	connect( ui.objectTreeWidget, SIGNAL( currentItemChanged( QTreeWidgetItem*, QTreeWidgetItem* ) ), this, SLOT( objectChanged( QTreeWidgetItem*, QTreeWidgetItem* ) ) );
	connect( ui.objectTreeWidget, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( treeWidgetContext( const QPoint& ) ) );

	connect( reWidget, SIGNAL( itemChanged( const std::string& ) ), this, SLOT( itemChanged( const std::string& ) ) );
	connect( reWidget, SIGNAL( zplaneChanged( int ) ), this, SLOT( zplaneChanged( int ) ) );
	connect( reWidget, SIGNAL( boxChanged( int ) ), this, SLOT( boxChanged( int ) ) );
	connect( reWidget, SIGNAL( scaleChanged( int ) ), this, SLOT( scaleChanged( int ) ) );
	connect( reWidget, SIGNAL( lightChanged( int ) ), this, SLOT( lightChanged( int ) ) );
	connect( reWidget, SIGNAL( newZplane( QString* ) ), this, SLOT( newZplane( QString* ) ) );
	connect( reWidget, SIGNAL( newItem( QString* ) ), this, SLOT( newItem( QString* ) ) );

	createActions();
	createMenu();
	createToolbar();

	newRoom();
}

RoomEditorWindow::~RoomEditorWindow() {

}

void RoomEditorWindow::createActions() {
	actionNew = new QAction( QIcon( ":/images/room" ), "New room", this );
	actionNew->setToolTip( "New" );
	actionNew->setStatusTip( "Create new room" );
	connect( actionNew, SIGNAL( triggered() ), this, SLOT( newRoom() ) );

	actionOpen = new QAction( QIcon( ":/images/open" ), "Open room", this );
	actionOpen->setToolTip( "Open" );
	actionOpen->setStatusTip( "Open room from file" );
	connect( actionOpen, SIGNAL( triggered() ), this, SLOT( openRoom() ) );

	actionSave = new QAction( QIcon( ":/images/save" ), "Save room", this );
	actionSave->setToolTip( "Save" );
	actionSave->setStatusTip( "Save room to file" );
	connect( actionSave, SIGNAL( triggered() ), this, SLOT( saveRoom() ) );

	actionExit = new QAction( QIcon( ":/images/exit" ), "Close window", this );
	actionExit->setToolTip( "Close" );
	actionExit->setStatusTip( "Close window" );
	connect( actionExit, SIGNAL( triggered() ), this, SLOT( closeWindow() ) );

	a_deleteZplane = new QAction( QIcon( ":/images/delete" ), "Delete z-plane", this );
	a_deleteZplane->setToolTip( "Delete z-plane" );
	a_deleteZplane->setStatusTip( "Delete z-plane" );
	connect( a_deleteZplane, SIGNAL( triggered() ), this, SLOT( deleteSelectedObject() ) );

	a_deleteItem = new QAction( QIcon( ":/images/delete" ), "Delete item", this );
	a_deleteItem->setToolTip( "Delete item" );
	a_deleteItem->setStatusTip( "Delete item" );
	connect( a_deleteItem, SIGNAL( triggered() ), this, SLOT( deleteSelectedObject() ) );

	a_deleteBox = new QAction( QIcon( ":/images/delete" ), "Delete box", this );
	a_deleteBox->setToolTip( "Delete box" );
	a_deleteBox->setStatusTip( "Delete box" );
	connect( a_deleteBox, SIGNAL( triggered() ), this, SLOT( deleteSelectedObject() ) );

	a_deleteScale = new QAction( QIcon( ":/images/delete" ), "Delete scale", this );
	a_deleteScale->setToolTip( "Delete scale" );
	a_deleteScale->setStatusTip( "Delete scale" );
	connect( a_deleteScale, SIGNAL( triggered() ), this, SLOT( deleteSelectedObject() ) );

	a_deleteLight = new QAction( QIcon( ":/images/delete" ), "Delete light", this );
	a_deleteLight->setToolTip( "Delete light" );
	a_deleteLight->setStatusTip( "Delete light" );
	connect( a_deleteLight, SIGNAL( triggered() ), this, SLOT( deleteSelectedObject() ) );

	a_newZplane = new QAction( QIcon( ":/images/add" ), "Add z-plane", this );
	a_newZplane->setToolTip( "Add z-plane" );
	a_newZplane->setStatusTip( "Add z-plane" );
	connect( a_newZplane, SIGNAL( triggered() ), this, SLOT( newZplane() ) );

	a_newItem = new QAction( QIcon( ":/images/add" ), "Add item", this );
	a_newItem->setToolTip( "Add item" );
	a_newItem->setStatusTip( "Add item" );
	connect( a_newItem, SIGNAL( triggered() ), this, SLOT( newItem() ) );

	a_newBox = new QAction( QIcon( ":/images/add" ), "Add box", this );
	a_newBox->setToolTip( "Add box" );
	a_newBox->setStatusTip( "Add box" );
	connect( a_newBox, SIGNAL( triggered() ), this, SLOT( newBox() ) );

	a_newScale = new QAction( QIcon( ":/images/add" ), "Add scale", this );
	a_newScale->setToolTip( "Add scale" );
	a_newScale->setStatusTip( "Add scale" );
	connect( a_newScale, SIGNAL( triggered() ), this, SLOT( newScale() ) );

	a_newLight = new QAction( QIcon( ":/images/add" ), "Add light", this );
	a_newLight->setToolTip( "Add light" );
	a_newLight->setStatusTip( "Add light" );
	connect( a_newLight, SIGNAL( triggered() ), this, SLOT( newLight() ) );
}

void RoomEditorWindow::createMenu() {
	QMenu* fileMenu = menuBar()->addMenu( tr( "&File" ) );
	fileMenu->addAction( actionNew );
	fileMenu->addAction( actionOpen );
	fileMenu->addAction( actionSave );
	fileMenu->addSeparator();
	fileMenu->addAction( actionExit );
}

void RoomEditorWindow::createToolbar() {
	ui.mainToolBar->addAction( actionNew );
	ui.mainToolBar->addAction( actionOpen );
	ui.mainToolBar->addAction( actionSave );
}

void RoomEditorWindow::newRoom() {
	room.free();
	initTreeView();
	reWidget->itemsChanged();
	selectNone();
	reWidget->updateGL();
}

void RoomEditorWindow::openRoom() {
	QString str = QFileDialog::getOpenFileName( this, "Open room", "", "room (*.xml)" );
	if( str != "" ) {
		try {
			const std::string& s = str.toStdString();
			room.load( s.c_str() );

			initTreeView();
			reWidget->itemsChanged();
			selectNone();
			reWidget->updateGL();
		} catch(std::exception& ex) {
			QMessageBox::critical(this, "Error", ex.what() );
		}
	}
}

void RoomEditorWindow::saveRoom() {
	QString str = QFileDialog::getSaveFileName( this, "Save room", "", "room (*.xml)" );
	if( str != "" ) {
		const std::string& s = str.toStdString();
		room.save( s.c_str() );
	}
}

void RoomEditorWindow::closeWindow() {
}

void RoomEditorWindow::initTreeView() {
	ui.objectTreeWidget->clear();
	QList<QTreeWidgetItem*> items;
	roomItem = new QTreeWidgetItem( ( QTreeWidgetItem* )0, QStringList( "Room" ) );
	roomItem->setIcon( 0, QIcon( ":/images/room" ) );

	zplanes = new QTreeWidgetItem( roomItem, QStringList( "z-planes" ) );
	zplanes->setIcon( 0, QIcon( ":/images/zplanes" ) );
	for( int i = 0; i < room.getZPlanes().size(); ++i ) {
		QTreeWidgetItem* zplane = new QTreeWidgetItem( zplanes, QStringList( QString::fromStdString( room.getZPlanes()[i]->getAnimation()->getFileName() ) ) );
		zplane->setIcon( 0, QIcon( ":/images/zplane" ) );
	}

	gitems = new QTreeWidgetItem( roomItem, QStringList( "items" ) );
	gitems->setIcon( 0, QIcon( ":/images/items" ) );
	for( ItemHashMap::const_iterator i = room.getItems().begin(); i != room.getItems().end(); ++i ) {
		QTreeWidgetItem* item = new QTreeWidgetItem( gitems, QStringList( i->first.c_str() ) );
		item->setIcon( 0, QIcon( ":/images/item" ) );
	}

	walking_boxes = new QTreeWidgetItem( roomItem, QStringList( "walking boxes" ) );
	walking_boxes->setIcon( 0, QIcon( ":/images/boxes" ) );
	for( int i = 0; i < room.getBoxes().size(); ++i ) {
		QTreeWidgetItem* box = new QTreeWidgetItem( walking_boxes, QStringList( QString( "box %1" ).arg( i ) ) );
		box->setIcon( 0, QIcon( ":/images/box" ) );
	}

	scales = new QTreeWidgetItem( roomItem, QStringList( "scales" ) );
	scales->setIcon( 0, QIcon( ":/images/scales" ) );
	for( int i = 0; i < room.getScales().size(); ++i ) {
		QTreeWidgetItem* scale = new QTreeWidgetItem( scales, QStringList( QString( "scale %1" ).arg( i ) ) );
		scale->setIcon( 0, QIcon( ":/images/scale" ) );
	}

	lights = new QTreeWidgetItem( roomItem, QStringList( "lights" ) );
	lights->setIcon( 0, QIcon( ":/images/lights" ) );
	for( int i = 0; i < room.getLights().size(); ++i ) {
		QTreeWidgetItem* light = new QTreeWidgetItem( lights, QStringList( QString( "light %1" ).arg( i ) ) );
		light->setIcon( 0, QIcon( ":/images/light" ) );
	}

	ui.objectTreeWidget->insertTopLevelItem( 0, roomItem );
}

void RoomEditorWindow::objectChanged( QTreeWidgetItem* item, QTreeWidgetItem* prev ) {
	if( !item ) {
		return ;
	}

	const QTreeWidgetItem* parent = item->parent();
	if( item == roomItem ) {
		selectNone();
		initTable( roomConfig, &style_room );
		ui.tableWidget->setEnabled( true );
	} else if( parent == zplanes ) {
		// selected zplane
		int index = parent->indexOfChild( item );
		selectZPlane( index );
	} else if( parent == gitems ) {
		// selected item
		selectItem( room.getItems()[item->text( 0 ).toStdString()] );
	} else if( parent == walking_boxes ) {
		// selected box
		int index = parent->indexOfChild( item );
		selectBox( index );
	} else if( parent == scales ) {
		// selected scale
		int index = parent->indexOfChild( item );
		selectScale( index );
	} else if( parent == lights ) {
		// light
		int index = parent->indexOfChild( item );
		selectLight( index );
	} else {
		selectNone();
	}
}

void RoomEditorWindow::treeWidgetContext( const QPoint& point ) {
	QMenu* menu;
	QTreeWidgetItem* item = ui.objectTreeWidget->currentItem();
	QTreeWidgetItem* parent = item->parent();

	if( item == gitems || parent == gitems ) {
		menu = new QMenu( this );
		menu->addAction( a_newItem );
	} else if( item == zplanes || parent == zplanes ) {
		menu = new QMenu( this );
		menu->addAction( a_newZplane );
	} else if( item == walking_boxes || parent == walking_boxes ) {
		menu = new QMenu( this );
		menu->addAction( a_newBox );
	} else if( item == scales || parent == scales ) {
		menu = new QMenu( this );
		menu->addAction( a_newScale );
	} else if( item == lights || parent == lights ) {
		menu = new QMenu( this );
		menu->addAction( a_newLight );
	} else {
		return ;
	}

	if( selection.type != ObjectIdentifier::NONE ) {
		switch( selection.type ) {
		case ObjectIdentifier::ZPLANE:
			menu->addAction( a_deleteZplane );
			break;
		case ObjectIdentifier::ITEM:
			menu->addAction( a_deleteItem );
			break;
		case ObjectIdentifier::BOX:
			menu->addAction( a_deleteBox );
			break;
		case ObjectIdentifier::SCALE:
			menu->addAction( a_deleteScale );
			break;
		case ObjectIdentifier::LIGHT:
			menu->addAction( a_deleteLight );
			break;
		}
	}

	menu->exec( ui.objectTreeWidget->mapToGlobal( point ) );
}

void RoomEditorWindow::initTable( TableItems& config, QString* style ) {
	if( style ) {
		ui.tableWidget->setStyleSheet( *style );
	}

	ui.tableWidget->clear();
	ui.tableWidget->setColumnCount( 2 );
	ui.tableWidget->setRowCount( config.count );
	ui.tableWidget->setHorizontalHeaderItem( 0, new QTableWidgetItem( "Property" ) );
	ui.tableWidget->setHorizontalHeaderItem( 1, new QTableWidgetItem( "Value" ) );

	QTableWidgetItem* item;
	for( int i = 0; i < config.count; ++i ) {
		item = new QTableWidgetItem( config.items[i]->getDescription() );
		item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
		ui.tableWidget->setItem( i, 0, item );

		item = new QTableWidgetItem( config.items[i]->toString() );
		item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable );
		ui.tableWidget->setItem( i, 1, item );
	}
	tableDelegate->setConfig( config );
	ui.tableWidget->setEnabled( true );
}

void RoomEditorWindow::selectZPlane( int index ) {
	selection.type = ObjectIdentifier::ZPLANE;
	selection.index = index;

	AnimatedSprite* zplane = room.getZPlanes()[index];
	//zplaneConfig.setZPlane( &zplane );
	createZPlaneConfig( zplaneConfig, zplane );
	initTable( zplaneConfig, &style_zplane );
	reWidget->selectZPlane( index, zplane );
}

void RoomEditorWindow::selectItem( Item& item ) {
	selection.type = ObjectIdentifier::ITEM;
	selection.strIndex = item.getName();

	//itemConfig.setItem( &item );
	createItemConfig( itemConfig, &item );
	initTable( itemConfig, &style_item );
	reWidget->selectItem( item.getName(), &item );
}

void RoomEditorWindow::selectBox( int index ) {
	selection.type = ObjectIdentifier::BOX;
	selection.index = index;

	WalkingBox& box = room.getBoxes()[index];
	//boxConfig.setBox( &box );
	createBoxConfig( boxConfig, &box );
	initTable( boxConfig, &style_box );
	reWidget->selectBox( index );
}

void RoomEditorWindow::selectScale( int index ) {
	selection.type = ObjectIdentifier::SCALE;
	selection.index = index;

	Scale& scale = room.getScales()[index];
	//scaleConfig.setScale( &scale );
	createScaleConfig( scaleConfig, &scale );
	initTable( scaleConfig, &style_scale );
	reWidget->selectScale( index );
}

void RoomEditorWindow::selectLight( int index ) {
	selection.type = ObjectIdentifier::LIGHT;
	selection.index = index;

	LightSource& light = room.getLights()[index];
	//lightConfig.setLight( &light );
	createLightConfig( lightConfig, &light );
	initTable( lightConfig, &style_light );
	reWidget->selectLight( index );
}

void RoomEditorWindow::selectNone() {
	selection.type = ObjectIdentifier::NONE;
	ui.tableWidget->setEnabled( false );
	reWidget->selectNone();
}

void RoomEditorWindow::zplaneChanged( int index ) {
	//zplaneConfig.setZPlane( &room.getZPlanes()[index] );
	createZPlaneConfig( zplaneConfig, room.getZPlanes()[index] );
	initTable( zplaneConfig, &style_zplane );
}

void RoomEditorWindow::itemChanged( const std::string& name ) {
	//itemConfig.setItem( &room.getItems()[name] );
	createItemConfig( itemConfig, &room.getItems()[name] );
	initTable( itemConfig, &style_item );
}

void RoomEditorWindow::scaleChanged( int index ) {
	//scaleConfig.setScale( &room.getScales()[index] );
	createScaleConfig( scaleConfig, &room.getScales()[index] );
	initTable( scaleConfig, &style_scale );
}

void RoomEditorWindow::boxChanged( int index ) {
	//boxConfig.setBox( &room.getBoxes()[index] );
	createBoxConfig( boxConfig, &room.getBoxes()[index] );
	initTable( boxConfig, &style_box );
}

void RoomEditorWindow::lightChanged( int index ) {
	//lightConfig.setLight( &room.getLights()[index] );
	createLightConfig( lightConfig, &room.getLights()[index] );
	initTable( lightConfig, &style_light );
}

void RoomEditorWindow::deleteSelectedObject() {
	switch( selection.type ) {
	case ObjectIdentifier::ZPLANE: {
		auto it = room.getZPlanes().begin();
		std::advance( it, selection.index );
		zplanes->removeChild( ui.objectTreeWidget->currentItem() );
		room.getZPlanes().erase( it );
		reWidget->itemsChanged();
		reWidget->updateGL();
	}
	break;
	case ObjectIdentifier::ITEM: {
		auto i = room.getItems().find( selection.strIndex );
		if( i != room.getItems().end() ) {
			gitems->removeChild( ui.objectTreeWidget->currentItem() );
			room.getItems().erase( i );
			reWidget->itemsChanged();
			reWidget->updateGL();
		}
	}
	break;
	case ObjectIdentifier::BOX: {
		WalkingBoxes::iterator it = room.getBoxes().begin();
		std::advance( it, selection.index );
		walking_boxes->removeChild( ui.objectTreeWidget->currentItem() );
		room.getBoxes().erase( it );
		reWidget->itemsChanged();
		reWidget->updateGL();
		for( int i = 0; i < walking_boxes->childCount(); ++i ) {
			QTreeWidgetItem* l = walking_boxes->child( i );
			l->setText( 0, QString( "box %1" ).arg( i ) );
		}
	}
	break;
	case ObjectIdentifier::SCALE: {
		Scales::iterator it = room.getScales().begin();
		for( int i = 0; i < selection.index; ++i ) {
			++it;
		}
		scales->removeChild( ui.objectTreeWidget->currentItem() );
		room.getScales().erase( it );
		for( int i = 0; i < scales->childCount(); ++i ) {
			QTreeWidgetItem* l = scales->child( i );
			l->setText( 0, QString( "scale %1" ).arg( i ) );
		}
	}
	break;
	case ObjectIdentifier::LIGHT: {
		Lights::iterator it = room.getLights().begin();
		for( int i = 0; i < selection.index; ++i ) {
			++it;
		}
		lights->removeChild( ui.objectTreeWidget->currentItem() );
		room.getLights().erase( it );
		reWidget->itemsChanged();
		reWidget->updateGL();
		for( int i = 0; i < lights->childCount(); ++i ) {
			QTreeWidgetItem* l = lights->child( i );
			l->setText( 0, QString( "light %1" ).arg( i ) );
		}
	}
	break;
	default:
		break;
	}
}

void RoomEditorWindow::itemNameChanged( QString newName, QString prevName ) {
	QTreeWidgetItem* item = ui.objectTreeWidget->currentItem();
	item->setText( 0, newName );

	std::string oname = prevName.toStdString();
	std::string nname = newName.toStdString();

	// nalezeni objektu
	ItemHashMap::iterator i = room.getItems().find( oname );
	// nastaveni noveho jmena
	i->second.setName( newName.toStdString() );
	// vlozeni noveho objektu
	room.getItems().insert( ItemHashMap::value_type( nname, i->second ) );
	// vybrani
	selectItem( room.getItems()[nname] );
	// smazani puvodniho
	room.getItems().erase( i );

	reWidget->itemsChanged();
}

void RoomEditorWindow::newZplane( QString* animation ) {
	std::string anim;
	if( !animation ) {
		// vybrani prvni animace
		QDir dir( "anims" );
		QStringList filter( "*.xml" );
		QStringList list = dir.entryList( filter );
		if( list.size() == 0 ) {
			QMessageBox msgBox( QMessageBox::Warning, "Error", "No aviable animation!", QMessageBox::Ok );
			msgBox.exec();
			return;
		}
		anim = list.at( 0 ).toStdString();
	} else {
		anim = animation->toStdString();
	}

	AnimatedSprite* sprite = new AnimatedSprite();
	sprite->setAnimation( AnimationManager::i()->createObject( anim ), true );

	// pridani do seznamu a map
	room.getZPlanes().push_back( sprite );
	QTreeWidgetItem* newItem = new QTreeWidgetItem( zplanes, QStringList( anim.c_str() ) );
	newItem->setIcon( 0, QIcon( ":/images/zplane" ) );
	reWidget->itemsChanged();

	ui.objectTreeWidget->setCurrentItem( newItem );
}

void RoomEditorWindow::newItem( QString* animation ) {
	std::string anim;
	if( !animation ) {
		// vybrani prvni animace
		QDir dir( "anims" );
		QStringList filter( "*.xml" );
		QStringList list = dir.entryList( filter );
		if( list.size() == 0 ) {
			QMessageBox msgBox( QMessageBox::Warning, "Error", "No aviable animation!", QMessageBox::Ok );
			msgBox.exec();
			return;
		}
		anim = list.at( 0 ).toStdString();
	} else {
		anim = animation->toStdString();
	}

	Item item;
	item.setAnimation( AnimationManager::i()->createObject( anim ), true );

	// vygenerovani jmena
	int index = 0;
	QString name;
	while( true ) {
		name = QString( "new_item_%1" ).arg( index++ );
		if( room.getItems().count( name.toStdString() ) == 0 ) {
			item.setName( name.toStdString() );
			break;
		}
	}

	// pridani do seznamu a map
	room.getItems().insert( ItemHashMap::value_type( item.getName(), item ) );
	QTreeWidgetItem* newItem = new QTreeWidgetItem( gitems, QStringList( name ) );
	newItem->setIcon( 0, QIcon( ":/images/item" ) );
	reWidget->itemsChanged();
	ui.objectTreeWidget->setCurrentItem( newItem );
}

void RoomEditorWindow::newBox() {
	WalkingBox box;

	box.p[0].x = 10;
	box.p[0].y = 10;
	box.p[1].x = 100;
	box.p[1].y = 10;
	box.p[2].x = 100;
	box.p[2].y = 100;
	box.p[3].x = 10;
	box.p[3].y = 100;

	room.getBoxes().push_back( box );
	QTreeWidgetItem* newBox = new QTreeWidgetItem( walking_boxes, QStringList( QString( "box %1" ).arg( room.getBoxes().size() - 1 ) ) );
	newBox->setIcon( 0, QIcon( ":/images/box" ) );
	reWidget->itemsChanged();
	ui.objectTreeWidget->setCurrentItem( newBox );
}

void RoomEditorWindow::newScale() {
	Scale scale;

	room.getScales().push_back( scale );
	QTreeWidgetItem* newScale = new QTreeWidgetItem( scales, QStringList( QString( "scale %1" ).arg( room.getScales().size() - 1 ) ) );
	newScale->setIcon( 0, QIcon( ":/images/scale" ) );
	reWidget->itemsChanged();
	ui.objectTreeWidget->setCurrentItem( newScale );
}

void RoomEditorWindow::newLight() {
	LightSource light;

	room.getLights().push_back( light );
	QTreeWidgetItem* newLight = new QTreeWidgetItem( lights, QStringList( QString( "light %1" ).arg( room.getLights().size() - 1 ) ) );
	newLight->setIcon( 0, QIcon( ":/images/light" ) );
	reWidget->itemsChanged();
	ui.objectTreeWidget->setCurrentItem( newLight );
}

RoomEditorWindow::RoomImageSetter::RoomImageSetter( LAE::GameRoom* room ) : room_(room) {
	if( room_->getBgImage() ) {
		file_.file = room_->getBgImage()->getFileName();
	}
}

void RoomEditorWindow::RoomImageSetter::setImage( const FileProperty& image ) {
	QDir dir( QApplication::applicationDirPath() );
	file_.file = dir.relativeFilePath( image.file.c_str() ).toStdString();

	room_->setBgImage( ImageManager::i()->createObject( file_.file ) );
}

const FileProperty& RoomEditorWindow::RoomImageSetter::getImage() const {
	//return room_->getBgImage()->getFileName();
	return file_;
}

void RoomEditorWindow::createRoomConfig( TableItems& config, LAE::GameRoom* room ) {
	config.resize(5);

	config.setItem( 0, new TableItem<FileProperty>( "Background", &roomBgImageSetter_, &RoomImageSetter::setImage, &RoomImageSetter::getImage ) );
	config.setItem( 1, new TableItem<unsigned short>( "Width", room, &GameRoom::setWidth, &GameRoom::getWidth, 1, 10000 ) );
	config.setItem( 2, new TableItem<unsigned short>( "Height", room, &GameRoom::setHeight, &GameRoom::getHeight, 1, 10000 ) );
	config.setItem( 3, new TableItem<std::string>( "Script", room, &GameRoom::setRoomScript, &GameRoom::getRoomScript ) );
	config.setItem( 4, new TableItem<std::string>( "Text", room, &GameRoom::setTextFile, &GameRoom::getTextFile ) );

	//config.setItem( 0, new ( TA_SPRITE, "Background", &r->bgImage_ ) );
	//config.setItem( 1, STableItem( TA_SHORT, "Width", &r->width_, 1, 10000 );
	//config.setItem( 2, STableItem( TA_SHORT, "Height", &r->height_, 1, 10000 );
	//config.setItem( 3, STableItem( TA_STRING, "Script", &r->roomScript_, 0, 255 );
	//config.setItem( 4, STableItem( TA_STRING, "Text", &r->textFile_, 0, 255 );
}

void RoomEditorWindow::createZPlaneConfig( TableItems& config, LAE::AnimatedSprite* zplane ) const {
	config.resize(6);
	config.setItem( 0, new TableItem<FileProperty, DeletePolicy>( "Animation", new AnimationSetter<LAE::AnimatedSprite>(zplane), &AnimationSetter<LAE::AnimatedSprite>::setAnimation, &AnimationSetter<LAE::AnimatedSprite>::getAnimation ) );
	config.setItem( 1, new TableItem<short>( "X", zplane, &AnimatedSprite::setX, &AnimatedSprite::getX, -10000, 10000 ) );
	config.setItem( 2, new TableItem<short>( "Y", zplane, &AnimatedSprite::setY, &AnimatedSprite::getY, -10000, 10000 ) );
	config.setItem( 3, new TableItem<short>( "Z", zplane, &AnimatedSprite::setZ, &AnimatedSprite::getZ, 0, 100 ) );
	config.setItem( 4, new TableItem<Screen::EffectType>( "Effect", zplane, &AnimatedSprite::setEffect, &AnimatedSprite::getEffect ) );
	config.setItem( 5, new TableItem<bool>( "Lightable", zplane, &AnimatedSprite::setLightable, &AnimatedSprite::isLightable ) );

	//citems[0] = STableItem( TA_ANIMATION, "Animation", s );
	//citems[1] = STableItem( TA_SHORT, "X", &s->x, -10000, 10000 );
	//citems[2] = STableItem( TA_SHORT, "Y", &s->y, -10000, 10000 );
	//citems[3] = STableItem( TA_ZORDER, "Z", &s->z, 0, 100 );
	//citems[4] = STableItem( TA_EFFECT, "Effect", &s->effect, 0, 100 );
	//citems[5] = STableItem( TA_BOOL, "Lightable", &s->lightable );
}

void RoomEditorWindow::createItemConfig( TableItems& config, LAE::Item* item ) const {
	config.resize(16);

	config.setItem( 0, new TableItem<bool>( "Active", item, &Item::setActive, &Item::isActive ) );
	config.setItem( 1, new TableItem<std::string>( "Name", item, &Item::setName, &Item::getName, 1, 255 ) );
	config.setItem( 2, new TableItem<std::string>( "Description", item, &Item::setDescription, &Item::getDescription, 0, 255 ) );
	config.setItem( 3, new TableItem<std::string>( "Cursor name", item, &Item::setCursorName, &Item::getCursorName, 0, 255 ) );
	config.setItem( 4, new TableItem<FileProperty, DeletePolicy>( "Animation", new AnimationSetter<LAE::AnimatedSprite>(item), &AnimationSetter<LAE::AnimatedSprite>::setAnimation, &AnimationSetter<LAE::AnimatedSprite>::getAnimation ) );
	config.setItem( 5, new TableItem<std::string>( "OnExamine", item, &Item::setOnExamine, &Item::getOnExamine, 0, 255 ) );
	config.setItem( 6, new TableItem<std::string>( "OnUse", item, &Item::setOnUse, &Item::getOnUse, 0, 255 ) );
	config.setItem( 7, new TableItem<short>( "X", item, &Item::setX, &Item::getX, -10000, 10000 ) );
	config.setItem( 8, new TableItem<short>( "Y", item, &Item::setY, &Item::getY, -10000, 10000 ) );
	config.setItem( 9, new TableItem<short>( "Z", item, &Item::setZ, &Item::getZ, 0, 100 ) );
	config.setItem( 10, new TableItem<int>( "Active rect X", &item->getActiveRect(), &Rectangle::setX, &Rectangle::x, 0, 10000 ) );
	config.setItem( 11, new TableItem<int>( "Active rect Y", &item->getActiveRect(), &Rectangle::setY, &Rectangle::y, 0, 10000 ) );
	config.setItem( 12, new TableItem<int>( "Active rect Width", &item->getActiveRect(), &Rectangle::setWidth, &Rectangle::width, 0, 10000 ) );
	config.setItem( 13, new TableItem<int>( "Active rect Height", &item->getActiveRect(), &Rectangle::setHeight, &Rectangle::height, 0, 10000 ) );
	config.setItem( 14, new TableItem<Screen::EffectType>( "Effect", item, &Item::setEffect, &Item::getEffect ) );
	config.setItem( 15, new TableItem<bool>( "Lightable", item, &Item::setLightable, &Item::isLightable ) );

	//citems[0] = STableItem( TA_BOOL, "Active", &i->active );
	//citems[1] = STableItem( TA_ITEMNAME, "Name", &i->name, 1, 255 );
	//citems[2] = STableItem( TA_STRING, "Description", &i->descr, 0, 255 );
	//citems[3] = STableItem( TA_STRING, "Cursor", &i->cursor, 0, 255 );
	//citems[4] = STableItem( TA_ANIMATION, "Animation", i );
	//citems[5] = STableItem( TA_STRING, "OnExamine", &i->onExamine, 0, 255 );
	//citems[6] = STableItem( TA_STRING, "OnUse", &i->onUse, 0, 255 );
	//citems[7] = STableItem( TA_SHORT, "X", &i->x, -10000, 10000 );
	//citems[8] = STableItem( TA_SHORT, "Y", &i->y, -10000, 10000 );
	//citems[9] = STableItem( TA_ZORDER, "Z", &i->z, 0, 100 );
	//citems[10] = STableItem( TA_SHORT, "Width", &i->w, 0, 10000 );
	//citems[11] = STableItem( TA_SHORT, "Height", &i->h, 0, 10000 );
	//citems[12] = STableItem( TA_EFFECT, "Effect", &i->effect, 0, 100 );
	//citems[13] = STableItem( TA_BOOL, "Lightable", &i->lightable );
}

void RoomEditorWindow::createBoxConfig( TableItems& config, LAE::WalkingBox* box ) const {
	config.resize(11);

	config.setItem( 0, new TableItem<bool>( "Active", &box->active ) );
	config.setItem( 1, new TableItem<unsigned char>( "Scale", &box->scale, 0, 255 ) );
	config.setItem( 2, new TableItem<short>( "Z", &box->z, 0, 100 ) );
	config.setItem( 3, new TableItem<double>( "X 1", &box->p[0].x, -10000, 10000 ) );
	config.setItem( 4, new TableItem<double>( "Y 1", &box->p[0].y, -10000, 10000 ) );
	config.setItem( 5, new TableItem<double>( "X 2", &box->p[1].x, -10000, 10000 ) );
	config.setItem( 6, new TableItem<double>( "Y 2", &box->p[1].y, -10000, 10000 ) );
	config.setItem( 7, new TableItem<double>( "X 3", &box->p[2].x, -10000, 10000 ) );
	config.setItem( 8, new TableItem<double>( "Y 3", &box->p[2].y, -10000, 10000 ) );
	config.setItem( 9, new TableItem<double>( "X 4", &box->p[3].x, -10000, 10000 ) );
	config.setItem( 10, new TableItem<double>( "Y 4", &box->p[3].y, -10000, 10000 ) );

	//citems[0] = STableItem( TA_BOOL, "Active", &b->active );
	//citems[1] = STableItem( TA_UCHAR, "Scale", &b->scale, 0, 255 );
	//citems[2] = STableItem( TA_ZORDER, "Z", &b->z, 0, 100 );
	//citems[3] = STableItem( TA_SHORT, "X 1", &b->p[0].x, -10000, 10000 );
	//citems[4] = STableItem( TA_SHORT, "Y 1", &b->p[0].y, -10000, 10000 );
	//citems[5] = STableItem( TA_SHORT, "X 2", &b->p[1].x, -10000, 10000 );
	//citems[6] = STableItem( TA_SHORT, "Y 2", &b->p[1].y, -10000, 10000 );
	//citems[7] = STableItem( TA_SHORT, "X 3", &b->p[2].x, -10000, 10000 );
	//citems[8] = STableItem( TA_SHORT, "Y 3", &b->p[2].y, -10000, 10000 );
	//citems[9] = STableItem( TA_SHORT, "X 4", &b->p[3].x, -10000, 10000 );
	//citems[10] = STableItem( TA_SHORT, "Y 4", &b->p[3].y, -10000, 10000 );
}

void RoomEditorWindow::createScaleConfig( TableItems& config, LAE::Scale* scale ) const {
	config.resize(4);

	config.setItem( 0, new TableItem<double>( "Y 1", &scale->p0.y, -10000, 10000 ) );
	config.setItem( 1, new TableItem<unsigned char>( "Scale 1", &scale->scale1, 0, 255 ) );
	config.setItem( 2, new TableItem<double>( "Y 2", &scale->p1.y, -10000, 10000 ) );
	config.setItem( 3, new TableItem<unsigned char>( "Scale 2", &scale->scale2, 0, 255 ) );

	//citems[0] = STableItem( TA_SHORT, "Y 1", &s->p0.y, -10000, 10000 );
	//citems[1] = STableItem( TA_UCHAR, "Scale 1", &s->scale1, 0, 255 );
	//citems[2] = STableItem( TA_SHORT, "Y 2", &s->p1.y, -10000, 10000 );
	//citems[3] = STableItem( TA_UCHAR, "Scale 2", &s->scale2, 0, 255 );
}

void RoomEditorWindow::createLightConfig( TableItems& config, LAE::LightSource* light ) const {
	config.resize(4);

	config.setItem( 0, new TableItem<bool>( "Active", &light->active ) );
	config.setItem( 1, new TableItem<short>( "X", &light->x, -10000, 10000 ) );
	config.setItem( 2, new TableItem<short>( "Y", &light->y, -10000, 10000 ) );
	config.setItem( 3, new TableItem<unsigned char>( "Intensity", &light->intensity, 0, 255 ) );

	//citems[0] = STableItem( TA_BOOL, "Active", &l->active );
	//citems[1] = STableItem( TA_SHORT, "X", &l->x, -10000, 10000 );
	//citems[2] = STableItem( TA_SHORT, "Y", &l->y, -10000, 10000 );
	//citems[3] = STableItem( TA_UCHAR, "Intensity", &l->intensity, 0, 255 );
}
