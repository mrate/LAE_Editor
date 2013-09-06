#include "SelectFileWidget.h"
// qt
#include <QFileDialog>

SelectFileWidget::SelectFileWidget(QWidget *parent)
	: QWidget(parent) {
	ui_.setupUi(this);
}

SelectFileWidget::~SelectFileWidget() {

}

void SelectFileWidget::onBrowseButtonClicked() {
	QString fileName = QFileDialog::getOpenFileName(this, ui_.fileNameBox->text(), filter_);
	if( !fileName.isNull() ) {
		ui_.fileNameBox->setText(fileName);
		Q_EMIT fileSelected();
	}
}

bool SelectFileWidget::isEditable() const {
	return !ui_.fileNameBox->isReadOnly();
}

void SelectFileWidget::setEditable( bool editable ) {
	ui_.fileNameBox->setReadOnly(!editable);
}

QString SelectFileWidget::getFileName() const {
	return ui_.fileNameBox->text();
}

void SelectFileWidget::setFileName( const QString& fileName ) {
	ui_.fileNameBox->setText(fileName);
}
