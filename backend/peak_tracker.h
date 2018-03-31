/*
    Copyright 2018 Nick Bailey <nick@n-ism.org>

    Modified to permit control via MIDI by Nick Bailey <nick@n-ism.org>
    Released as version 0.5.0 (et seq.) copyright 2013.
    
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

#ifndef PEAK_TRACKER_H
#define PEAK_TRACKER_H

#include <QtCore/QObject>

namespace JackMix {
        
/*
 * Notify other objects of the signal level at selected nodes in the backend
 * 
 * Signal levels are categorised into bands labeled with the Level enumeration.
 * These are none (virutally no signal detected), nominal (normal working level),
 * high (signal level on the high side) and too_high (very close or at clipping).
 * 
 * The backend passes the maximum value of its input and output channels
 * on a per-buffer basis. This class filters the data from each channel
 * and issues a signal containing the state of the channels where this has changed.
 * 
 * Additionally, the too_high level is made to persist; it isn't cancelled
 * for a minumum period after the high signal level causing it is removed.
 * The intention is that the corresponding GUI action, such as illuminating a
 * "clipping" indicator, is obvious even if the high signal level is very brief.
 */
class PeakTracker : public QObject {
Q_OBJECT
Q_PROPERTY(Level current_level READ level NOTIFY levelChanged)

public:
        enum Level {none, nominal, high, too_high};
        Q_ENUM(Level)
        
        PeakTracker(QObject* parent = 0);
        Level level() const;
        void  new_current_level(float max_signal);
        
signals:
        void levelChanged(Level newLevel);

private:
        Level current_level {none};
        float current_max;
        static const float threshold[];
};
        
}

#endif