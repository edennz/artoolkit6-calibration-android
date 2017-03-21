/*
 *  ARTracker2D.h
 *  ARToolKit6
 *
 *  Tracker implementations for ARTVision 2D image tracker.
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


#ifndef __AR6_ARTracker2D_h__
#define __AR6_ARTracker2D_h__

#include <AR6/ARTracker.h>
#include <artvision/cv/matchers/multi_target_planar_recogniser.h>
#include <artvision/cv/geometry/radial_lens_model.h>
#include <artvision/cv/matchers/target.h>
#include <artvision/cv/trackers/single_target_planar_tracker.h>
#include <artvision/cv/trackers/multi_target_planar_tracker.h>
#include <artvision/cv/trackers/tracked_planar_target.h>
#include <artvision/cv/trackers/projection_matrix_smoother.h>
#include <map>

/**
 * Implements ARTVision's 2D image tracker.
 *
 * In user facing messages about events related to this tracker, this tracker
 * should be referred to as the "2D image tracker".
 */
class ARTracker2D : public ARTracker
{

private:
#pragma mark Private types and instance variables
    // ------------------------------------------------------------------------------
    // Private types and instance variables.
    // ------------------------------------------------------------------------------
    
    // 2D image tracking.
    bool m_running;
    bool m_2DTrackerDataLoaded;
    std::shared_ptr<artvision::MultiTargetPlanarTracker> m_2DTracker;
    std::shared_ptr<artvision::RadialLensModel<float> > m_2DTrackerLensModel;
    std::shared_ptr<artvision::MultiTargetPlanarRecogniser> m_2DRecogniser;
    int m_2DTrackerMaxSimultaneousTrackedImages;
    int m_2DTrackerDetectedImageCount;
    std::vector<std::shared_ptr<artvision::ProjectionMatrixSmoother<float>>> m_2DTrackerSmoother;
    bool m_isStereo;
    ARdouble m_transL2R[3][4];
#pragma mark Private methods.
    // ------------------------------------------------------------------------------
    // Private methods.
    // ------------------------------------------------------------------------------
    
    bool unload2DTrackerData(void);

    bool load2DTrackerData(std::vector<ARTrackable *>& trackables, size_t width, size_t height);

public:
#pragma mark Public API
    // ------------------------------------------------------------------------------
    // Public API
    // ------------------------------------------------------------------------------
    
    /**
     * Constructor.
     */
    ARTracker2D();
    
    /**
     * Destructor. Must be virtual.
     */
    virtual ~ARTracker2D();
    
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

    void setMaxSimultaneousTrackedImages(int noOfMarkers);
    
    int getMaxSimultaneousTrackedImages() const;
    
    int getFeatureCount(int imageId);

    unsigned char* drawFeatures(ARTrackable* markerInfo, std::string fileName, std::string saveName);
    
    bool saveTrackerImageDatabase(std::string fileName);
    
    bool loadTrackerImageDatabase(std::string fileName, std::string targetImageDirectory, std::map<int, std::string> imageArchive);
};


#endif // !__AR6_ARTracker2D_h__
