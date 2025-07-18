#include <stdio.h>
#include <limits.h>
#include <math.h>

#include <alsa/asoundlib.h>

#define BITS        2
#define FRAGMENT    8
#define DURATION    5.0
#define MODE        1
#define FREQ        44100
#define BUFSIZE     (int)(BITS*FREQ*DURATION*MODE)

int setupDSP(snd_pcm_t *dev, int buf_size, int format, int sampleRate, int channels);


int main(int argc, char **argv){
    snd_pcm_t *playback_handle;
    double total = DURATION, t;
    int freq = 440;
    int i, frames, count = 1;
    char *snd_dev_out = "plughw:0,0";
    short buf[BUFSIZE];

    if(snd_pcm_open(&playback_handle, snd_dev_out, SND_PCM_STREAM_PLAYBACK, 0) < 0){
        perror("Could not set output audio device");
        return -1;
    }

    if(setupDSP(playback_handle, BUFSIZE, SND_PCM_FORMAT_S16_LE, FREQ, MODE) < 0){
        perror("Could not set output audio device");
        return -1;
    }

    printf("Make Sine wave!!!\n");
    for(i=0; i<BUFSIZE; i++){
        t = (total / BUFSIZE)*i;
        buf[i] = SHRT_MAX * sin((int)(2.0*M_PI*freq*t));
    }
    frames = BUFSIZE / (MODE * BITS);

    while(count--){
        snd_pcm_prepare(playback_handle);

        snd_pcm_writei(playback_handle, buf, frames);
    }

    snd_pcm_drop(playback_handle);

    snd_pcm_close(playback_handle);

    return 0;
}

int setupDSP(snd_pcm_t *dev, int buf_size, int format, int sampleRate, int channels){
    snd_pcm_hw_params_t* hw_params;
    snd_pcm_uframes_t frames;
    int fragments = FRAGMENT;
    int bits = (format == SND_PCM_FORMAT_S16_LE)?2:1;

    // 하드웨어 매개변수 구조체 공간 마련
    if(snd_pcm_hw_params_malloc(&hw_params) < 0){
        perror("Could not allocate parameter");
        return -1;
    }

    // 
    if(snd_pcm_hw_params_any(dev, hw_params) < 0){
        perror("Could not initialize parameter");
        return -1;
    }

    if(snd_pcm_hw_params_set_access(dev, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED) < 0){
        perror("Could not set access type");
        return -1;
    }

    if(snd_pcm_hw_params_set_format(dev, hw_params, format) < 0){
        perror("Could not set sample format");
        return -1;
    }
    
    if(snd_pcm_hw_params_set_rate_near(dev, hw_params, &sampleRate, 0) < 0){
        perror("Could not set sample rate");
        return -1;
    }
    
    if(snd_pcm_hw_params_set_channels(dev, hw_params, channels) < 0){
        perror("Could not set channel count");
        return -1;
    }
    
    if(snd_pcm_hw_params_set_periods_near(dev, hw_params, &fragments, 0) < 0){
        perror("Could not set fragment");
        return -1;
    }
    
    frames = (buf_size * fragments) / (channels * bits);
    if(snd_pcm_hw_params_set_buffer_size_near(dev, hw_params, &frames) < 0){
        perror("Could not set buffer_size");
        return -1;
    }

    buf_size = frames * channels * bits / fragments;

    if(snd_pcm_hw_params(dev, hw_params) < 0){
        perror("Could not set HW params");
        return -1;
    }

    return 0;
}