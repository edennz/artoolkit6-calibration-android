package org.artoolkit.ar6.calib_camera.share;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;


import org.artoolkit.ar6.calib_camera.R;

import java.io.File;
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
 *  Created by thorstenbux on 24/08/16.
 */
class ShareCalibrationsArrayAdapter extends ArrayAdapter<File> {
    private final int mMenuResource;
    private final ArrayList<File> mCacheFiles;

    public ShareCalibrationsArrayAdapter(ShareActivity shareActivity, ArrayList<File> cacheFiles) {
        super(shareActivity,R.layout.share_list_item,cacheFiles);
        this.mMenuResource = R.layout.share_list_item;
        this.mCacheFiles = cacheFiles;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        ViewHolder viewHolder = null;

        if(convertView == null){
            viewHolder = new ViewHolder();
            convertView =  LayoutInflater.from(parent.getContext()).inflate(mMenuResource, parent, false);
            viewHolder.shareItemTextView = (TextView) convertView.findViewById(R.id.shareItem_Text);
            convertView.setTag(viewHolder);
        }
        else{
            viewHolder = (ViewHolder) convertView.getTag();
        }

        String calibFileText = mCacheFiles.get(position).getName();
        viewHolder.shareItemTextView.setText(calibFileText);
        return convertView;
    }

    private static class ViewHolder{
        TextView shareItemTextView;
    }
}
