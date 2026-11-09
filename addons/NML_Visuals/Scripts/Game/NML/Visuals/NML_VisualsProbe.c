//! Stage 1 frame-time probe (docs/visuals/v2/STAGE1-PLAN.md). Exists only with -nmlvProbe <seconds>.
//! Per frame: one tick-count read, integer maths and one histogram increment; no allocations. One summary
//! line per interval (parsed by tools/nml-visuals/probe-summary.mjs). Keep it on in both arms of a
//! comparison so its own cost cancels out.
class NML_VisualsProbe
{
	protected static const int BINS = 129;				// 1 ms bins 0..127 plus overflow
	protected static const int DISCARD_MS = 5000;		// after a phase switch: first material/texture loads
	protected static const int APPLY_WINDOW_MS = 500;	// spikes this soon after a non-CYCLE Apply are attributed to it
	protected static const int SPIKE_MS = 33;

	protected NML_VisualsManager m_Manager;	// weak: the manager owns the probe
	protected int m_iIntervalMs;
	protected string m_sPhase = "-";

	// Frame statistics (reset per interval)
	protected ref array<int> m_aHist = {};
	protected bool m_bStarted;
	protected int m_iLast;
	protected int m_iIntervalStart;
	protected int m_iDiscardUntil;
	protected int m_iFrames;
	protected int m_iSumMs;
	protected int m_iMaxMs;
	protected int m_iSpikes33;
	protected int m_iSpikes50;
	protected int m_iSpikes100;
	protected int m_iApplyWindowEnd;
	protected int m_iApplySpikes;
	protected int m_iApplyMaxMs;

	// Events (reset per interval)
	protected ref array<int> m_aApplies = {};
	protected int m_iSets;
	protected int m_iClears;
	protected int m_iReasserts;
	protected int m_iCameraChanges;
	protected int m_iControlledChanges;
	protected int m_iSettingsOurs;
	protected int m_iSettingsOther;
	protected int m_iStarts;

	//------------------------------------------------------------------------------------------------
	void NML_VisualsProbe(NML_VisualsManager manager, int intervalSeconds)
	{
		m_Manager = manager;
		m_iIntervalMs = intervalSeconds * 1000;

		for (int i = 0; i < BINS; i++)
			m_aHist.Insert(0);

		for (int r = 0; r <= NML_EVisualsApplyReason.CYCLE; r++)
			m_aApplies.Insert(0);

		GetGame().GetCallqueue().CallLater(Tick, 0, true);
		PrintFormat("[NML] Probe: started (interval %1 s)", intervalSeconds);
	}

	//------------------------------------------------------------------------------------------------
	void ~NML_VisualsProbe()
	{
		if (GetGame() && GetGame().GetCallqueue())
			GetGame().GetCallqueue().Remove(Tick);
	}

	//------------------------------------------------------------------------------------------------
	protected void Tick()
	{
		int now = System.GetTickCount();
		if (!m_bStarted)
		{
			// First frame: skip world-load hitches like after a phase switch.
			m_bStarted = true;
			m_iLast = now;
			m_iIntervalStart = now;
			m_iDiscardUntil = now + DISCARD_MS;
			return;
		}

		int dt = now - m_iLast;
		m_iLast = now;
		if (dt < 0)
			dt = 0;

		if (now >= m_iDiscardUntil)
		{
			int bin = dt;
			if (bin >= BINS)
				bin = BINS - 1;

			m_aHist[bin] = m_aHist[bin] + 1;
			m_iFrames++;
			m_iSumMs += dt;
			if (dt > m_iMaxMs)
				m_iMaxMs = dt;

			if (dt > SPIKE_MS)
			{
				m_iSpikes33++;
				if (dt > 50)
					m_iSpikes50++;

				if (dt > 100)
					m_iSpikes100++;
			}

			if (now <= m_iApplyWindowEnd)
			{
				if (dt > SPIKE_MS)
					m_iApplySpikes++;

				if (dt > m_iApplyMaxMs)
					m_iApplyMaxMs = dt;
			}
		}

		if (now - m_iIntervalStart >= m_iIntervalMs)
			Flush(now);
	}

