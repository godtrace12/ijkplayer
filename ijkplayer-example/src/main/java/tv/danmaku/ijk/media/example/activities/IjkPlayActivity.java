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
import android.widget.Toast;


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

//    private String playUrl = "file:///storage/emulated/0/amazonIjk/2019-12-11 18:57:10.mp4";//

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
                Log.e(TAG, "startPlay: play original url=");
                mMediaPlayer.setDataSource(playUrl);
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
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        capture();
                    }
                }).run();
                break;
            case R.id.btnRecordStart:
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
        boolean result = mMediaPlayer.doStartRecord(strRecordPath);
        if (!result){
            Log.e(TAG, "startRecord: 开启录制失败，正在编码中");
            Toast.makeText(IjkPlayActivity.this,"开启录制失败，正在编码中",Toast.LENGTH_SHORT).show();
        }else{
            Log.e(TAG, "startRecord: 录制开启成功");
        }
    }

    private void stopRecord(){
        Log.e(TAG, "stopRecord: ");
        mMediaPlayer.doStopRecord();
    }




}
