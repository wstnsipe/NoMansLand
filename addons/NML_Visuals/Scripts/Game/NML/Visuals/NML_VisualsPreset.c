//! Daytime base values of one NML_Visuals preset (ADR 0007). Weather and time scale these at runtime.
class NML_VisualsPreset
{
	float m_fTrimSaturation;	// Colors.Saturation on top of the LUT
	float m_fGrain;				// FilmGrain.Intensity
	float m_fFringe;			// ChromAber.PowerX/PowerY
	float m_fSoft;				// GaussFilter.Intensity
	float m_fMotion;			// RotBlur.Power (0 = off)
	float m_fVignette;			// overlay opacity
	float m_fGrime;				// overlay opacity (0 = off)

	//------------------------------------------------------------------------------------------------
	void NML_VisualsPreset(float trimSaturation, float grain, float fringe, float soft, float motion, float vignette, float grime)
	{
		m_fTrimSaturation = trimSaturation;
		m_fGrain = grain;
		m_fFringe = fringe;
		m_fSoft = soft;
		m_fMotion = motion;
		m_fVignette = vignette;
		m_fGrime = grime;
	}

	//------------------------------------------------------------------------------------------------
	//! Returns null for OFF.
	static NML_VisualsPreset Get(NML_EVisualsPreset preset)
	{
		switch (preset)
		{
			//                                                      sat   grain fringe  soft  motion vign  grime
			case NML_EVisualsPreset.LOW:       return new NML_VisualsPreset(1.10, 0.10, 0.0015, 0.00, 0.000, 0.35, 0.0);
			case NML_EVisualsPreset.AUTHENTIC: return new NML_VisualsPreset(1.00, 0.22, 0.0030, 0.15, 0.008, 0.55, 0.0);
			// Stubs: placeholders until AUTHENTIC is approved (ADR 0007 prototype 2).
			case NML_EVisualsPreset.HEAVY:     return new NML_VisualsPreset(0.90, 0.35, 0.0050, 0.30, 0.012, 0.70, 0.5);
			case NML_EVisualsPreset.EXTREME:   return new NML_VisualsPreset(0.80, 0.50, 0.0080, 0.45, 0.016, 0.85, 0.8);
		}

		return null;
	}
}
