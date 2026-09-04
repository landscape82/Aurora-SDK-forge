/** HardwareSmokeTest
 *
 *  A module-oriented hardware validation example for Aurora.
 *
 *  What it checks:
 *  - stereo audio passthrough
 *  - all knob controls
 *  - all CV inputs
 *  - freeze/reverse/shift buttons
 *  - freeze/reverse gate inputs
 *  - RGB LED output
 *
 *  LED behavior:
 *  - LED_1..LED_6 show knob level in green and matching CV level in blue
 *  - LED_FREEZE shows freeze button/gate state and left input peak
 *  - LED_REVERSE shows reverse button/gate state and right input peak
 *  - LED_BOT_1 shows left input level
 *  - LED_BOT_2 shows right input level
 *  - LED_BOT_3 shows shift button state
 *
 *  Audio behavior:
 *  - audio always passes through
 *  - holding Freeze or driving the Freeze gate adds a quiet left-channel test tone
 *  - holding Reverse or driving the Reverse gate adds a quiet right-channel test tone
 */
#include <math.h>

#include "aurora.h"
#include "daisysp.h"

using namespace daisy;
using namespace aurora;
using namespace daisysp;

Hardware   hw;
Oscillator left_tone;
Oscillator right_tone;

struct UiState
{
    float knob[KNOB_LAST];
    float cv[CV_LAST];
    float input_peak[2];
    bool  button_freeze;
    bool  button_reverse;
    bool  button_shift;
    bool  gate_freeze;
    bool  gate_reverse;
};

volatile UiState ui_state = {};

static float Clamp01(float value)
{
    if(value < 0.0f)
        return 0.0f;
    if(value > 1.0f)
        return 1.0f;
    return value;
}

static float BipolarToUnit(float value)
{
    return Clamp01((value + 1.0f) * 0.5f);
}

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    hw.ProcessAllControls();

    for(int i = 0; i < KNOB_LAST; i++)
    {
        ui_state.knob[i] = hw.GetKnobValue(i);
    }

    for(int i = 0; i < CV_LAST; i++)
    {
        ui_state.cv[i] = BipolarToUnit(hw.GetCvValue(i));
    }

    const bool freeze_pressed = hw.GetButton(SW_FREEZE).Pressed();
    const bool reverse_pressed = hw.GetButton(SW_REVERSE).Pressed();
    const bool shift_pressed = hw.GetButton(SW_SHIFT).Pressed();
    const bool freeze_gate = hw.GetGateState(GATE_FREEZE);
    const bool reverse_gate = hw.GetGateState(GATE_REVERSE);

    ui_state.button_freeze = freeze_pressed;
    ui_state.button_reverse = reverse_pressed;
    ui_state.button_shift = shift_pressed;
    ui_state.gate_freeze = freeze_gate;
    ui_state.gate_reverse = reverse_gate;

    float left_peak = 0.0f;
    float right_peak = 0.0f;

    const bool left_tone_on = freeze_pressed || freeze_gate;
    const bool right_tone_on = reverse_pressed || reverse_gate;

    for(size_t i = 0; i < size; i++)
    {
        float left = in[0][i];
        float right = in[1][i];

        const float abs_left = fabsf(left);
        const float abs_right = fabsf(right);

        if(abs_left > left_peak)
            left_peak = abs_left;
        if(abs_right > right_peak)
            right_peak = abs_right;

        if(left_tone_on)
            left += left_tone.Process() * 0.15f;
        if(right_tone_on)
            right += right_tone.Process() * 0.15f;

        out[0][i] = left;
        out[1][i] = right;
    }

    ui_state.input_peak[0] = Clamp01(left_peak * 2.0f);
    ui_state.input_peak[1] = Clamp01(right_peak * 2.0f);
}

int main(void)
{
    hw.Init();

    left_tone.Init(hw.AudioSampleRate());
    left_tone.SetWaveform(Oscillator::WAVE_SIN);
    left_tone.SetFreq(220.0f);
    left_tone.SetAmp(1.0f);

    right_tone.Init(hw.AudioSampleRate());
    right_tone.SetWaveform(Oscillator::WAVE_SIN);
    right_tone.SetFreq(440.0f);
    right_tone.SetAmp(1.0f);

    hw.StartAudio(AudioCallback);

    const Leds knob_leds[KNOB_LAST]
        = {LED_1, LED_2, LED_3, LED_4, LED_5, LED_6};

    while(1)
    {
        hw.ClearLeds();

        for(int i = 0; i < KNOB_LAST; i++)
        {
            hw.SetLed(knob_leds[i], 0.0f, ui_state.knob[i], ui_state.cv[i]);
        }

        const float freeze_red = ui_state.gate_freeze ? 0.4f : 0.0f;
        const float freeze_green = ui_state.input_peak[0];
        const float freeze_blue = ui_state.button_freeze ? 0.8f : 0.0f;
        hw.SetLed(LED_FREEZE, freeze_red, freeze_green, freeze_blue);

        const float reverse_red = ui_state.button_reverse ? 0.8f : 0.0f;
        const float reverse_green = ui_state.input_peak[1];
        const float reverse_blue = ui_state.gate_reverse ? 0.4f : 0.0f;
        hw.SetLed(LED_REVERSE, reverse_red, reverse_green, reverse_blue);

        hw.SetLed(LED_BOT_1, ui_state.input_peak[0], 0.0f, 0.0f);
        hw.SetLed(LED_BOT_2, 0.0f, 0.0f, ui_state.input_peak[1]);
        hw.SetLed(LED_BOT_3,
                  ui_state.button_shift ? 0.6f : 0.0f,
                  ui_state.button_shift ? 0.6f : 0.0f,
                  ui_state.button_shift ? 0.6f : 0.0f);

        hw.WriteLeds();
        hw.DelayMs(25);
    }
}
