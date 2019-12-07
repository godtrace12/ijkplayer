package tv.danmaku.ijk.media.example.util;

import android.graphics.Bitmap;
import android.os.Environment;
import android.text.TextUtils;
import android.util.Log;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Calendar;

public class FileUtil {
    private static final DateFormat detailFormatter = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss"); //显示每段录像的起止时间

    public static String generateCaptureFilePath(){
        String desDir = Environment.getExternalStorageDirectory().getAbsolutePath()+"/amazonIjk";
        File dir = new File(desDir);
        if (!dir.exists()){
            dir.mkdirs();
        }
        String strNow = detailFormatter.format(Calendar.getInstance().getTime());
        String filePath = desDir+"/"+strNow+".jpg";
        return filePath;
    }

    public static String generateRecordFilePath(){
        String desDir = Environment.getExternalStorageDirectory().getAbsolutePath()+"/amazonIjk";
        File dir = new File(desDir);
        if (!dir.exists()){
            dir.mkdirs();
        }
        String strNow = detailFormatter.format(Calendar.getInstance().getTime());
        String filePath = desDir+"/"+strNow+".mp4";
        return filePath;
    }


    public static void saveCapturePictrue(String filePath, Bitmap bitmap) {
        FileOutputStream out = null;
        try {
            // 保存原图

            if (!TextUtils.isEmpty(filePath)) {
                File file = new File(filePath);
                out = new FileOutputStream(file);
                bitmap.compress(Bitmap.CompressFormat.JPEG, 100, out);
                //out.write(tempBuf, 0, size);
                out.flush();
                out.close();
                out = null;
            }

            // 保存缩略图
//            if (!TextUtils.isEmpty(thumbnailFilePath)) {
//                File file = new File(thumbnailFilePath);
//                thumbnailOut = new FileOutputStream(file);
//                boolean decodeRet = decodeThumbnail(bitmap, bitmap.getWidth(), bitmap.getHeight(), thumbnailOut);
//                thumbnailOut.flush();
//                thumbnailOut.close();
//                thumbnailOut = null;
//                if (!decodeRet) {
//                    Log.d("jiayy", "exception 000");
//                    throw new InnerException("decode thumbnail picture fail");
//                }
//            }

        } catch (FileNotFoundException e) {
            Log.e("jiayy", "exception 111");
        } catch (IOException e) {
            Log.e("jiayy", "exception 222");
        } finally {
            if (out != null) {
                try {
                    out.close();
                } catch (IOException e) {
                    // TODO Auto-generated catch block
                    Log.d("jiayy", "exception 333");
                    e.printStackTrace();
                }
            }
        }
    }



}
