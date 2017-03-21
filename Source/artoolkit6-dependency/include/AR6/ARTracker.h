/*
 *  ARTracker.h
 *  ARToolKit6
 *
 *  Base class for tracker implementations.
 *
 *  This file is part of ARToolKit.
 *
 *  ARToolKit is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ARToolKit is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with ARToolKit.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  As a special exception, the copyright holders of this library give you
 *  permission to link this library with independent modules to produce an
 *  executable, regardless of the license terms of these independent modules, and to
 *  copy and distribute the resulting executable under terms of your choice,
 *  provided that you also meet, for each linked independent module, the terms and
 *  conditions of the license of that module. An independent module is a module
 *  which is neither derived from nor based on this library. If you modify this
 *  library, you may extend this exception to your version of the library, but you
 *  are not obligated to do so. If you do not wish to do so, delete this exception
 *  statement from your version.
 *
 *  Copyright 2015-2016 Daqri, LLC.
 *  Copyright 2010-2015 ARToolworks, Inc.
 *
 *  Author(s): Philip Lamb, Julian Looser.
 *
 */


#ifndef __AR6_ARTracker_h__
#define __AR6_ARTracker_h__

#include <AR6/Platform.h>
#include <AR6/AR/ar.h>
#include <AR6/ARTrackable.h>

#include <vector>

/**
 * Abstract tracker functionality, providing methods for tracker initialisation, updates,
 * and cleanup.
 */
class ARTracker
{

public:
    
    /**
     * Initialise so trackables can be added and removed.
     * After this call, trackables can be added and removed, any time up until final() is called.
     * @return       true if initialisation was OK, false if an error occured.
     */
    virtual bool init() = 0;
    
    virtual bool startWithCameraParameters(ARParamLT *paramLT, AR_PIXEL_FORMAT pixelFormat) = 0;
    
    virtual bool startWithStereoCameraParameters(ARParamLT *paramLT0, AR_PIXEL_FORMAT pixelFormat0, ARParamLT *paramLT1, AR_PIXEL_FORMAT pixelFormat1, const ARdouble transL2R[3][4]) = 0;
    
    virtual bool isRunning() = 0;
    
    virtual bool update(AR2VideoBufferT *buff0, AR2VideoBufferT *buff1, std::vector<ARTrackable *>& trackables) = 0;
    
    virtual bool stop() = 0;
    
    virtual bool final() = 0;

    /**
     * ARTrackable factory method.
     */
    virtual ARTrackable *newWithConfig(std::vector<std::string> config) = 0;
};


#endif // !__AR6_ARTracker_h__
