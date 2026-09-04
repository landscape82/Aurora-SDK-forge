/** Template
 *
 *  Reusable starting point for custom Aurora firmware.
 *
 *  This example keeps the DSP simple on purpose:
 *  - stereo passthrough with a wet path gain stage
 *  - clearly separated control, audio, and LED update functions
 *  - a single state struct to extend for future firmware
 *
 *  Suggested next steps when copying this template:
 *  - replace UpdateEffectState() with your control mapping
 *  - replace ProcessSample() with your DSP
 *  - replace UpdateLeds() with your module-specific feedback
 *  - add USB or persistent settings only after the core behavior is stable
 */
#include "aurora.h"

using namespace daisy;
using namespace aurora;

Hardware hw;

struct TemplateState
{
    float knob_mix;
    float knob_warp;
    float cv_mix;
    float cv_warp;
    float wet_amount;
    float warp_amount;
    float input_peak[2];
    bool  freeze_active;
    bool  reverse_active;
    bool  shift_active;
};

volatile TemplateState state = {};

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

static void UpdateEffectState()
{
    state.knob_mix = hw.GetKnobValue(KNOB_MIX);
    state.knob_warp = hw.GetKnobValue(KNOB_WARP);
    state.cv_mix = BipolarToUnit(hw.GetCvValue(CV_MIX));
    state.cv_warp = BipolarToUnit(hw.GetCvValue(CV_WARP));

    state.wet_amount = Clamp01((state.knob_mix * 0.75f) + (state.cv_mix * 0.25f));
    state.warp_amount
        = Clamp01((state.knob_warp * 0.75f) + (state.cv_warp * 0.25f));

    state.freeze_active
        = hw.GetButton(SW_FREEZE).Pressed() || hw.GetGateState(GATE_FREEZE);
    state.reverse_active
        = hw.GetButton(SW_REVERSE).Pressed() || hw.GetGateState(GATE_REVERSE);
    state.shift_active = hw.GetButton(SW_SHIFT).Pressed();
}

static float ProcessSample(float input, float wet_amount, float warp_amount)
{
    const float gain = 0.6f + (warp_amount * 0.8f);
    const float dry = input;
    const float wet = input * gain;
    return (dry * (1.0f - wet_amount)) + (wet * wet_amount);
}

static void UpdateLeds()
{
    hw.ClearLeds();

    hw.SetLed(LED_1, 0.0f, state.knob_mix, state.cv_mix);
    hw.SetLed(LED_2, state.wet_amount, state.wet_amount, 0.0f);
    hw.SetLed(LED_3, 0.0f, state.knob_warp, state.cv_warp);
    hw.SetLed(LED_4, state.warp_amount, 0.0f, state.warp_amount);
    hw.SetLed(LED_5, state.input_peak[0], 0.0f, 0.0f);
    hw.SetLed(LED_6, 0.0f, 0.0f, state.input_peak[1]);

    hw.SetLed(LED_FREEZE,
              state.freeze_active ? 0.0f : 0.0f,
              state.freeze_active ? 0.6f : 0.0f,
              state.freeze_active ? 0.6f : 0.0f);
    hw.SetLed(LED_REVERSE,
              state.reverse_active ? 0.7f : 0.0f,
              state.reverse_active ? 0.4f : 0.0f,
              0.0f);

    hw.SetLed(LED_BOT_1, state.freeze_active ? 0.0f : state.wet_amount, 0.0f, 0.0f);
    hw.SetLed(LED_BOT_2, 0.0f, state.reverse_active ? 0.0f : state.warp_amount, 0.0f);
    hw.SetLed(LED_BOT_3,
              state.shift_active ? 0.5f : 0.0f,
              state.shift_active ? 0.5f : 0.0f,
              state.shift_active ? 0.5f : 0.5f * state.wet_amount);

    hw.WriteLeds();
}

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    hw.ProcessAllControls();
    UpdateEffectState();

    float left_peak = 0.0f;
    float right_peak = 0.0f;

    for(size_t i = 0; i < size; i++)
    {
        float left = ProcessSample(in[0][i], state.wet_amount, state.warp_amount);
        float right = ProcessSample(in[1][i], state.wet_amount, state.warp_amount);

        if(state.freeze_active)
        {
            left *= 0.8f;
            right *= 0.8f;
        }

        if(state.reverse_active)
        {
            left = -left;
            right = -right;
        }

        const float abs_left = fabsf(left);
        const float abs_right = fabsf(right);
        if(abs_left > left_peak)
            left_peak = abs_left;
        if(abs_right > right_peak)
            right_peak = abs_right;

        out[0][i] = left;
        out[1][i] = right;
    }

    state.input_peak[0] = Clamp01(left_peak * 2.0f);
    state.input_peak[1] = Clamp01(right_peak * 2.0f);
}

int main(void)
{
    hw.Init();
    hw.StartAudio(AudioCallback);

    while(1)
    {
        UpdateLeds();
        hw.DelayMs(25);
    }
}
