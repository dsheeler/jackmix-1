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

#ifndef STEREO_ELEMENTS_H
#define STEREO_ELEMENTS_H

#include <mixingmatrix.h>
#include <dbvolcalc.h>

namespace JackMix {
namespace MixerElements {

class Mono2StereoElement;
class Stereo2StereoElement;

/**
 * A MonotoStereo control.
 */
class Mono2StereoElement : public JackMix::MixingMatrix::Element, public dB2VolCalc
{
Q_OBJECT
//Q_PROPERTY( double volume READ volume );
//Q_PROPERTY( double panorama READ panorama );
public:
	Mono2StereoElement( QStringList, QStringList, MixingMatrix::Widget*, const char* =0 );
	~Mono2StereoElement();

	int inchannels() const { return 1; }
	int outchannels() const { return 2; }

	double volume() const { return _volume_value; }
	double panorama() const { return _balance_value; }
signals:
	void volume_changed( double );
	void panorama_changed( double );
public slots:
	void set_panorama( double n ) { balance( n ); }
	void balance( float );
	void volume( float );
private slots:
	void slot_toggle();
	void slot_replace() { emit replace( this ); }
	void calculateVolumes();

	void printSignals();
private:
	QString _inchannel, _outchannel1, _outchannel2;
	float _balance_value, _volume_value;
};

/**
 * A StereotoStereo control.
 */
class Stereo2StereoElement : public JackMix::MixingMatrix::Element, public dB2VolCalc
{
Q_OBJECT
//Q_PROPERTY( double volume READ getVolume )
public:
	Stereo2StereoElement( QStringList, QStringList, MixingMatrix::Widget*, const char* =0 );
	~Stereo2StereoElement();

	int inchannels() const { return 2; }
	int outchannels() const { return 2; }

	double getVolume() const { return _volume_value; }
signals:
	void volume_changed( float );
private slots:
	void slot_toggle();
	void slot_replace() { emit replace( this ); }
	void balance( float );
	void volume( float );
	void calculateVolumes();
private:
	QString _inchannel1, _inchannel2, _outchannel1, _outchannel2;
	float _balance_value, _volume_value;
};

}; // MixerElements
}; // JackMix

#endif // STEREO_ELEMENTS_H

