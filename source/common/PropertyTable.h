#ifndef _PROPERTY_TABLE_H_
#define _PROPERTY_TABLE_H_

// stl
#include <string>
// LAE
#include <LAE_Core/objects/Item.h>
#include <LAE_Core/screens/GameRoom.h>
// qt
#include <QCheckBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QDoubleSpinBox>
// boost
#ifndef Q_MOC_RUN
#	include <boost/call_traits.hpp>
#	include <boost/type_traits.hpp>
#endif
// local
#include "SelectFileWidget.h"

enum ETableItemType {
    TA_BOOL,
    TA_STRING,
    TA_UCHAR,
    TA_INT,
    TA_SHORT,
    TA_ANIMATION,
    TA_SPRITE,
    TA_EFFECT,
    TA_ZORDER,
    TA_ITEMNAME
};

struct FileProperty {
	std::string file;

	FileProperty() {}
	explicit FileProperty( const std::string& file ) : file(file) {}
};

//////////////////////////////////////////////////////////////////////////
template<typename __ValueType>
class EditorCreatorImpl {};

template<>
class EditorCreatorImpl<bool> {
public:
	QWidget* createEditor( QWidget* parent ) const { return new QCheckBox(parent); }
	void setEditorData(QWidget* editor, bool value ) const { static_cast<QCheckBox*>(editor)->setChecked(value); }
	bool getEditorData(QWidget* editor) const { return static_cast<QCheckBox*>(editor)->isChecked(); }
	QString toString( bool value ) const { return value ? "true" : "false"; }	
	void setModelData( QAbstractItemModel* model, const QModelIndex& index, bool value ) const { model->setData(index, value); }
};

template<>
class EditorCreatorImpl<std::string> {
public:
	EditorCreatorImpl( int minLength = 0, int maxLength = std::numeric_limits<int>::max() ) 
		: min_(minLength)
		, max_(maxLength)
	{}

	QWidget* createEditor( QWidget* parent ) const { QLineEdit* ret = new QLineEdit(parent); ret->setMaxLength(max_); return ret; }
	void setEditorData(QWidget* editor, const std::string& value ) const { static_cast<QLineEdit*>(editor)->setText(value.c_str()); }
	std::string getEditorData(QWidget* editor) const { return static_cast<QLineEdit*>(editor)->text().toStdString(); }
	QString toString( std::string value ) const { return QString::fromStdString( value ); }
	void setModelData( QAbstractItemModel* model, const QModelIndex& index, const std::string& value ) const { model->setData(index, QString::fromStdString(value)); }

private:
	int min_;
	int max_;
};

template<>
class EditorCreatorImpl<FileProperty> {
public:
	EditorCreatorImpl() 
	{}

	QWidget* createEditor( QWidget* parent ) const { SelectFileWidget* ret = new SelectFileWidget(parent); ret->setEditable(false); return ret; }
	void setEditorData(QWidget* editor, const FileProperty& value ) const { static_cast<SelectFileWidget*>(editor)->setFileName(QString::fromStdString(value.file)); }
	FileProperty getEditorData(QWidget* editor) const { return FileProperty(static_cast<SelectFileWidget*>(editor)->getFileName().toStdString()); }
	QString toString( FileProperty value ) const { return QString::fromStdString( value.file ); }
	void setModelData( QAbstractItemModel* model, const QModelIndex& index, const FileProperty& value ) const { model->setData(index, QString::fromStdString(value.file)); }

private:
	int min_;
	int max_;
};

template<typename __Type>
class NumericEditorCreator {
public:
	NumericEditorCreator( __Type min = std::numeric_limits<__Type>::min(), __Type max = std::numeric_limits<__Type>::max() )
		: min_( min ), max_( max ) {
	}

	QWidget* createEditor( QWidget* parent ) const { QSpinBox* ret = new QSpinBox(parent); ret->setMinimum(min_); ret->setMaximum(max_); return ret; }
	void setEditorData(QWidget* editor, __Type value ) const { static_cast<QSpinBox*>(editor)->setValue(value); }
	__Type getEditorData(QWidget* editor) const { return static_cast<QSpinBox*>(editor)->value(); }
	QString toString( __Type value ) const { QString tmp; tmp.setNum( value ); return tmp; }
	void setModelData( QAbstractItemModel* model, const QModelIndex& index, __Type value ) const { model->setData(index, value); }

private:
	__Type min_;
	__Type max_;
};

