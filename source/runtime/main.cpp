#include "../main/MainWindow.h"
// Qt
#include <QtGui/QApplication>
#include <QFile>
#include <QTextStream>

int main( int argc, char* argv[] ) {
	QApplication a( argc, argv );

	QFile file( ":/style/shade" );
	if( file.open( QIODevice::ReadOnly ) ) {
		QTextStream str( &file );
		a.setStyleSheet( str.readAll() );
		file.close();
	}

	MainWindow w;
	w.show();

	return a.exec();
}
