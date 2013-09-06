#ifndef SELECTFILEWIDGET_H
#define SELECTFILEWIDGET_H

#include <QWidget>
#include "ui_SelectFileWidget.h"

class SelectFileWidget : public QWidget {
	Q_OBJECT
public:
	SelectFileWidget(QWidget *parent = 0);
	~SelectFileWidget();

	bool isEditable() const;
	void setEditable(bool editable);

	QString getFileName() const;
	void setFileName( const QString& fileName );

	const QString& getFilter() const { return filter_; }
	void setFilter( const QString& filter ) { filter_ = filter; }

Q_SIGNALS:
	void fileSelected();

private:
	Ui::SelectFileWidget ui_;
	QString filter_;

private Q_SLOTS:
	void onBrowseButtonClicked();
};

#endif // SELECTFILEWIDGET_H
