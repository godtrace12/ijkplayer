package tv.danmaku.ijk.media.example.activities;

import android.graphics.Bitmap;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.SpannableString;
import android.text.SpannableStringBuilder;
import android.text.TextUtils;
import android.text.style.ClickableSpan;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;


import java.io.IOException;

import tv.danmaku.ijk.media.example.R;
import tv.danmaku.ijk.media.example.util.FileUtil;
import tv.danmaku.ijk.media.player.IMediaPlayer;
import tv.danmaku.ijk.media.player.IjkMediaPlayer;

public class IjkPlayActivity extends AppCompatActivity implements SurfaceHolder.Callback, View.OnClickListener {

    private static final String TAG = "VideoPlayActivity";
    SurfaceView svPlayBackWindow;
    Button btnPlay;
    Button btnSoundOff;
    Button btnSoundOn;
    Button btnCapture;
    Button btnRecordStart;
    Button btnRecordStop;
    Button btnSpeed2;
    TextView tvConfirm;
    /** 由ijkplayer提供，用于播放视频，需要给他传入一个surfaceView */
    private IMediaPlayer mMediaPlayer = null;
//        private String playUrl = "http://vfx.mtime.cn/Video/2019/02/04/mp4/190204084208765161.mp4";
//    private String playUrl = "https://v-cdn.zjol.com.cn/277004.mp4";
//    private String playUrl = "https://v-cdn.zjol.com.cn/276994.mp4";//2分多钟
//    private String playUrl = "http://clips.vorwaerts-gmbh.de/big_buck_bunny.mp4";
//    private String playUrl = "http://www.w3school.com.cn/example/html5/mov_bbb.mp4";
    private String playUrl = "http://vfx.mtime.cn/Video/2019/03/14/mp4/190314223540373995.mp4";
//    private String playUrl = "file:///storage/emulated/0/HikvisionMobile/Album/1069/20191115/20191115135232516_155_Q01030A23060000CU9Q2.mp4";
    /** 停止播放状态 */
    private final int STATUS_STOP = 0;
    /** 正在播放状态 */
    private final int STATUS_PLAYING = 1;
    /** 暂停播放状态 */
    private final int STATUS_PAUSE = 2;
    private int mCurrentPlaybackStatus = STATUS_STOP;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ijk_play);
        findViews();
    }

    private void findViews() {
        svPlayBackWindow = (SurfaceView) findViewById(R.id.svPlayBackWindow);
        btnPlay = (Button) findViewById(R.id.btnPlay);
        btnPlay.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startPlay();
            }
        });
        btnSoundOff = (Button) findViewById(R.id.btnSoundOff);
        btnSoundOn = (Button) findViewById(R.id.btnSoundOn);
        btnCapture =(Button) findViewById(R.id.btnCapture);
        btnRecordStart = (Button) findViewById(R.id.btnRecordStart);
        btnRecordStop = (Button) findViewById(R.id.btnRecordStop);
        btnSpeed2 = (Button) findViewById(R.id.btnSpeed2);
        tvConfirm = (TextView) findViewById(R.id.tvConfirm);
        btnCapture.setOnClickListener(this);
        btnRecordStart.setOnClickListener(this);
        btnRecordStop.setOnClickListener(this);
        btnSpeed2.setOnClickListener(this);
        btnSoundOff.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mMediaPlayer.setVolume(0,0);
            }
        });
        btnSoundOn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mMediaPlayer.setVolume(1,1);
            }
        });
        svPlayBackWindow.getHolder().addCallback(this);
