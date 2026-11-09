//! Stage 1 measurement switches (docs/visuals/v2/STAGE1-PLAN.md). Read once from the launch parameters; release
//! builds included. With no -nmlv* parameter every value keeps v1.0.0 behaviour, and IsDefault() is true.
//! Invalid values log one warning and fall back to the default.
class NML_VisualsTestConfig
{
	// Layer bits (also used by the manager and the probe)
	static const int LAYER_LUT = 1;
	static const int LAYER_TRIM = 2;
	static const int LAYER_FRINGE = 4;
	static const int LAYER_VIGNETTE = 8;
	static const int LAYER_GRIME = 16;
	static const int LAYER_GRIMEHEAVY = 32;
	static const int LAYERS_PP = 7;			// LUT | TRIM | FRINGE
	static const int LAYERS_OVERLAY = 56;	// VIGNETTE | GRIME | GRIMEHEAVY

	protected static const string DIGITS = "0123456789";

	bool m_bLayersSet;			// -nmlvLayers given: overrides the user's layer settings
	int m_iLayers;
	bool m_bApplyChanged;		// -nmlvApply changed
	bool m_bAltSlots;			// -nmlvSlots alt: fringe at 17
	bool m_bSettingsFilter;		// -nmlvSettingsFilter 1
	bool m_bHideZero;			// -nmlvHideZero 1
	int m_iForcedPreset = -1;	// -nmlvPreset
	int m_iCycleSeconds;		// -nmlvCycle
	bool m_bCycleMasks;			// -nmlvCycle <sec>:<A>/<B> (otherwise OFF/ON)
	int m_iCycleLayersA;
	int m_iCycleLayersB;
	int m_iProbeSeconds;		// -nmlvProbe

	protected static ref NML_VisualsTestConfig s_Instance;

	//------------------------------------------------------------------------------------------------
	static NML_VisualsTestConfig Get()
	{
		if (!s_Instance)
		{
			s_Instance = new NML_VisualsTestConfig();
			s_Instance.Read();
		}

		return s_Instance;
	}

	//------------------------------------------------------------------------------------------------
	bool IsDefault()
	{
		return !m_bLayersSet && !m_bApplyChanged && !m_bAltSlots && !m_bSettingsFilter && !m_bHideZero
			&& m_iForcedPreset < 0 && m_iCycleSeconds <= 0 && m_iProbeSeconds <= 0;
	}

	//------------------------------------------------------------------------------------------------
	string Describe()
	{
		string apply = "legacy";
		if (m_bApplyChanged)
			apply = "changed";

		string slots = "v1";
		if (m_bAltSlots)
			slots = "alt";

		return string.Format("apply=%1 slots=%2 filter=%3 hideZero=%4 layers=%5 cycle=%6 probe=%7",
			apply, slots, m_bSettingsFilter, m_bHideZero, LayersToString(m_iLayers, m_bLayersSet), m_iCycleSeconds, m_iProbeSeconds);
	}

	//------------------------------------------------------------------------------------------------
	static string LayersToString(int layers, bool fromParam = true)
	{
		if (!fromParam)
			return "settings";

		if (layers == 0)
			return "none";

		string s;
		if (layers & LAYER_LUT) s += "lut+";
		if (layers & LAYER_TRIM) s += "trim+";
		if (layers & LAYER_FRINGE) s += "fringe+";
		if (layers & LAYER_VIGNETTE) s += "vignette+";
		if (layers & LAYER_GRIME) s += "grime+";
		if (layers & LAYER_GRIMEHEAVY) s += "grimeheavy+";

		return s.Substring(0, s.Length() - 1);
	}

	//------------------------------------------------------------------------------------------------
	protected void Read()
	{
		string value;

		if (GetParam("nmlvLayers", value))
		{
			int layers = ParseLayers(value);
			if (layers >= 0)
			{
				m_bLayersSet = true;
				m_iLayers = layers;
			}
			else
			{
				Warn("nmlvLayers", value);
			}
		}

		if (GetParam("nmlvApply", value))
		{
			if (value == "changed")
				m_bApplyChanged = true;
			else if (value != "legacy")
				Warn("nmlvApply", value);
		}

		if (GetParam("nmlvSlots", value))
		{
			if (value == "alt")
				m_bAltSlots = true;
			else if (value != "v1")
				Warn("nmlvSlots", value);
		}

		if (GetParam("nmlvSettingsFilter", value))
			m_bSettingsFilter = ParseFlag("nmlvSettingsFilter", value);

		if (GetParam("nmlvHideZero", value))
			m_bHideZero = ParseFlag("nmlvHideZero", value);

		if (GetParam("nmlvPreset", value))
		{
			string upper = value;
			upper.ToUpper();
			int preset = typename.StringToEnum(NML_EVisualsPreset, upper);
			if (preset > NML_EVisualsPreset.OFF && preset <= NML_EVisualsPreset.EXTREME)
				m_iForcedPreset = preset;
			else
				Warn("nmlvPreset", value);
		}

		if (GetParam("nmlvCycle", value))
			ParseCycle(value);

		if (GetParam("nmlvProbe", value))
		{
			int seconds = ParseSeconds(value);
			if (seconds > 0)
				m_iProbeSeconds = seconds;
			else
				Warn("nmlvProbe", value);
		}

		if (!IsDefault())
			PrintFormat("[NML] Visuals test config: %1", Describe());
	}

