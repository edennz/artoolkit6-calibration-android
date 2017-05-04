/*
 *  Error.h
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
 *  Copyright 2014-2015 ARToolworks, Inc.
 *
 *  Author(s): Philip Lamb
 *
 */

#ifndef __AR6_Error_h__
#define __AR6_Error_h__

/**
 * \file Error.h
 * Defines error codes used in the library.
 */

#ifdef __cplusplus
extern "C" {
#endif

    enum {
        ARW_ERROR_NONE                        =   0,
        ARW_ERROR_GENERIC                     =  -1,
        ARW_ERROR_OUT_OF_MEMORY               =  -2,
        ARW_ERROR_OVERFLOW                    =  -3,
        ARW_ERROR_NODATA                      =  -4,
        ARW_ERROR_IOERROR                     =  -5,
        ARW_ERROR_EOF                         =  -6,
        ARW_ERROR_TIMEOUT                     =  -7,
        ARW_ERROR_INVALID_COMMAND             =  -8,
        ARW_ERROR_INVALID_ENUM                =  -9,
        ARW_ERROR_THREADS                     = -10,
        ARW_ERROR_FILE_NOT_FOUND              = -11,
        ARW_ERROR_LENGTH_UNAVAILABLE          = -12,
        ARW_ERROR_DEVICE_UNAVAILABLE          = -13,
        ARW_ERROR_INVALID_PARAMETER           = -14,
        ARW_ERROR_2D_IMG_TRACKER_INIT         = -15,
        ARW_ERROR_SQUARE_TRACKER_INIT         = -16,
        ARW_ERROR_INSTANTON_TRACKER_INIT      = -17,
        ARW_ERROR_AR_UNINITIALISED            = -18,
        ARW_ERROR_START_VIDEO_SOURCING        = -19,
        ARW_ERROR_UPDATE_TO_RUN_MARKER_DETECT = -20,
        ARW_ERROR_VIDEO_FRAME_CAPTURE_ERROR   = -21
    };

#ifdef __cplusplus
}
#endif
#endif // !__AR6_Error_h__
