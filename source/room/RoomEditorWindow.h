#ifndef ROOMEDITORWINDOW_H
#define ROOMEDITORWINDOW_H

// qt
#include <QMainWindow>
#include <QDir>
// local
#include "ui_RoomEditorWindow.h"
#include "RoomEditorWidget.h"
#include "RoomObjectTableDelegate.h"
#include "ObjectIdentifier.h"
#include "../common/PropertyTable.h"
// LAE
#include <LAE_Core/graphics/AnimatedSprite.h>
#include <LAE_Core/cache/AnimationCache.h>

class RoomEditorWindow : public QMainWindow {
	Q_OBJECT

public:
	RoomEditorWindow( QGLContext* context, QWidget* parent = 0 );
	~RoomEditorWindow();

private:
	Ui::RoomEditorWindowClass ui;
	RoomEditorWidget*	reWidget;

	QGLContext* context;

	QAction*		actionNew;
	QAction*		actionOpen;
	QAction*		actionSave;
	QAction*		actionExit;

	QAction*		a_newZplane;
	QAction*		a_deleteZplane;
	QAction*		a_newItem;
	QAction*		a_deleteItem;
	QAction*		a_newBox;
	QAction*		a_deleteBox;
	QAction*		a_newScale;
	QAction*		a_deleteScale;
	QAction*		a_newLight;
	QAction*		a_deleteLight;

	void createActions();
	void createMenu();
	void createToolbar();
	void initTreeView();

	void createRoomConfig( TableItems& config, LAE::GameRoom* room ) ;
	void createZPlaneConfig( TableItems& config, LAE::AnimatedSprite* zplane ) const;
	void createItemConfig( TableItems& config, LAE::Item* item ) const;
	void createBoxConfig( TableItems& config, LAE::WalkingBox* box ) const;
	void createScaleConfig( TableItems& config, LAE::Scale* scale ) const;
	void createLightConfig( TableItems& config, LAE::LightSource* light ) const;

	// game room
	LAE::GameRoom		room;

	// tree view
	QTreeWidgetItem* roomItem;
	QTreeWidgetItem* zplanes;
	QTreeWidgetItem* gitems;
	QTreeWidgetItem* walking_boxes;
	QTreeWidgetItem* scales;
	QTreeWidgetItem* lights;

	// table
	RoomObjectTableDelegate* tableDelegate;

	TableItems		roomConfig;
	TableItems		itemConfig;
	TableItems		zplaneConfig;
	TableItems		scaleConfig;
	TableItems		boxConfig;
	TableItems		lightConfig;
	
	class RoomImageSetter {
	public:
		RoomImageSetter( LAE::GameRoom* room );
		void setImage( const FileProperty& image );
		const FileProperty& getImage() const;
	private:
		LAE::GameRoom* room_;
		FileProperty file_;
	};

	RoomImageSetter roomBgImageSetter_;

	template<typename T>
	class AnimationSetter {
	public:
		AnimationSetter( T* t ) : t_(t) {
			file_.file = t->getAnimation()->getFileName();
		}

		void setAnimation( const FileProperty& anim ) { 
			QDir dir( QApplication::applicationDirPath() );
			file_.file = dir.relativeFilePath( anim.file.c_str() ).toStdString();

			t_->setAnimation( LAE::AnimationManager::i()->createObject(file_.file) );
		}
		const FileProperty& getAnimation() const { 
			return file_; 
		}

	private:
		T* t_;
		mutable FileProperty file_;
	};	

	void initTable( TableItems& config, QString* style = 0 );

	ObjectIdentifier		selection;

private slots:
	void newRoom();
	void openRoom();
	void saveRoom();
	void closeWindow();

	void selectZPlane( int index );
	void selectItem( LAE::Item& item );
	void selectBox( int index );
	void selectScale( int index );
	void selectLight( int index );
	void selectNone();

	void deleteSelectedObject();

	void newZplane( QString* animation = 0 );
	void newItem( QString* animation = 0 );
	void newBox();
	void newScale();
	void newLight();

	void objectChanged( QTreeWidgetItem*, QTreeWidgetItem* );
	void treeWidgetContext( const QPoint& );

	void zplaneChanged( int index );
	void itemChanged( const std::string& name );
	void scaleChanged( int index );
	void boxChanged( int index );
	void lightChanged( int index );

	void itemNameChanged( QString newName, QString prevName );
};

#endif // ROOMEDITORWINDOW_H