	//------------------------------------------------------------------------------------------------
	//! <sec> alternates OFF/ON; <sec>:<layersA>/<layersB> alternates two layer sets.
	protected void ParseCycle(string value)
	{
		array<string> parts = {};
		value.Split(":", parts, false);
		if (parts.IsEmpty() || parts.Count() > 2)
		{
			Warn("nmlvCycle", value);
			return;
		}

		int seconds = ParseSeconds(parts[0]);
		if (seconds <= 0)
		{
			Warn("nmlvCycle", value);
			return;
		}

		if (parts.Count() == 2)
		{
			array<string> sets = {};
			parts[1].Split("/", sets, false);
			if (sets.Count() != 2)
			{
				Warn("nmlvCycle", value);
				return;
			}

			int a = ParseLayers(sets[0]);
			int b = ParseLayers(sets[1]);
			if (a < 0 || b < 0)
			{
				Warn("nmlvCycle", value);
				return;
			}

			m_bCycleMasks = true;
			m_iCycleLayersA = a;
			m_iCycleLayersB = b;
		}

		m_iCycleSeconds = seconds;
		if (seconds < 15)
			Print("[NML] Visuals test config: -nmlvCycle below 15 s leaves under 10 s of data per phase after the 5 s discard", LogLevel.WARNING);
	}

	//------------------------------------------------------------------------------------------------
	//! Comma list of layer names (no spaces), or "none". Returns -1 for an empty list or any unknown name.
	static int ParseLayers(string value)
	{
		string lower = value;
		lower.ToLower();
		if (lower == "none")
			return 0;

		array<string> names = {};
		lower.Split(",", names, true);
		if (names.IsEmpty())
			return -1;

		int layers;
		foreach (string name : names)
		{
			switch (name)
			{
				case "lut": layers |= LAYER_LUT; break;
				case "trim": layers |= LAYER_TRIM; break;
				case "fringe": layers |= LAYER_FRINGE; break;
				case "vignette": layers |= LAYER_VIGNETTE; break;
				case "grime": layers |= LAYER_GRIME; break;
				case "grimeheavy": layers |= LAYER_GRIMEHEAVY; break;
				default: return -1;
			}
		}

		return layers;
	}

	//------------------------------------------------------------------------------------------------
	//! Whole seconds 1..3600; anything else returns 0. Digits are checked first because ToInt() throws on
	//! an empty string and returns 0 for text.
	static int ParseSeconds(string value)
	{
		if (value.IsEmpty() || value.Length() > 4)
			return 0;

		for (int i = 0; i < value.Length(); i++)
		{
			if (!DIGITS.Contains(value.Get(i)))
				return 0;
		}

		int seconds = value.ToInt();
		if (seconds < 1 || seconds > 3600)
			return 0;

		return seconds;
	}

	//------------------------------------------------------------------------------------------------
	protected bool ParseFlag(string name, string value)
	{
		if (value == "1")
			return true;

		if (value != "0")
			Warn(name, value);

		return false;
	}

	//------------------------------------------------------------------------------------------------
	//! Accepts the camelCase and the all-lowercase spelling, like v1's diag parameters.
	protected bool GetParam(string name, out string value)
	{
		if (System.GetCLIParam(name, value))
			return true;

		string lower = name;
		lower.ToLower();
		return System.GetCLIParam(lower, value);
	}

	//------------------------------------------------------------------------------------------------
	protected void Warn(string name, string value)
	{
		if (value.Length() > 64)
			value = value.Substring(0, 64) + "...";

		Print(string.Format("[NML] Visuals test config: ignored invalid -%1 '%2'", name, value), LogLevel.WARNING);
	}
}
