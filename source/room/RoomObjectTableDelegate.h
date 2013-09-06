#ifndef ROOMOBJECTTABLEDELEGATE_H
#define ROOMOBJECTTABLEDELEGATE_H

// qt
#include <QItemDelegate>
// LAE
#include <LAE_Core/types/types.h>
#include <LAE_Core/objects/Item.h>
// LAE_Tools
#include "../common/PropertyTable.h"

class RoomObjectTableDelegate : public QItemDelegate {
	Q_OBJECT

public:
	RoomObjectTableDelegate( LAE::GameRoom& room, QWidget* parent = 0 );
	~RoomObjectTableDelegate();

	void setConfig( TableItems& config );

	QWidget* createEditor( QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& index ) const;
	void setEditorData( QWidget* editor, const QModelIndex& index ) const;
	void setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const;

signals:
	void zorderChanged() const;
	void itemNameChanged( QString, QString ) const;

private:
	TableItems* config;
	LAE::GameRoom*	room;
};

#endif // ROOMOBJECTTABLEDELEGATE_H
