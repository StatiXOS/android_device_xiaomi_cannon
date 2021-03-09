set -x
# $1: device for output
#     spk: speaker
#     rcv: receiver
#     spk_hp: speaker high power
#     us: ultrasound

# tinyplay file.wav [-D card] [-d device] [-p period_size] [-n n_periods]
# sample usage: playback.sh spk
# rcv.wav:-4.5dbfs   spk: -4.8dbfs  ultra: -4.5dbfs  spk_hp:-1.8dbfs

function enable_receiver_hp
{
    echo "enabling receiver hp"
    tinymix 'Handset_PGA_GAIN' 0
    tinymix 'ADDA_DL_CH1 DL1_CH1' 1
    tinymix 'ADDA_DL_CH2 DL1_CH2' 1
    tinymix 'DAC In Mux' 'Normal Path'
    tinymix 'RCV Mux' 'Voice Playback'
    tinymix 'Ext_Receiver_Amp Switch' 1
    tinymix 'RCV_AMP_MODE' 0
}

function disable_receiver_hp
{
    echo "disabling receiver hp"
    tinymix 'ADDA_DL_CH1 DL1_CH1' 0
    tinymix 'ADDA_DL_CH2 DL1_CH2' 0
    tinymix 'RCV Mux' 'Open'
    tinymix 'Ext_Receiver_Amp Switch' 0
}

function enable_receiver
{
    echo "enabling receiver"
    tinymix 'ADDA_DL_CH1 DL1_CH1' 1
    tinymix 'ADDA_DL_CH2 DL1_CH2' 1
    tinymix 'DAC In Mux' 'Normal Path'
    tinymix 'RCV Mux' 'Voice Playback'
    tinymix 'Ext_Receiver_Amp Switch' 1
    tinymix 'RCV_AMP_MODE' 1
}

function disable_receiver
{
    echo "disabling receiver"
    tinymix 'ADDA_DL_CH1 DL1_CH1' 0
    tinymix 'ADDA_DL_CH2 DL1_CH2' 0
    tinymix 'RCV Mux'  'Open'
    tinymix 'Ext_Receiver_Amp Switch' 0
}

function enable_speaker
{
    echo "enabling speaker"
    tinymix 'ADDA_DL_CH1 DL1_CH1' 1
    tinymix 'ADDA_DL_CH2 DL1_CH2' 1
    tinymix 'ADDA_DL_CH3 DL1_CH1' 1
    tinymix 'ADDA_DL_CH4 DL1_CH2' 1
    tinymix 'DAC In Mux' 'Normal Path'
    tinymix 'LOL Mux' 'Playback'
    tinymix 'Ext_Speaker_Amp Switch' 1

    sleep 1
}

function disable_speaker
{
    echo "disabling speaker"
    tinymix 'ADDA_DL_CH1 DL1_CH1' 0
    tinymix 'ADDA_DL_CH2 DL1_CH2' 0
    tinymix 'ADDA_DL_CH3 DL1_CH1' 0
    tinymix 'ADDA_DL_CH4 DL1_CH2' 0
    tinymix 'LOL Mux' 'Open'
    tinymix 'Ext_Speaker_Amp Switch' 0
}

function enable_ultrasound
{
    echo "enable ultrasound"
    tinymix 'ADDA_DL_CH1 DL7_CH1' 1
    tinymix 'ADDA_DL_CH2 DL7_CH2' 1
    tinymix 'DAC In Mux' 'Normal Path'
    tinymix 'RCV Mux' 'Voice Playback'
    tinymix 'Ext_Receiver_Amp Switch' 1
    tinymix 'RCV_AMP_MODE' 1

}

function disable_ultrasound
{
    echo "disable ultrasound"
    tinymix 'ADDA_DL_CH1 DL7_CH1' 0
    tinymix 'ADDA_DL_CH2 DL7_CH2' 0
    tinymix 'RCV Mux'  'Open'
    tinymix 'Ext_Receiver_Amp Switch' 0

}

if [ "$1" = "spk" ]; then
    enable_speaker
    filename=/vendor/etc/spk.wav
    pcm_id=0
elif [ "$1" = "spk_hp" ]; then
    enable_speaker
    filename=/vendor/etc/spk_hp.wav
    pcm_id=0
elif [ "$1" = "rcv" ]; then
    enable_receiver
    filename=/vendor/etc/rcv.wav
    pcm_id=0
elif [ "$1" = "rcv_hp" ]; then
    enable_receiver_hp
    filename=/vendor/etc/rcv_hp.wav
    pcm_id=0
elif [ "$1" = "us" ]; then
    enable_ultrasound
    filename=/vendor/etc/ultrasound.wav
    pcm_id=7
else
    echo "Usage: playback.sh device; device: spk or spk_hp or rcv or rcv_hp"
fi

echo "start playing"
tinyplay $filename -D 0 -d $pcm_id

if [ "$1" = "spk" ]; then
    disable_speaker
elif [ "$1" = "spk_hp" ]; then
    disable_speaker
elif [ "$1" = "rcv" ]; then
    disable_receiver
elif [ "$1" = "rcv_hp" ]; then
    disable_receiver_hp
elif [ "$1" = "us" ]; then
    disable_ultrasound
fi

exit 0