//        proxyCacheServer = new HttpProxyCacheServer(this);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (mMediaPlayer != null) {
            mMediaPlayer.stop();
            mMediaPlayer.setDisplay(null);
            mMediaPlayer.release();
            mMediaPlayer = null;
        }
    }

    /**
     *【说明】：每次播放新文件，都要重新创建播放器
     *@author daijun
     *@date 2018/3/6 16:29
     *@param
     *@return
     */
    private void createPlayer() {
        if (mMediaPlayer != null) {
            mMediaPlayer.stop();
            mMediaPlayer.setDisplay(null);
            mMediaPlayer.release();
        }
        IjkMediaPlayer ijkMediaPlayer = new IjkMediaPlayer();
        //mediacodec 0-软解 1-硬解
        ijkMediaPlayer.setOption(IjkMediaPlayer.OPT_CATEGORY_PLAYER, "mediacodec", 0);
        //解决倍速时的声音音调问题
        ijkMediaPlayer.setOption(IjkMediaPlayer.OPT_CATEGORY_PLAYER, "soundtouch", 1);
        //音视频不同步问题
        ijkMediaPlayer.setOption(IjkMediaPlayer.OPT_CATEGORY_PLAYER, "framedrop", 5);
        mMediaPlayer = ijkMediaPlayer;
        mMediaPlayer.setOnPreparedListener(new IMediaPlayer.OnPreparedListener() {
            @Override
            public void onPrepared(IMediaPlayer iMediaPlayer) {
                Log.e(TAG, "onPrepared:  duration="+mMediaPlayer.getDuration());
            }
        });
        mMediaPlayer.setDisplay(svPlayBackWindow.getHolder());
        mMediaPlayer.setOnInfoListener(new IMediaPlayer.OnInfoListener() {
            @Override
            public boolean onInfo(IMediaPlayer iMediaPlayer, int i, int i1) {
                return false;
            }
        });
        mMediaPlayer.setOnErrorListener(new IMediaPlayer.OnErrorListener() {
            @Override
            public boolean onError(IMediaPlayer iMediaPlayer, int i, int i1) {
                Log.e(TAG, "onError: ");
                return false;
            }
        });
        mMediaPlayer.setOnCompletionListener(new IMediaPlayer.OnCompletionListener() {
            @Override
            public void onCompletion(IMediaPlayer iMediaPlayer) {
                Log.e(TAG, "onCompletion: 播放完了");
                mCurrentPlaybackStatus = STATUS_STOP;
            }
        });
    }

    public void startPlay(){
        if (mMediaPlayer != null) {
            try {
//                mMediaPlayer.reset();
                String amazons3 = "https://hxecloud.eos-beijing-2.cmecloud.cn/YKIX10B7IA9WQ0I7CEI7/3YUB0230843SKB7/2019/11/04/TYY20191104161606_161636.mp4?AWSAccessKeyId=YKIX10B7IA9WQ0I7CEI7&Expires=1572941658&Signature=TfHWWZvKnpHDIm1DV8XQX%2FwBbh0%3D";
//                String amazonwuhan = "http://113.57.175.178:8161/hxecloud/TYY20190612170736_170804.mp4?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Date=20191104T060554Z&X-Amz-SignedHeaders=host&X-Amz-Expires=86399&X-Amz-Credential=HIKzNT7037Ub6UC4V2990M84ZT3Arjux%2F20191104%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Signature=e22e5a0261d92b0ca97bef6f4406fde15539619bf6b74aac3d3aa2ecec5c4b98\n" +
//                        "\t";
                String amazonwuhan1105 = "http://113.57.175.178:8161/hxecloud/TYY20190612170736_170804.mp4?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Date=20191105T082557Z&X-Amz-SignedHeaders=host&X-Amz-Expires=86399&X-Amz-Credential=HIKzNT7037Ub6UC4V2990M84ZT3Arjux%2F20191105%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Signature=007efcdac0c8354e23d610f81656e1240a3c24e87e33008fed7cfcde3266eb07";
                String amazonDecodeUrl = "http://113.57.175.178:8161/hxecloud/TYY20190612170736_170804.mp4?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Date=20191106T030514Z&X-Amz-SignedHeaders=host&X-Amz-Expires=86399&X-Amz-Credential=HIKzNT7037Ub6UC4V2990M84ZT3Arjux/20191106/us-east-1/s3/aws4_request&X-Amz-Signature=f9e9cff124966614021b6da7f212d15887a6fcd1c1142e471b0ffbdc5cebe13c";
                String amazonDecodeUrl11 = "http://113.57.175.178:8161/hxecloud/HIKzNT7037Ub6UC4V2990M84ZT3Arjux_Q01030A2101005CLBBJM_2019_11_14_20191114162417_20191114162500.mp4?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=HIKzNT7037Ub6UC4V2990M84ZT3Arjux%2F20191115%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Date=20191115T015855Z&X-Amz-Expires=10000&X-Amz-SignedHeaders=host&X-Amz-Signature=9a1f03c7298a23edc13b634c828afc8c30be09231472b8f4b0bf2c20b9665ae2";
//                String cacheUrl = proxyCacheServer.getProxyUrl(playUrl);
////                cacheUrl = "";
//                Log.e(TAG, "startPlay: cacheUrl="+cacheUrl);
//                // 1、正在下载中的url地址是本地http://127.0.0.1 ···  2、已经下载好的地址是本地文件地址 =file:///storage/emulated/0/Android/data/nativ.hikan.hk.dj.amazondemo/cache/video-cache/8d13094640b9d1bc26969b9985d92dde.mp4
//                // 可以根据地址前缀来判断文件的下载情况，是否选择等文件完全下载完毕后再进行播放
//                if (!TextUtils.isEmpty(cacheUrl)){
//                    Log.e(TAG, "startPlay: play cacheUrl");
//                    mMediaPlayer.setDataSource(cacheUrl);
//                }else{
                    Log.e(TAG, "startPlay: play original url=");
                    mMediaPlayer.setDataSource(playUrl);
//                }
            } catch (IOException e) {
                Log.e(TAG, "start: 视频文件路径有问题");
                e.printStackTrace();
            }
            Log.e(TAG, "start: 准备prepare");
            mMediaPlayer.prepareAsync();
            Log.e(TAG, "start: 准备好prepare");
            mMediaPlayer.start();
        }
    }


    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        createPlayer();
        if (mMediaPlayer != null) {
            mMediaPlayer.setDisplay(holder);
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }

    @Override
    public void onClick(View view) {
        int viewId = view.getId();
        switch (viewId){
            case R.id.btnCapture:
//                Bitmap bitmap = Bitmap.createBitmap(mMediaPlayer.getVideoWidth(),mMediaPlayer.getVideoHeight(), Bitmap.Config.ARGB_8888);
//                boolean result = mMediaPlayer.doCapture(bitmap);
//                String filePath = FileUtil.generateCaptureFilePath();
//                Log.e(TAG, "onClick: filePath="+filePath);
//                if (result){
//                    Log.e(TAG, "onClick: 截图成功，开始进行保存");
//                    FileUtil.saveCapturePictrue(filePath,bitmap);
//                }else{
//                    Log.e(TAG, "onClick: 截图失败");
//                }
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        capture();
                    }
                }).run();
                break;
            case R.id.btnRecordStart:
