/*
    Copyright ( C ) 2004 Arnold Krille <arnold@arnoldarts.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation;
    version 2 of the License.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

*/

#ifndef JACKMIX_VOLUMEGROUP_H
#define JACKMIX_VOLUMEGROUP_H

#include <qframe.h>
#include <qvaluevector.h>

namespace JackMix {

class VolumeGroup;
class VolumeGroupMasterWidget;
class VolumeGroupChannelWidget;

class VolumeGroupFactory {
private:
	VolumeGroupFactory();
	~VolumeGroupFactory();
	QValueVector <VolumeGroup*> _groups;
public:
	static VolumeGroupFactory* the();
	void registerGroup( VolumeGroup* );
	int groups() { return _groups.size(); }
	VolumeGroup* group( int n ) { return _groups[ n ]; }
};

class VolumeGroup : public QObject {
Q_OBJECT
public:
	/**
		\param QString name of the group
		\param int number of channels
	*/
	VolumeGroup( QString, int, QObject* =0, const char* =0 );
	virtual ~VolumeGroup();

	/// returns the masterwidget. The argument is used the first time for parenting the widget.
	virtual VolumeGroupMasterWidget* masterWidget( QWidget* ) =0;
	/**
		returns a new channelwidget.
		\param the name of the input channel
		\param The parent for the widget
	*/
	virtual VolumeGroupChannelWidget* channelWidget( QString, QWidget* ) =0;

	int channels();
	QString name() { return _name; }
private:
	QString _name;
	int _channels;
protected:
	VolumeGroupMasterWidget* _masterwidget;
};

class VolumeGroupMasterWidget : public QFrame {
Q_OBJECT
public:
	VolumeGroupMasterWidget( VolumeGroup*, QWidget* =0, const char* =0 );
	~VolumeGroupMasterWidget();
protected:
	VolumeGroup* _group;
};

class VolumeGroupChannelWidget : public QFrame {
Q_OBJECT
public:
	VolumeGroupChannelWidget( QString, VolumeGroup*, QWidget*, const char* =0 );
	virtual ~VolumeGroupChannelWidget();
	QString groupname() { return _group->name(); }
protected:
	QString _in;
	VolumeGroup* _group;
};

};

#endif

