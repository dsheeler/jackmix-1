
#ifndef JACK_CHANNELWIDGET_H
#define JACK_CHANNELWIDGET_H

#include <qframe.h>
#include <qstring.h>

namespace JackMix {

class ChannelWidget : public QFrame {
Q_OBJECT
public:
	ChannelWidget( QString name, QWidget* =0, const char* =0 );
	~ChannelWidget();
private slots:
	void valueChanged( QString, float );
private:
	QString _name;
};

};

#endif
