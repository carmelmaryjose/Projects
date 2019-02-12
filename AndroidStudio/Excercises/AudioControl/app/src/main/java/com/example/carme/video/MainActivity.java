package com.example.carme.video;

import android.content.Context;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.MediaController;
import android.widget.SeekBar;
import android.widget.VideoView;

import java.util.Timer;
import java.util.TimerTask;

public class MainActivity extends AppCompatActivity {
    MediaPlayer audioPlay;
    int currentPos;
    public void play(View view){
        if(currentPos == 0)
        {

            audioPlay.start();
        }

        else
        {
            audioPlay.seekTo(currentPos);
            audioPlay.start();
        }
    }
    public void pause(View view){
        audioPlay.pause();
        currentPos=audioPlay.getCurrentPosition();
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        audioPlay=MediaPlayer.create(this,R.raw.bird);
        final SeekBar audioControl = findViewById(R.id.audioControl);
        final AudioManager audioManager = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
        int maxVolume = audioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
        int currVolume = audioManager.getStreamVolume(AudioManager.STREAM_MUSIC);
        audioControl.setProgress(currVolume);
        audioControl.setMax(maxVolume);
        audioControl.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                audioManager.setStreamVolume(AudioManager.STREAM_MUSIC,progress,0);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        final SeekBar audioFile = findViewById(R.id.audioFile);
        audioFile.setMax(audioPlay.getDuration());
        new Timer().scheduleAtFixedRate(new TimerTask() {
            @Override
            public void run() {
                audioFile.setProgress(audioPlay.getCurrentPosition());
            }
        },0,100);
        audioFile.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                audioPlay.seekTo(progress);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                audioPlay.pause();

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                audioPlay.start();
            }
        });
    }
}
