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

#include "volumegroup.h"
#include "volumegroup.moc"

#include "jack_backend.h"

#include <iostream>
#include <qlabel.h>
#include <qlayout.h>

using namespace JackMix;

VolumeGroupFactory::VolumeGroupFactory() {
}
VolumeGroupFactory::~VolumeGroupFactory() {
}
VolumeGroupFactory* VolumeGroupFactory::the() {
	static VolumeGroupFactory* factory = new VolumeGroupFactory();
	return factory;
}
void VolumeGroupFactory::registerGroup( VolumeGroup* n ) {
	_groups.push_back( n );
}

VolumeGroup::VolumeGroup( QString name, int channels, QObject* p, const char* n )
 : QObject( p,n )
 , _name( name )
 , _channels( channels )
 , _masterwidget( 0 )
{
	VolumeGroupFactory::the()->registerGroup( this );
}
VolumeGroup::~VolumeGroup() {
}

/*VolumeGroupMasterWidget* VolumeGroup::masterWidget( QWidget* parent ) {
	if ( !_masterwidget )
		_masterwidget = new VolumeGroupMasterWidget( this, parent );
	return _masterwidget;
}*/
/*VolumeGroupChannelWidget* VolumeGroup::channelWidget( QString name, QWidget* parent ) {
	return new VolumeGroupChannelWidget( name, this, parent );
}*/

int VolumeGroup::channels() { return _channels; }

VolumeGroupMasterWidget::VolumeGroupMasterWidget( VolumeGroup* group, QWidget* p, const char* n )
 : QFrame( p,n )
 , _group( group )
{
}
VolumeGroupMasterWidget::~VolumeGroupMasterWidget() {
}


VolumeGroupChannelWidget::VolumeGroupChannelWidget( QString in, VolumeGroup* group, QWidget* p, const char* n )
 : QFrame( p,n )
 , _in( in )
 , _group( group )
{
	setMargin( 1 );
	setLineWidth( 1 );
	setFrameStyle( QFrame::Panel|QFrame::Raised );
}
VolumeGroupChannelWidget::~VolumeGroupChannelWidget() {
}