template<>
class NumericEditorCreator<double> {
public:
	NumericEditorCreator( double min = std::numeric_limits<double>::min(), double max = std::numeric_limits<double>::max() )
		: min_( min ), max_( max ) {
	}

	QWidget* createEditor( QWidget* parent ) const { QDoubleSpinBox* ret = new QDoubleSpinBox(parent); ret->setMinimum(min_); ret->setMaximum(max_); return ret; }
	void setEditorData(QWidget* editor, double value ) const { static_cast<QDoubleSpinBox*>(editor)->setValue(value); }
	double getEditorData(QWidget* editor) const { return static_cast<QDoubleSpinBox*>(editor)->value(); }
	QString toString( double value ) const { QString tmp; tmp.setNum( value ); return tmp; }
	void setModelData( QAbstractItemModel* model, const QModelIndex& index, double value ) const { model->setData(index, value); }

private:
	double min_;
	double max_;
};

template<>
class EditorCreatorImpl<LAE::Screen::EffectType> {
public:
	static const QString Captions[2];

	QWidget* createEditor( QWidget* parent ) const {
		QComboBox* ret = new QComboBox(parent); 
		for( int i = 0; i < sizeof(Captions) / sizeof(QString); ++i ) {
			ret->addItem(Captions[i]);
		}
		return ret;
	}
	void setEditorData(QWidget* editor, LAE::Screen::EffectType value ) const {
		/*static_cast<QComboBox*>(editor)->setText(value.c_str());*/ 
	}
	LAE::Screen::EffectType getEditorData(QWidget* editor) const { 
		return static_cast<LAE::Screen::EffectType>( static_cast<QComboBox*>(editor)->currentIndex() ); 
	}
	QString toString( LAE::Screen::EffectType value ) const { 
		return Captions[value]; 
	}
	
	void setModelData( QAbstractItemModel* model, const QModelIndex& index, LAE::Screen::EffectType value ) const { model->setData(index, value); }
};

//////////////////////////////////////////////////////////////////////////

template<typename __Type, bool __IsIntegral>
struct EditorCreatorType {
	typedef EditorCreatorImpl< __Type > CreatorType;
};

template<>
struct EditorCreatorType< bool, true > {
	typedef EditorCreatorImpl< bool > CreatorType;
};

template<typename __Type>
struct EditorCreatorType< __Type, true > {
	typedef NumericEditorCreator< __Type > CreatorType;
};

//////////////////////////////////////////////////////////////////////////

template<typename __Type>
struct EditorCreator {
	typedef typename EditorCreatorType< __Type, boost::is_integral< __Type >::value || boost::is_floating_point< __Type >::value>::CreatorType CreatorType;
};

//////////////////////////////////////////////////////////////////////////
//template<typename __Type, bool __IsConst>
//struct RemoveConst {
//	typedef __Type type;
//};
//
//template<typename __Type>
//struct RemoveConst<__Type, true> {
//	typedef typename boost::remove_const<__Type>::type type;
//};
//////////////////////////////////////////////////////////////////////////

class ITableItem {
public:
	ITableItem( const QString& description ) : description_( description ) {}
	virtual ~ITableItem() {}

	const QString& getDescription() const { return description_; }
	
	virtual QString toString() const = 0;
	virtual int getType() const = 0;

	virtual QWidget* createEditor( QWidget* parent ) const = 0;
	virtual void setEditorData( QWidget* editor ) = 0;

	virtual void setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) = 0;

private:
	QString description_;
};

template<typename T, bool __IsEnum>
struct ParamType {
	typedef typename boost::call_traits<T>::param_type Type;
};

template<typename T>
struct ParamType<T, true> {
	typedef T Type;
};

template<typename __ValueType>
class ValueGetterSetter {
public:
	typedef typename ParamType<__ValueType, boost::is_enum<__ValueType>::value >::Type __OrigParamType;
	typedef typename boost::remove_const<__OrigParamType>::type __ParamType;

	ValueGetterSetter() {}
	virtual ~ValueGetterSetter() {}

	virtual __ParamType getValue() const = 0;
	virtual void setValue(__ParamType v) = 0;
};

template<typename __ValueType>
class SimpleSetter : public ValueGetterSetter<__ValueType> {
public:
	typedef typename ValueGetterSetter<__ValueType>::__ParamType __ParamType;

