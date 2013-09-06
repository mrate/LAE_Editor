#include "PropertyTable.h"
#include <sstream>

using namespace LAE;

//template<>
//class EditorCreatorImpl<LAE::Screen::EffectType> {
//public:
//	static const QString Captions[2];
//
//	QWidget* createEditor( QWidget* parent ) const {
//		return new QComboBox(parent); 
//	}
//	void setEditorData(QWidget* editor, LAE::Screen::EffectType value ) const {
//		/*static_cast<QComboBox*>(editor)->setText(value.c_str());*/ 
//	}
//	LAE::Screen::EffectType getEditorData(QWidget* editor) const { 
//		return static_cast<LAE::Screen::EffectType>( static_cast<QComboBox*>(editor)->currentIndex() ); 
//	}
//	QString toString( LAE::Screen::EffectType value ) const { 
//		return Captions[value]; 
//	}
//};

const QString EditorCreatorImpl<LAE::Screen::EffectType>::Captions[2] = { "None", "LightRay" };

void TableItems::resize( int size ) {
	if( items ) {
		deleteAll();
	}
	items = new ITableItem*[size];
	for( int i = 0; i < size; ++i ) {
		items[i] = 0;
	}
	count = size;
}

TableItems::~TableItems() {
	deleteAll();
}

void TableItems::setItem( int index, ITableItem* config ) {
	if( items[index] ) {
		delete items[index];
	}
	items[index] = config;
}

void TableItems::deleteAll() {
	if( items ) {
		for( int i = 0; i < count; ++i ) {
			delete items[i];
		}
		delete[] items;
		items = 0;
		count = 0;
	}
}
