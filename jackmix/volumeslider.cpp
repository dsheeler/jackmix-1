
#include "volumeslider.h"
#include "volumeslider.moc"

#include <iostream>
#include <qlayout.h>
#include <qlabel.h>
#include "qfloatslider.h"

using namespace JackMix;

VolumeSlider::VolumeSlider( QString name, float value, Qt::Orientation o, QWidget* p, const char* n )
 : QFrame( p,n )
 , dB2VolCalc( -36, 12 )
 , _name( name )
{
	QBoxLayout *_layout = new QBoxLayout( this, QBoxLayout::TopToBottom/*, 5, 5*/ );
	QFloatSlider *tmp = new QFloatSlider(  value,dbmin,dbmax,0.1,1000,o,this );
	connect( tmp, SIGNAL( valueChanged( float ) ), this, SLOT( iValueChanged( float ) ) );
	_layout->addWidget( new QLabel( _name, this ),-10,Qt::AlignCenter );
	_layout->addWidget( tmp,100 );
}
VolumeSlider::~VolumeSlider() {
}

void VolumeSlider::iValueChanged( float n ) {
	//std::cerr << "VolumeSlider::iValueChanged( " << n << ")" << std::endl;
	//std::cerr << "That is an ampfactor of: " << dbtoamp( n ) << std::endl;
	emit valueChanged( _name, dbtoamp( n ) );
}
