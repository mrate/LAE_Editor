#ifndef _OBJECT_IDENTIFIER_H_
#define _OBJECT_IDENTIFIER_H_

struct ObjectIdentifier {
	enum EObjectType {
	    NONE,
	    ZPLANE,
	    ITEM,
	    BOX,
	    SCALE,
	    LIGHT
	};

	EObjectType		type;
	int				index;
	std::string		strIndex;

	ObjectIdentifier() : type( NONE ), index( 0 ) {}
	ObjectIdentifier( EObjectType type, int index ) : type( type ), index( index ) {}
	ObjectIdentifier( EObjectType type, const std::string& str ) : type( type ), index( 0 ), strIndex( str ) {}
};

#endif //_OBJECT_IDENTIFIER_H_
