#include "RoomObjectTableDelegate.h"

#include <QSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QDir>
#include <QMessageBox>

using namespace LAE;

RoomObjectTableDelegate::RoomObjectTableDelegate( GameRoom& room, QWidget* parent )
	: QItemDelegate( parent )
	, room( &room )
	, config( nullptr ) {

}

RoomObjectTableDelegate::~RoomObjectTableDelegate() {

}

QWidget* RoomObjectTableDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& index ) const {
	int row = index.row();
	if( row > config->count ) {
		return nullptr;
	}

	ITableItem* tableItem = config->items[row];
	return tableItem->createEditor(parent);
}

void RoomObjectTableDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const {
	int row = index.row();
	if( row > config->count ) {
		return ;
	}

	ITableItem* tableItem = config->items[row];
	tableItem->setEditorData(editor);
}

void RoomObjectTableDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const {
	int row = index.row();
	if( row > config->count ) {
		return ;
	}

	ITableItem* tableItem = config->items[row];
	tableItem->setModelData(editor, model, index);

	/*
	STableItem& item = config->items[row];
	switch( item.type ) {
	case TA_BOOL: {
		QComboBox* combo = qobject_cast<QComboBox*>( editor );
		bool* b = ( bool* )item.data;
		*b = combo->currentIndex() == 0;
		model->setData( index, ( *b ? "true" : "false" ) );
		break;
	}
	break;
	case TA_ITEMNAME:
	case TA_STRING: {
		QLineEdit* edit = qobject_cast<QLineEdit*>( editor );
		std::string* str = ( std::string* )item.data;
		std::string newStr = edit->text().toStdString();
		if( *str == newStr ) {
			return ;
		}

		if( item.type == TA_ITEMNAME ) {
			if( room->items_.count( newStr ) > 0 ) {
				// jmeno uz existuje
				QMessageBox msgBox( QMessageBox::Warning, "Error", "Item with this name already exists!", QMessageBox::Ok );
				msgBox.exec();
				return ;
			} else {
				emit itemNameChanged( newStr.c_str(), str->c_str() );
			}
		} else {
			*str = newStr;
		}
		model->setData( index, edit->text() );
	}
	break;
	case TA_ZORDER:
	case TA_UCHAR:
	case TA_INT:
	case TA_SHORT: {
		QSpinBox* spinBox = qobject_cast<QSpinBox*>( editor );
		if( item.type == TA_INT ) {
			int* i = ( int* )item.data;
			*i = spinBox->value();
		} else if( item.type == TA_SHORT || item.type == TA_ZORDER ) {
			short* i = ( short* )item.data;
			*i = spinBox->value();
		} else {
			unsigned char* c = ( unsigned char* )item.data;
			*c = spinBox->value();
		}
		model->setData( index, QString().setNum( spinBox->value() ) );
		if( item.type == TA_ZORDER ) {
			emit zorderChanged();
		}
	}
	break;
	case TA_SPRITE: {
		QComboBox* box = qobject_cast<QComboBox*>( editor );
		std::string* str = ( std::string* )item.data;
		*str = box->currentText().toStdString();
		model->setData( index, box->currentText() );
	}
	break;
	case TA_ANIMATION: {
		QComboBox* box = qobject_cast<QComboBox*>( editor );
		AnimatedSprite* as = ( AnimatedSprite* )item.data;
		as->setAnimation( box->currentText().toStdString(), true );
		model->setData( index, box->currentText() );
	}
	break;
	case TA_EFFECT: {
		QComboBox* box = qobject_cast<QComboBox*>( editor );
		Screen::EffectType* effect = ( Screen::EffectType* )item.data;
		*effect = ( Screen::EffectType )box->currentIndex();
		model->setData( index, box->currentText() );
	}
	break;
	}
	*/
}

//void RoomObjectTableDelegate::setConfig( STableConfig& cfg ) {
//	config = &cfg;
//}

void RoomObjectTableDelegate::setConfig( TableItems& cfg ) {
	config = &cfg;
}
