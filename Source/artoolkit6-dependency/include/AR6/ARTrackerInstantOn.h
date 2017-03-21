/*
 *  ARTrackerInstantOn.h
 *  ARToolKit6
 *
 *  Tracker implementations for InstantOn tracker.
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
 *  Copyright 2013-2016 Daqri, LLC.
 *
 *  Author(s): Philip Lamb, Dan Bell, Nalin Senthamil.
 *
 */


#ifndef __AR6_ARTrackerInstantOn_h__
#define __AR6_ARTrackerInstantOn_h__

#include <AR6/AR/config.h>
#if USE_INSTANTON

#include <AR6/ARTracker.h>
#include <AR6/InstantOn/extensions/Vision4d-InstantOn.h>
#include <artvision/cv/geometry/radial_lens_model.h>

/**
 * Implements InstantOn tracker.
 *
 * In user facing messages about events related to this tracker, this tracker
 * should be referred to as the "InstantOn tracker".
 */
class ARTrackerInstantOn : public ARTracker
{

private:
#pragma mark Private types and instance variables
    // ------------------------------------------------------------------------------
    // Private types and instance variables.
    // ------------------------------------------------------------------------------
    
    // InstantOn tracking.
    std::shared_ptr<artvision_instanton::facade::InstantOnTracker> m_Tracker;
    std::shared_ptr<artvision::RadialLensModel<float> > m_2DTrackerLensModel;
    bool m_running;
    bool m_isStereo;
    ARdouble m_transL2R[3][4];

#pragma mark Private methods.
    // ------------------------------------------------------------------------------
    // Private methods.
    // ------------------------------------------------------------------------------
    
public:
#pragma mark Public API
    // ------------------------------------------------------------------------------
    // Public API
    // ------------------------------------------------------------------------------
    
    /**
     * Constructor.
     */
    ARTrackerInstantOn();
    
    /**
     * Destructor. Must be virtual.
     */
    virtual ~ARTrackerInstantOn();
    
    /**
     * Initialise so trackables can be added and removed.
     * After this call, trackables can be added and removed, any time up until final() is called.
     * @return       true if initialisation was OK, false if an error occured.
     */
    virtual bool init() override;
    
    virtual bool startWithCameraParameters(ARParamLT *paramLT, AR_PIXEL_FORMAT pixelFormat) override;
    
    virtual bool startWithStereoCameraParameters(ARParamLT *paramLT0, AR_PIXEL_FORMAT pixelFormat0, ARParamLT *paramLT1, AR_PIXEL_FORMAT pixelFormat1, const ARdouble transL2R[3][4]) override;
    
    virtual bool isRunning() override;
    
    virtual bool update(AR2VideoBufferT *buff0, AR2VideoBufferT *buff1, std::vector<ARTrackable *>& trackables) override;
    
    virtual bool stop() override;
    
    virtual bool final() override;

    virtual ARTrackable *newWithConfig(std::vector<std::string> config) override;

};

#endif // USE_INSTANTON

#endif // !__AR6_ARTrackerInstantOn_h__