//                String strRecordPath = FileUtil.generateRecordFilePath();
//                mMediaPlayer.doStartRecord(strRecordPath);
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        startRecord();
                    }
                }).run();
                break;
            case R.id.btnRecordStop:
//                mMediaPlayer.doStopRecord();
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        stopRecord();
                    }
                }).run();
                break;
            case R.id.btnSpeed2:
                ((IjkMediaPlayer)mMediaPlayer).setSpeed(2);
                break;
        }
    }

    private void capture(){
        Log.e(TAG, "capture: videoWidth="+mMediaPlayer.getVideoWidth()+" videoHeight="+mMediaPlayer.getVideoHeight());
        Bitmap bitmap = Bitmap.createBitmap(mMediaPlayer.getVideoWidth(),mMediaPlayer.getVideoHeight(), Bitmap.Config.ARGB_8888);
        boolean result = mMediaPlayer.doCapture(bitmap);
        String filePath = FileUtil.generateCaptureFilePath();
        Log.e(TAG, "onClick: filePath="+filePath);
        if (result){
            Log.e(TAG, "onClick: 截图成功，开始进行保存");
            FileUtil.saveCapturePictrue(filePath,bitmap);
        }else{
            Log.e(TAG, "onClick: 截图失败");
        }
    }

    private void startRecord(){
        Log.e(TAG, "startRecord: ");
        String strRecordPath = FileUtil.generateRecordFilePath();
        Log.e(TAG, "startRecord: recordPath="+strRecordPath);
//        mMediaPlayer.doStartRecord(strRecordPath);
    }

    private void stopRecord(){
        Log.e(TAG, "stopRecord: ");
//        mMediaPlayer.doStopRecord();
    }




}
