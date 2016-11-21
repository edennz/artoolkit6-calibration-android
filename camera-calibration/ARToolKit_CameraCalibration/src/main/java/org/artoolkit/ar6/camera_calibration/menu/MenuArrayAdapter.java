package org.artoolkit.ar6.camera_calibration.menu;

import android.app.Activity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import org.artoolkit.ar6.camera_calibration.CameraCalibrationActivity;
import org.opencv.ar6.camera_calibration.R;

import java.util.ArrayList;

/**
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
 *  Created by Thorsten Bux on 8/08/16.
 */
public class MenuArrayAdapter extends ArrayAdapter<String>{

    private final int mMenuResource;

    public MenuArrayAdapter(CameraCalibrationActivity cameraCalibrationActivity, ArrayList<String> strings) {
        super(cameraCalibrationActivity,R.layout.menu_list_item,strings);
        mMenuResource = R.layout.menu_list_item;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {

        View view = LayoutInflater.from(parent.getContext()).inflate(mMenuResource, parent, false);
        ImageView imageView = (ImageView) view.findViewById(R.id.menuItem_Image);
        TextView textView = (TextView) view.findViewById(R.id.menuItem_Text);

        int artImage;

        switch (position) {
            case CameraCalibrationActivity.COMPARE_MENU_POS:
                artImage = R.drawable.ic_compare_black_24px;
                textView.setText(R.string.comparison);
                break;
            case CameraCalibrationActivity.SETTINGS_MENU_POS:
                artImage = R.drawable.ic_tune_black_24px;
                textView.setText(R.string.menuItem_settings);
                break;
            case CameraCalibrationActivity.UNDISTORETED_VIDEO_POS:
                artImage = R.drawable.ic_photo_filter_black_24px;
                textView.setText(R.string.undistorted);
                break;
            case CameraCalibrationActivity.NEW_CALIBRATION:
                artImage = R.drawable.ic_transform_black_24px;
                textView.setText(R.string.calibration);
                break;
            case CameraCalibrationActivity.SHARE_POS:
                artImage = R.drawable.ic_share_black_24px;
                textView.setText(R.string.share_calibration);
                break;
            case CameraCalibrationActivity.HELP_POS:
                artImage = R.drawable.ic_help_outline_black_24px;
                textView.setText(R.string.help);
                break;
            case CameraCalibrationActivity.CALIB_MESSAGE_POS:
                String message = this.getItem(position);
                view = LayoutInflater.from(parent.getContext()).inflate(R.layout.menu_list_item_calib_result, parent, false);
                textView = (TextView) view.findViewById(R.id.textView_calibrationResult);
                textView.setText(message);
                artImage = -1;
                break;
//            case CameraCalibrationActivity.PRINT_POS:
//                artImage = R.drawable.ic_cloud_print;
//                textView.setText(R.string.prefCategory_title_printing);
//                break;
            case CameraCalibrationActivity.CALIB_STATS:
                textView.setText(R.string.calib_stats);
                artImage = R.drawable.ic_chart_black_24px;
                break;
            default:
                return null;
        }
        if(artImage >0)
            imageView.setImageResource(artImage);
        return view;
    }
}