	SimpleSetter( __ValueType* ptr ) 
		: ptr_( ptr )
	{}

	virtual __ParamType getValue() const override { return *ptr_; }
	virtual void setValue(__ParamType v) override { *ptr_ = v; }

private:
	__ValueType* ptr_;
};

template<typename __ClassType, typename __ValueType, typename __DeletePolicy>
class MethodSetter : public ValueGetterSetter<__ValueType> {
public:
	typedef typename ValueGetterSetter<__ValueType>::__ParamType __ParamType;

	typedef void(__ClassType::*__SetterType)(__ParamType);
	typedef __ParamType(__ClassType::*__GetterType)() const;

	MethodSetter( __ClassType* cls, __SetterType setter, __GetterType getter ) 
		: cls_( cls )
		, setter_( setter )
		, getter_( getter )
	{}

	virtual ~MethodSetter() {
		__DeletePolicy t;
		t(cls_);
	}

	virtual __ParamType getValue() const override { return (cls_->*getter_)(); }
	virtual void setValue(__ParamType v) override { (cls_->*setter_)(v); }

private:
	__ClassType* cls_;
	__SetterType setter_;
	__GetterType getter_;
};

struct EmptyPolicy {
	EmptyPolicy() {}

	template<typename T>
	void operator()( T* ) const {}
};

struct DeletePolicy {
	DeletePolicy() {}

	template<typename T>
	void operator()( T* t ) const {
		delete t;
	}
};

template<typename __ValueType, typename __DeletePolicy = EmptyPolicy >
class TableItem : public ITableItem {
public:

	template<typename __ClassType, typename __SetterType, typename __GetterType>
	TableItem( const QString& name, __ClassType* cls, __SetterType setter, __GetterType getter ) 
		: ITableItem( name )
		, setter_( new MethodSetter<__ClassType, __ValueType, __DeletePolicy>(cls, setter, getter) )
	{}

	template<typename __ClassType, typename __SetterType, typename __GetterType, typename T1>
	TableItem( const QString& name, __ClassType* cls, __SetterType setter, __GetterType getter, T1 t1 ) 
		: ITableItem( name )
		, setter_( new MethodSetter<__ClassType, __ValueType, __DeletePolicy>(cls, setter, getter) )
		, creator_(t1)
	{}

	template<typename __ClassType, typename __SetterType, typename __GetterType, typename T1, typename T2>
	TableItem( const QString& name, __ClassType* cls, __SetterType setter, __GetterType getter, T1 t1, T2 t2 ) 
		: ITableItem( name )
		, setter_( new MethodSetter<__ClassType, __ValueType, __DeletePolicy>(cls, setter, getter) )
		, creator_(t1, t2)
	{}

	TableItem( const QString& name, __ValueType* ptr )
		: ITableItem( name )
		, setter_( new SimpleSetter<__ValueType>(ptr) )
	{}

	template<typename T1>
	TableItem( const QString& name, __ValueType* ptr, T1 t1 )
		: ITableItem( name )
		, setter_( new SimpleSetter<__ValueType>(ptr) )
		, creator_(t1)
	{}

	template<typename T1, typename T2>
	TableItem( const QString& name, __ValueType* ptr, T1 t1, T2 t2 )
		: ITableItem( name )
		, setter_( new SimpleSetter<__ValueType>(ptr) )
		, creator_(t1, t2)
	{}

	virtual ~TableItem() { 
		delete setter_; 
		setter_ = 0; 
	}

	virtual int getType() const override{ return 0; }
	virtual QWidget* createEditor( QWidget* parent ) const override { return creator_.createEditor(parent); }
	virtual void setEditorData( QWidget* editor ) override { creator_.setEditorData(editor, setter_->getValue()); }
	virtual QString toString() const override { return creator_.toString( setter_->getValue() ); }
	virtual void setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) override { 
		setter_->setValue( creator_.getEditorData(editor) ); 
		creator_.setModelData(model, index, setter_->getValue() );
	}

private:
	typename EditorCreator<__ValueType>::CreatorType creator_;
	ValueGetterSetter<__ValueType>* setter_;
};

struct TableItems {
	int count;
	ITableItem** items;

	TableItems() : count( 0 ), items( 0 ) {}
	~TableItems();

	void resize( int size );
	void setItem( int index, ITableItem* config );
	void deleteAll();
};

#endif //_PROPERTY_TABLE_H_
