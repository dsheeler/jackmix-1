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

#include "mixingmatrix.h"
#include "mixingmatrix_privat.h"
#include "mixingmatrix.moc"
//#include "mixingmatrix_privat.moc"
#include "connectionlister.h"

#include <QtGui/QLayout>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QMenu>
#include <QtGui/QAction>
#include <QtGui/QCursor>
#include <QtCore/QTimer>
#include <QtGui/QContextMenuEvent>
#include <QtCore/QMetaProperty>

namespace JackMix {
namespace MixingMatrix {

bool ElementSlotSignalPair::exists() const {
	return ( element->metaObject()->indexOfProperty( slot.toStdString().c_str() ) > -1 )?true:false;
}

Widget::Widget( QStringList ins, QStringList outs, JackMix::BackendInterface* backend, QWidget* p, const char* n )
	: QFrame( p )
	, _mode( Normal )
	, _direction( None )
	, _inchannels( ins )
	, _outchannels( outs )
	, _connectionlister( 0 )
	, _backend( backend )
{
	if ( _inchannels.size()==0 ) {
		_direction = Vertical;
		_inchannels = _outchannels;
	}
	if ( _outchannels.size()==0 ) {
		_direction = Horizontal;
		_outchannels = _inchannels;
	}
	setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
}
Widget::~Widget() {
	if ( _connectionlister )
		delete _connectionlister;
}


void Widget::addElement( Element* n ) {
	_elements.push_back( n );
	connect( n, SIGNAL( replace( Element* ) ), this, SLOT( replace( Element* ) ) );
	connect( n, SIGNAL( valueChanged( Element*, QString ) ), this, SLOT( valueChanged( Element*, QString ) ) );
	connect( n, SIGNAL( connectSlave( Element*, QString ) ), this, SLOT( connectSlave( Element*, QString ) ) );
	connect( n, SIGNAL( disconnectSlave( Element*, QString ) ), this, SLOT( disconnectSlave( Element*, QString ) ) );
	connect( n, SIGNAL( disconnectMaster( Element*, QString ) ), this, SLOT( disconnectMaster( Element*, QString ) ) );
	if ( _connectionlister )
		_connectionlister->addElement( n );
	resizeEvent( 0 );
}
void Widget::removeElement( Element* n ) {
	disconnectMaster( n, 0 );
	disconnectSlave( n, 0 );
	if ( _connectionlister )
		_connectionlister->removeElement( n );
	_elements.removeAll( n );
}

void Widget::replace( Element* n ) {
qDebug( "Widget::replace( Element* %p )", n );
qDebug( "This Element has %i selected neighbors.", n->neighbors() );
qDebug( " and %i selected followers.", n->followers( n->neighbors() ) );
	QStringList in, out;
	in = n->neighborsList();
	qDebug( "Selected ins = %s", in.join( "," ).toStdString().c_str() );
	out = n->followersList();
	qDebug( "Selected outs = %s", out.join( "," ).toStdString().c_str() );
	for ( QStringList::ConstIterator it=out.begin(); it!=out.end(); ++it ) {
		for ( QStringList::ConstIterator jt=in.begin(); jt!=in.end(); ++jt ) {
			Element* tmp = getResponsible( ( *jt ),( *it ) );
			qDebug( "About to delete %p", tmp );
			if ( tmp ) {
				delete tmp;
			}
		}
	}
	createControl( in, out );
	//autoFill();
}

Element* Widget::getResponsible( QString in, QString out ) const {
	for ( int i=0; i<_elements.size(); i++ )
		if ( _elements[ i ]->isResponsible( in, out ) )
			return _elements[ i ];
	return 0;
}

bool Widget::createControl( QStringList inchannels, QStringList outchannels ) {
	//qDebug( "Widget::createControl( QStringList '%s', QStringList '%s')", inchannels.join( "," ).toStdString().c_str(), outchannels.join( "," ).toStdString().c_str() );

	QString control = Global::the()->canCreate( inchannels.size(), outchannels.size() )[ 0 ];
	//qDebug( "Found %s to control [%i,%i] channels", control.toStdString().c_str(), inchannels.size(), outchannels.size() );
	return Global::the()->create( control, inchannels, outchannels, this );

	return false;
}

void Widget::autoFill() {
qDebug( "\nWidget::autoFill()" );
//	qDebug( "_direction = %i", _direction );
	if ( _direction == None ) {
//		qDebug( "Doing the Autofill-boogie..." );
		for ( QStringList::Iterator init=_inchannels.begin(); init!=_inchannels.end(); ++init )
			for ( QStringList::Iterator outit=_outchannels.begin(); outit!=_outchannels.end(); ++outit ) {
				if ( !getResponsible( *init, *outit ) ) {
//					qDebug( "...together with (%s|%s)", ( *init ).toStdString().c_str(), ( *outit ).toStdString().c_str() );
					createControl( QStringList()<<*init, QStringList()<<*outit );
				}
//				else qDebug( "   (%s|%s) is allready occupied. :(", ( *init ).toStdString().c_str(), ( *outit ).toStdString().c_str() );
			}
	} else if ( _direction == Vertical ) {
		for ( QStringList::Iterator outit=_outchannels.begin(); outit!=_outchannels.end(); ++outit )
			if ( !getResponsible( *outit, *outit ) )
				createControl( QStringList()<<*outit, QStringList()<<*outit );
	} else if ( _direction == Horizontal ) {
		for ( QStringList::Iterator init=_inchannels.begin(); init!=_inchannels.end(); ++init )
			if ( !getResponsible( *init, *init ) )
				createControl( QStringList()<<*init, QStringList()<<*init );
	}
	resizeEvent( 0 );
	qDebug( "\n" );
}

void Widget::resizeEvent( QResizeEvent* ) {
	setMinimumSize( sizeHint() );
	//qDebug( "MinimumSize = (%i,%i)", sizeHint().width(), sizeHint().height() );
	int ins = _inchannels.size(), outs = _outchannels.size();
	//qDebug( "%i InChannels and %i OutChannels", ins, outs );

	int w=1, h=1;
	if ( ins && outs ) {
		w = width()/ins;
		h = height()/outs;
	}

	if ( smallestElement().width()>w )
		w = smallestElement().width();
	if ( smallestElement().height()>h )
		h = smallestElement().height();

	if ( _direction == Horizontal )
//		h=0;
		for ( int i=0; i<_elements.size(); i++ ) {
			_elements[ i ]->setGeometry( _inchannels.indexOf( _elements[ i ]->in()[ 0 ] )*w, 0, w*_elements[ i ]->inchannels(), h );
			_elements[ i ]->show();
		}
	else if ( _direction == Vertical )
//		w=0;
		for ( int i=0; i<_elements.size(); i++ ) {
			_elements[ i ]->setGeometry( 0, _outchannels.indexOf( _elements[ i ]->out()[ 0 ] )*h, w, h*_elements[ i ]->outchannels() );
			_elements[ i ]->show();
		}
	else
		for ( int i=0; i<_elements.size(); i++ ) {
			_elements[ i ]->setGeometry( _inchannels.indexOf( _elements[ i ]->in()[ 0 ] )*w, _outchannels.indexOf( _elements[ i ]->out()[ 0 ] )*h, w*_elements[ i ]->inchannels(), h*_elements[ i ]->outchannels() );
			_elements[ i ]->show();
		}
}

QSize Widget::minimumSizeHint() const {
	//qDebug( "Widget::minimumSizeHint()" );
	QSize smallest = smallestElement();
	if ( _direction == Horizontal )
		return QSize(  smallest.width()*_inchannels.size(),smallest.height() );
	if ( _direction == Vertical )
		return QSize(  smallest.width(),smallest.height()*_outchannels.size() );
	return QSize( smallest.width()*_inchannels.size(),smallest.height()*_outchannels.size() );
}

QSize Widget::smallestElement() const {
	//qDebug( "Widget::smallestElement()" );
	int w = 1, h = 1;
	for ( int i=0; i<elements(); i++ ) {
		if ( _elements[ i ]->minimumSizeHint().width() > w )
			w = _elements[ i ]->minimumSizeHint().width();
		if ( _elements[ i ]->minimumSizeHint().height() > h )
			h = _elements[ i ]->minimumSizeHint().height();
	}
	return QSize( w,h );
}

QString Widget::nextIn( QString n ) const {
	for ( QStringList::ConstIterator it = _inchannels.begin(); it != _inchannels.end(); ++it )
		if ( ( *it ) == n ) { ++it; return ( *it ); }
	return 0;
}
QString Widget::nextOut( QString n ) const {
	for ( QStringList::ConstIterator it = _outchannels.begin(); it != _outchannels.end(); ++it )
		if ( ( *it ) == n ) { ++it; return ( *it ); }
	return 0;
}

void Widget::addinchannel( QString name ) {
	_inchannels.push_back( name );
	this->updateGeometry();
}
void Widget::addoutchannel( QString name ) {
	_outchannels.push_back( name );
	this->updateGeometry();
}
void Widget::removeinchannel( QString name ) {
//	qDebug( "Widget::removeinchannel( %s )", name.toStdString().c_str() );
	for ( QStringList::Iterator it = _outchannels.begin(); it != _outchannels.end(); it++ ) {
		Element* tmp = getResponsible( name, *it );
		if ( tmp ) {
//			qDebug( "removing element %p", tmp );
			delete tmp;
			_inchannels.removeAll( name );
		}
	}
	autoFill();
//	qDebug( "_inchannels.count = %i", _inchannels.size() );
}
void Widget::removeoutchannel( QString name ) {
//	qDebug( "Widget::removeoutchannel( %s )", name.toStdString().c_str() );
	for ( QStringList::Iterator it = _inchannels.begin(); it != _inchannels.end(); it++ ) {
		Element* tmp = getResponsible( *it, name );
		if ( tmp ) {
//			qDebug( "removing element %p", tmp );
			delete tmp;
			_outchannels.removeAll( name );
		}
	}
	autoFill();
//	qDebug( "_outchannels.count = %i", _outchannels.size() );
}

void Widget::connectSlave( Element* slave, QString slot ) {
	qDebug( "Widget::connectSlave( %p, %s ) [ ! simple ! ]", slave, slot.toStdString().c_str() );
	if ( !_elements[ 0 ]->metaObject()->indexOfProperty( slot.toStdString().c_str() ) )
		connectMasterSlave( _elements[ 0 ], slot, slave, slot );
}
void Widget::connectMaster( Element* master, QString signal ) {
	qDebug( "Widget::connectMaster( %p, %s ) [ ! defunct ! ]", master, signal.toStdString().c_str() );
}
void Widget::connectMasterSlave( Element* master, QString signal, Element* slave, QString slot ) {
	//qDebug( "Widget::connectMasterSlave( %p, %s, %p, %s )", master, signal.toStdString().c_str(), slave, slot.toStdString().c_str() );
	ElementSlotSignalPair sender( master, signal );
	ElementSlotSignalPair receiver( slave, slot );
	connectMasterSlave( sender, receiver );
}
void Widget::connectMasterSlave( ElementSlotSignalPair sender, ElementSlotSignalPair receiver ) {
	qDebug( "Widget::connectMasterSlave( %s, %s )", sender.debug().toStdString().c_str(), receiver.debug().toStdString().c_str() );
	if ( sender.exists() && receiver.exists() ) {
		if ( receiver == sender )
			qWarning( " * Tried to connect a property with itself! Cancelling this..." );
		else if ( _connections[ sender ] == receiver || _connections[ receiver ] == sender )
			qWarning( " * Feedback-loop detected! Aborting..." );
		else
			_connections.insert( receiver,sender );
	} else {
		if ( !sender.exists() )
			qWarning( " * * Sender %s doesn't exist! * *", sender.debug().toStdString().c_str() );
		if ( !receiver.exists() )
			qWarning( " * * Receiver %s doesn't exist! * *", receiver.debug().toStdString().c_str() );
	}
}

void Widget::disconnectSlave( Element* slave, QString slot ) {
	//qDebug( "Widget::disconnectSlave( %p, %s )", slave, slot.toStdString().c_str() );
	disconnectSlave( ElementSlotSignalPair( slave, slot ) );
}
void Widget::disconnectSlave( ElementSlotSignalPair slave ) {
	qDebug( "Widget::disconnectSlave( %s )", slave.debug().toStdString().c_str() );
	QMap<ElementSlotSignalPair,ElementSlotSignalPair>::Iterator it;
	for ( it = _connections.begin(); it != _connections.end(); ++it ) {
		if ( it.key() == slave ) {
			_connections.erase( it );
		}
	}
}
void Widget::disconnectMaster( Element* master, QString signal ) {
	//qDebug( "Widget::disconnectMaster( %p, %s )", master, signal.toStdString().c_str() );
	disconnectMaster( ElementSlotSignalPair( master, signal ) );
}
void Widget::disconnectMaster( ElementSlotSignalPair master ) {
	qDebug( "Widget::disconnectMaster( %s )", master.debug().toStdString().c_str() );
	QMap<ElementSlotSignalPair,ElementSlotSignalPair>::Iterator it;
	for ( it = _connections.begin(); it != _connections.end(); ++it )
		if ( it.value() == master )
			_connections.erase( it );
}

void Widget::toggleConnectionLister( bool n ) {
	if ( !_connectionlister )
		_connectionlister = new ConnectionLister( this, 0 );
	_connectionlister->setShown( n );
}
void Widget::toggleConnectionLister() {
	if ( !_connectionlister )
		_connectionlister = new ConnectionLister( this, 0 );
	_connectionlister->setVisible( !_connectionlister->isVisible() );
}

void Widget::valueChanged( Element* master, QString signal ) {
	valueChanged( ElementSlotSignalPair( master, signal ) );
}
void Widget::valueChanged( ElementSlotSignalPair value ) {
	QMap<ElementSlotSignalPair,ElementSlotSignalPair>::Iterator it;
	for ( it = _connections.begin(); it != _connections.end(); ++it )
		if ( it.value() == value ) {
			//qDebug( "Widget::valueChanged( %s ) connections=%i", value.debug().toStdString().c_str(), _connections.size() );
			//qDebug( " * Connection found!\n   Element: %p %s", ( *it ).element, ( *it ).slot.toStdString().c_str() );
			it.key().element->setProperty( it.key().slot.toStdString().c_str(), value.element->property( value.slot.toStdString().c_str() ) );
		}
}

void Widget::debugPrint() {
	qDebug( "\nWidget::debugPrint()" );
	qDebug( "  Elements:" );
	QList<Element*>::Iterator it;
	for ( it=_elements.begin(); it!=_elements.end(); ++it ) {
		qDebug( "    %p [%s]:\n      %s", ( *it ), ( *it )->metaObject()->className(), ( *it )->getPropertyList().join( "," ).toStdString().c_str() );
	}
	qDebug( "\n" );
}


Element::Element( QStringList in, QStringList out, Widget* p, const char* n )
	: QFrame( p )
	, _in( in )
	, _out( out )
	, _selected( false )
	, _parent( p )
	, _menu( new QMenu( this ) )
{
	//qDebug( "MixingMatrix::Element::Element( QStringList '%s', QStringList '%s' )", in.join(",").toStdString().c_str(), out.join(",").toStdString().c_str() );
	//setMargin( 2 );
	setFrameStyle( QFrame::Raised|QFrame::StyledPanel );
	setLineWidth( 2 );
	QTimer::singleShot( 0, this, SLOT( lazyInit() ) );
}
Element::~Element() {
	//qDebug( "MixingMatrix::Element::~Element()" );
	_parent->removeElement( this );
}
void Element::lazyInit() {
	_parent->addElement( this );
}

bool Element::isResponsible( QString in, QString out ) {
	if ( _in.indexOf( in )>-1 && _out.indexOf( out )>-1 )
		return true;
	return false;
}

void Element::select( bool n ) {
qDebug( "MixingMatrix::Element::select( bool %i )", n );
	if ( n != _selected ) {
		if ( _parent->mode() == Widget::Select ) {
			_selected = n;
/*			if ( _selected ) {
				setFrameShadow( QFrame::Sunken );
				setBackgroundColor( colorGroup().background().dark() );
			} else {
				setFrameShadow( QFrame::Raised );
				setBackgroundColor( colorGroup().background() );
			}*/
			isSelected( n );
		}
	}
}

int Element::neighbors() const {
//	qDebug( "neighbor: %s", _parent->nextIn( _in[ 0 ] ).toStdString().c_str() );
	Element* neighbor = _parent->getResponsible( _parent->nextIn( _in[ _in.size()-1 ] ), _out[ 0 ] );
	if ( neighbor && neighbor->isSelected() )
		return neighbor->neighbors()+1;
	return 0;
}
QStringList Element::neighborsList() const {
//	qDebug( "self = [%s]", _in.join( "|" ).toStdString().c_str() );
//	qDebug( "neighbor = %s", _parent->nextIn( _in[ _in.size()-1 ] ).toStdString().c_str() );
	Element* neighbor = _parent->getResponsible( _parent->nextIn( _in[ _in.size()-1 ] ), _out[ 0 ] );
	QStringList tmp;
	if ( neighbor && neighbor->isSelected() )
		tmp = neighbor->neighborsList();
	tmp += _in;
	tmp.sort();
	return tmp;
}
int Element::followers( int n ) const {
//qDebug( "Element::followers( int %i )", n );
	Element* follower = _parent->getResponsible( _in[ 0 ], _parent->nextOut( _out[ _out.size()-1 ] ) );
/*	if ( follower )
		qDebug( "follower of %p is %p which has selected=%i", this, follower, follower->isSelected() );
	else
		qDebug( "follower of %p is %p", this, follower );*/
	if ( follower && follower->isSelected() && follower->neighbors() >= n )
		return follower->followers( n )+1;
	return 0;
}
QStringList Element::followersList() const {
//	qDebug( "self = [%s]", _out.join( "|" ).toStdString().c_str() );
//	qDebug( "follower = %s", _parent->nextOut( _out[ _out.size()-1 ] ).toStdString().c_str() );
	Element* follower = _parent->getResponsible( _in[ 0 ], _parent->nextOut(  _out[  _out.size()-1 ] ) );
	QStringList tmp;
	if ( follower && follower->isSelected() )
		tmp = follower->followersList();
	tmp += _out;
	tmp.sort();
	return tmp;
}

QStringList Element::getPropertyList() {
	QStringList tmp; // = QStringList::fromStrList( metaObject()->propertyNames() );
	for ( int i=0; i<metaObject()->propertyCount(); ++i ) {
		tmp += metaObject()->property( i ).name();
	}
	return tmp;
}

void Element::showMenu() {
	_menu->exec( QCursor::pos() );
}
void Element::contextMenuEvent( QContextMenuEvent* ev ) {
	showMenu();
	ev->accept();
}

ElementFactory::ElementFactory() {
	Global::the()->registerFactory( this );
}
ElementFactory::~ElementFactory() {
	Global::the()->unregisterFactory( this );
}

void ElementFactory::globaldebug() {
	Global::the()->debug();
}

Global::Global() {}
Global::~Global() {}

Global* Global::the() {
	static Global* tmp = new Global();
	return tmp;
}

void Global::registerFactory( ElementFactory* n ) {
	//qDebug( "Global::registerFactory( ElementFactory* %p )", n );
	_factories.push_back( n );
}
void Global::unregisterFactory( ElementFactory* n ) {
	//qDebug( "Global::unregisterFactory( ElementFactory* %p )", n );
	_factories.removeAll( n );
}

QStringList Global::canCreate( int in, int out ) {
	QStringList tmp;
	for ( int i=0; i<_factories.size(); i++ ) {
		tmp += _factories[ i ]->canCreate( in, out );
	}
	return tmp;
}

bool Global::create( QString type, QStringList ins, QStringList outs, Widget* parent, const char* name ) {
	//qDebug( "Global::create( QString %s, QStringList '%s', QStringList '%s', Widget* %p, const char* %s )", type.toStdString().c_str(), ins.join( "," ).toStdString().c_str(), outs.join( "," ).toStdString().c_str(), parent, name );
	Element* elem=0;
	for ( int i=0; i<_factories.size() && elem==0; i++ ) {
		elem = _factories[ i ]->create( type, ins, outs, parent, name );
	}
//	qDebug( "Will show and return %p", elem );
	if ( elem ) {
		elem->show();
	}
	return elem;
}

void Global::debug() {
	for ( int i=0; i<_factories.size(); i++ )
		qDebug( "The factory %p can create '%s'", _factories[ i ], _factories[ i ]->canCreate().join( " " ).toStdString().c_str() );
}


}; // MixingMatrix
}; //JackMix
