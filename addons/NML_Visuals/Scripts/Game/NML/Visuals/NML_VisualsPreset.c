//! Daytime base values of one NML_Visuals preset (ADR 0007). Weather and time scale these at runtime.
//! All presets share one colour identity (the EXTREME grade the lead chose as anchor; AUTHENTIC uses a lighter
//! version of it). Strength grows through camera degradation: grime, lens falloff, a little fringing.
//! No preset uses blur or grain, and the stationary image stays sharp.
class NML_VisualsPreset
{
	protected static const ResourceName LUT_LOW = "{AB48A8929ECE40AD}Materials/NML/Visuals/NML_Visuals_Lut_Low.emat";
	protected static const ResourceName LUT_FAMILY_LIGHT = "{6A7CD3BFB176F91B}Materials/NML/Visuals/NML_Visuals_Lut_Day.emat";
	protected static const ResourceName LUT_FAMILY = "{5C8AE66255F800AA}Materials/NML/Visuals/NML_Visuals_Lut_Extreme.emat";

	ResourceName m_sDayLut;		// ColorGrading material used in daylight (night always uses the shared LowLight LUT)
	float m_fTrimSaturation;	// Colors.Saturation on top of the LUT
	float m_fFringe;			// ChromAber.PowerX/PowerY
	float m_fVignette;			// vignette overlay opacity
	float m_fGrime;				// base grime overlay opacity (0 = off)
	float m_fGrimeHeavy;		// dense EXTREME grime layer on top (0 = off)
	float m_fWeather;			// how strongly weather and time change the look (1 = AUTHENTIC)

	protected static ref array<ref NML_VisualsPreset> s_aPresets;

	//------------------------------------------------------------------------------------------------
	void NML_VisualsPreset(ResourceName dayLut, float trimSaturation, float fringe, float vignette, float grime, float grimeHeavy, float weather)
	{
		m_sDayLut = dayLut;
		m_fTrimSaturation = trimSaturation;
		m_fFringe = fringe;
		m_fVignette = vignette;
		m_fGrime = grime;
		m_fGrimeHeavy = grimeHeavy;
		m_fWeather = weather;
	}

	//------------------------------------------------------------------------------------------------
	//! Returns null for OFF. Presets are immutable and built once.
	static NML_VisualsPreset Get(NML_EVisualsPreset preset)
	{
		if (preset <= NML_EVisualsPreset.OFF || preset > NML_EVisualsPreset.EXTREME)
			return null;

		if (!s_aPresets)
		{
			s_aPresets = {};
			for (int i = 0; i <= NML_EVisualsPreset.EXTREME; i++)
				s_aPresets.Insert(Create(i));
		}

		return s_aPresets[preset];
	}

	//------------------------------------------------------------------------------------------------
	protected static NML_VisualsPreset Create(NML_EVisualsPreset preset)
	{
		switch (preset)
		{
			//                                                             day LUT           sat   fringe  vign  grime heavy weather
			case NML_EVisualsPreset.LOW:       return new NML_VisualsPreset(LUT_LOW,          1.00, 0.0010, 0.25, 0.00, 0.00, 0.8);
			case NML_EVisualsPreset.AUTHENTIC: return new NML_VisualsPreset(LUT_FAMILY_LIGHT, 1.05, 0.0015, 0.35, 0.00, 0.00, 1.0);
			case NML_EVisualsPreset.HEAVY:     return new NML_VisualsPreset(LUT_FAMILY,       1.10, 0.0025, 0.45, 0.45, 0.00, 1.3);
			case NML_EVisualsPreset.EXTREME:   return new NML_VisualsPreset(LUT_FAMILY,       1.10, 0.0035, 0.60, 0.80, 0.85, 1.6);
		}

		return null; // OFF
	}
}