	//------------------------------------------------------------------------------------------------
	//! A cycle phase started: close the running interval and skip the first DISCARD_MS of the new phase.
	void OnPhase(string label)
	{
		int now = System.GetTickCount();
		Flush(now);
		m_sPhase = label;
		m_iDiscardUntil = now + DISCARD_MS;
	}

	//------------------------------------------------------------------------------------------------
	void OnApply(NML_EVisualsApplyReason reason, int sets, int clears, int reasserts)
	{
		m_aApplies[reason] = m_aApplies[reason] + 1;
		m_iSets += sets;
		m_iClears += clears;
		m_iReasserts += reasserts;
		if (reason != NML_EVisualsApplyReason.CYCLE)
			m_iApplyWindowEnd = System.GetTickCount() + APPLY_WINDOW_MS;
	}

	void OnCameraChanged() { m_iCameraChanges++; }
	void OnControlledChanged() { m_iControlledChanges++; }
	void OnStart() { m_iStarts++; }

	//------------------------------------------------------------------------------------------------
	void OnSettings(bool ours)
	{
		if (ours)
			m_iSettingsOurs++;
		else
			m_iSettingsOther++;
	}

	//------------------------------------------------------------------------------------------------
	protected void Flush(int now)
	{
		if (m_iFrames > 0)
		{
			string applies;
			for (int r = 0; r < m_aApplies.Count(); r++)
			{
				if (r > 0)
					applies += ",";

				applies += m_aApplies[r].ToString();
			}

			// Integer maths only; mean frame time in 1/100 ms. string.Format takes at most 9 arguments: add new
			// fields as a further Format call, not to these.
			int mean100 = (m_iSumMs * 100) / m_iFrames;
			string frames = string.Format("phase=%1 ms=%2 frames=%3 mean100=%4 p50=%5 p95=%6 p99=%7 max=%8",
				m_sPhase, m_iSumMs, m_iFrames, mean100, Percentile(50), Percentile(95), Percentile(99), m_iMaxMs);
			string spikes = string.Format("s33=%1 s50=%2 s100=%3 aSpk=%4 aMax=%5", m_iSpikes33, m_iSpikes50, m_iSpikes100, m_iApplySpikes, m_iApplyMaxMs);
			string events = string.Format("applies=%1 sets=%2 clears=%3 reasserts=%4 cam=%5 ctl=%6 setOurs=%7 setOther=%8 starts=%9",
				applies, m_iSets, m_iClears, m_iReasserts, m_iCameraChanges, m_iControlledChanges, m_iSettingsOurs, m_iSettingsOther, m_iStarts);

			Print("[NML] Probe: " + frames + " " + spikes + " " + events + " " + m_Manager.GetProbeState());
		}

		Reset(now);
	}

	//------------------------------------------------------------------------------------------------
	//! Frame time (ms) at percentile p, from the 1 ms histogram.
	protected int Percentile(int p)
	{
		int target = (m_iFrames * p + 99) / 100;
		int seen;
		for (int i = 0; i < BINS; i++)
		{
			seen += m_aHist[i];
			if (seen >= target)
				return i;
		}

		return BINS - 1;
	}

	//------------------------------------------------------------------------------------------------
	protected void Reset(int now)
	{
		for (int i = 0; i < BINS; i++)
			m_aHist[i] = 0;

		for (int r = 0; r < m_aApplies.Count(); r++)
			m_aApplies[r] = 0;

		m_iIntervalStart = now;
		m_iFrames = 0;
		m_iSumMs = 0;
		m_iMaxMs = 0;
		m_iSpikes33 = 0;
		m_iSpikes50 = 0;
		m_iSpikes100 = 0;
		m_iApplySpikes = 0;
		m_iApplyMaxMs = 0;
		m_iSets = 0;
		m_iClears = 0;
		m_iReasserts = 0;
		m_iCameraChanges = 0;
		m_iControlledChanges = 0;
		m_iSettingsOurs = 0;
		m_iSettingsOther = 0;
		m_iStarts = 0;
	}
}
