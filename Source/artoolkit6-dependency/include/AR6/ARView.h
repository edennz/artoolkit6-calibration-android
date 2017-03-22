/*
 *  ARView.h
 *  ARToolKit6
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
 *  Author(s): Philip Lamb
 *
 */

#ifndef __AR6_ARView_h__
#define __AR6_ARView_h__

#include <AR6/Platform.h>
#include <AR6/ARVideoSource.h>

#include <AR6/AR/ar.h>
#include <AR6/ARG/arg.h>

/**
 * An ARView provides a rendering target for visual overlay. Typically, this is used in
 * video see-through augmented reality.
 *
 * All non-const functions in this class REQUIRE an rebderubg context to be active at the
 * time the function is called. E.g. on platforms supporting OpenGL, a valid OpenGL context
 * must be initialised and active before calling.
 * In multi-threaded applications, non-const functions in this class should be called from
 * a rendering thread.
 */
class ARView {

public:

    enum class HorizontalAlignment {
        H_ALIGN_LEFT,
        H_ALIGN_CENTRE,
        H_ALIGN_RIGHT
    };

    enum class VerticalAlignment {
        V_ALIGN_TOP,
        V_ALIGN_CENTRE,
        V_ALIGN_BOTTOM
    };

    enum class ScalingMode {
        SCALE_MODE_FIT,
        SCALE_MODE_FILL,
        SCALE_MODE_STRETCH,
        SCALE_MODE_1_TO_1
    };
    
    struct Size {
        int width;
        int height;
    };

    ARView();
    
    bool initWithVideoSource(const ARVideoSource& vs, const int contextWidth, const int contextHeight);
    
    bool distortionCompensation() const;
    void setDistortionCompensation(const bool distortionCompensation);
    bool rotate90() const;
    void setRotate90(const bool rotate90);
    bool flipH() const;
    void setFlipH(const bool flipH);
    bool flipV() const;
    void setFlipV(const bool flipV);
    HorizontalAlignment horizontalAlignment() const;
    void setHorizontalAlignment(const HorizontalAlignment hAlign);
    VerticalAlignment verticalAlignment() const;
    void setVerticalAlignment(const VerticalAlignment vAlign);
    ScalingMode scalingMode() const;
    void setScalingMode(const ScalingMode scaling);
    ARView::Size contextSize() const;
    void setContextSize(const Size size);
    
    void getViewport(int32_t viewport[4]) const;
    
    void draw(ARVideoSource* vs);
    
    ~ARView();
    
private:
    void update();
    ARGL_CONTEXT_SETTINGS_REF m_arglContextSettings;
    AR2VideoTimestampT m_drawTime;
    int m_contentWidth;
    int m_contentHeight;
    int m_contextWidth;
    int m_contextHeight;
    bool m_distortionCompensation;
    bool m_rotate90;
    bool m_flipH;
    bool m_flipV;
    HorizontalAlignment m_hAlign;
    VerticalAlignment m_vAlign;
    ScalingMode m_scalingMode;
    int32_t m_viewport[4];
    
};

#endif // !__AR6_ARView_h__
