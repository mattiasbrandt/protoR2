// =============================================================================
// dome.js
//
// Dome page controller.
// - What the droid sent to Dome Rotation, after the speed limit (read-only)
// - Where the dome believes it points, Front is here and Go home (#445)
// - Dome motor configuration load/save, and the full turn the bearing is
//   integrated against
// - Shared API helper error handling
// =============================================================================
(() => {
  const domeFeedback = document.getElementById("dome-feedback");
  const domeDisabledCard = document.getElementById("dome-disabled-card");
  const domeHardwareState = document.getElementById("dome-hardware-state");
  const domeSpeedDisplay = document.getElementById("dome-speed-display");
  const domePulseDisplay = document.getElementById("dome-pulse-display");
  const domeRotationState = document.getElementById("dome-rotation-state");
  const domeLiveFill = document.getElementById("dome-live-fill");
  const domeTrackNegative = document.getElementById("dome-track-negative");
  const domeTrackPositive = document.getElementById("dome-track-positive");

  const domeNeutral = document.getElementById("dome-neutral");
  const domeMinPulse = document.getElementById("dome-min-pulse");
  const domeMaxPulse = document.getElementById("dome-max-pulse");
  const domeSpeedLimit = document.getElementById("dome-speed-limit");
  const reloadEscButton = document.getElementById("reload-esc-button");
  const escFeedback = document.getElementById("esc-feedback");

  const domeRndEnable = document.getElementById("dome-rnd-enable");
  const domeRndSpeed = document.getElementById("dome-rnd-speed");
  const domeRndPauseMin = document.getElementById("dome-rnd-pause-min");
  const domeRndPauseMax = document.getElementById("dome-rnd-pause-max");
  const domeRndMoveMs = document.getElementById("dome-rnd-move-ms");
  const reloadRndButton = document.getElementById("reload-rnd-button");
  const rndFeedback = document.getElementById("rnd-feedback");

  const domeBearingValue = document.getElementById("dome-bearing-value");
  const domeBearingWord = document.getElementById("dome-bearing-word");
  const domeFrontButton = document.getElementById("dome-front-button");
  const domeHomeButton = document.getElementById("dome-home-button");

  const domeTurnState = document.getElementById("dome-turn-state");
  const domeTurnMs = document.getElementById("dome-turn-ms");
  const domeTurnPct = document.getElementById("dome-turn-pct");
  const domeTurnDir = document.getElementById("dome-turn-dir");
  const domeTurnDirButtons = Array.from(domeTurnDir?.querySelectorAll("button[data-value]") || []);
  const turnFeedback = document.getElementById("turn-feedback");

  let domeHardwareEnabled = true;
  // Only WHETHER the droid has sent a reading yet, not what it said about web
  // control: this surface has no control web control gates, so the value
  // itself is the plate's to report (#348).
  let statusHeard = false;
  // What the feedback line last said about the dome itself (updateDomeControlsEnabled()).
  let feedbackState = null;

  const FEEDBACK_BASE_CLASS = "feedback";

  const showFeedback = (el, text, level = "") => {
    if (!el) return;
    el.textContent = text;
    el.className = level ? `${FEEDBACK_BASE_CLASS} ${level}` : FEEDBACK_BASE_CLASS;
  };

  // The three readouts this surface used to paint as colored pills are now
  // words, and the setPillState that painted them is gone with them. None of
  // the three was a health signal, which is the only thing that may take a
  // signal color (GLOSSARY.md "Status Color"):
  //
  //   the dome motor switched on or off in Configuration is an AVAILABILITY FAMILY,
  //   "change it here", and those are told apart by treatment and never by hue
  //   - it read green when on and amber when off, and amber promises the
  //     builder something is wrong rather than that they made a choice;
  //
  //   which way the dome is being turned is a VALUE - it read green forward
  //     and amber reverse, so turning left looked like a symptom;
  //
  //   whether this browser may command the droid is a CHOSEN POSTURE and
  //     takes no color at all. It is also the Status Plate's CONTROL chip, so
  //     what is left here is the half the plate cannot carry: that the consent
  //     is the feet's and the dome turns either way.
  const setText = (el, text) => {
    if (el) el.textContent = text;
  };


  // What the droid last reported it sent (GET /api/status domeRotationSentPct
  // and domeRotationPulseUs), or null before it has said, or when its firmware
  // does not report it. Kept so a Positive turns press redraws the word at once.
  let lastSent = null;

  // The one rule every surface prints the sent figure by: one decimal, as the
  // droid sends it in domeRotationSentPct and writes it in its own Dome
  // Rotation detail (src/web/status_json.cpp). The smallest turn that goes out
  // is 0.1 %, so a turning dome never reads 0.0 %.
  const sentPercentText = (pct) => `${pct.toFixed(1)}%`;

  // The gauge's ends say which way each side turns once Positive turns is
  // set (the shared rule, data/health_signals.js domeRotationWord); until then
  // they say only how far.
  const paintTrackEnds = () => {
    const words = window.PAHealthSignals;
    const known = turnDir === "cw" || turnDir === "ccw";
    setText(domeTrackNegative, known ? words.domeRotationWord(-1, turnDir) : "-100%");
    setText(domeTrackPositive, known ? words.domeRotationWord(1, turnDir) : "100%");
  };

  const renderSent = (sent) => {
    lastSent = sent;
    paintTrackEnds();
    if (sent === null) {
      setText(domeSpeedDisplay, "");
      setText(domePulseDisplay, "");
      setText(domeRotationState, window.PALiveReading.UNKNOWN);
      if (domeLiveFill) domeLiveFill.style.opacity = "0";
      return;
    }
    const percent = Math.max(-100, Math.min(100, sent.pct));
    const widthPct = Math.abs(percent) / 2;

    setText(domeSpeedDisplay, sentPercentText(percent));
    setText(domePulseDisplay, sent.pulseUs > 0 ? `${sent.pulseUs} \u00b5s` : "");

    // Which side of centre the bar fills is what says the direction; the bar's
    // own color is one color, declared in the stylesheet, the same one Foot
    // Drive's live output bars take. It used to be mixed towards green going
    // forward and towards amber going back, which made one of the two
    // directions look like a fault.
    if (domeLiveFill) {
      domeLiveFill.style.width = `${widthPct}%`;
      if (percent === 0) {
        domeLiveFill.style.opacity = "0";
        domeLiveFill.style.left = "50%";
      } else if (percent >= 0) {
        domeLiveFill.style.opacity = "1";
        domeLiveFill.style.left = "50%";
      } else {
        domeLiveFill.style.opacity = "1";
        domeLiveFill.style.left = `calc(50% - ${widthPct}%)`;
      }
    }

    // The word, and only the word. The signed percentage sits beside it in
    // #dome-speed-display, and printing the number in both put the same fact on
    // the plate twice. Stopped is a share of exactly 0, the rule the
    // Dashboard's Dome Rotation row reads too (#504).
    setText(domeRotationState, window.PAHealthSignals.domeRotationWord(percent, turnDir));
  };

  const updateDomeControlsEnabled = () => {
    const configEnabled = domeHardwareEnabled;
    window.PAApi.gateControls(
      [domeNeutral, domeMinPulse, domeMaxPulse, domeSpeedLimit, reloadEscButton,
       domeRndEnable, domeRndSpeed, domeRndPauseMin, domeRndPauseMax, domeRndMoveMs, reloadRndButton,
       domeFrontButton, domeHomeButton, domeTurnMs, domeTurnPct, ...domeTurnDirButtons],
      configEnabled,
    );

    domeDisabledCard?.classList.toggle("hidden", domeHardwareEnabled);

    setText(
      domeHardwareState,
      domeHardwareEnabled ? "on in Configuration" : "switched off in Configuration",
    );

    // Written when the state it reports changes, not on every reading: the
    // line also carries the answer to Front is here and Go home, and a reading
    // a second later would otherwise wipe the droid's clause before it is read.
    const state = !domeHardwareEnabled ? "off" : !statusHeard ? "waiting" : "ready";
    if (state === feedbackState) return;
    feedbackState = state;
    if (state === "off") {
      showFeedback(domeFeedback, "Dome Rotation is switched off. Switch it on in Configuration.", "warning");
    } else if (state === "waiting") {
      showFeedback(domeFeedback, "Waiting for the droid.");
    } else {
      // All this knows is that readings arrive: nothing comes back from the
      // dome itself.
      showFeedback(domeFeedback, "The droid is answering.");
    }
  };

  const setDomeHardwareEnabled = (enabled) => {
    domeHardwareEnabled = enabled;
    updateDomeControlsEnabled();
  };

  const resolveDomeEnabledFromStatus = (payload) => {
    if (typeof payload?.domeEnabled === "boolean") {
      return payload.domeEnabled;
    }
    if (typeof payload?.components?.domeRotation?.enabled === "boolean") {
      return payload.components.domeRotation.enabled;
    }
    return null;
  };

  // The sent figure from one status frame, or null when it carries none.
  const resolveSent = (payload) => {
    const pct = window.PAHealthSignals.domeRotationSentPct(payload);
    if (pct === null) return null;
    const pulseUs = Number(payload.domeRotationPulseUs);
    return { pct, pulseUs: Number.isFinite(pulseUs) ? pulseUs : 0 };
  };

  const renderReading = (reading) => {
    const payload = reading.status;
    statusHeard = payload !== null;

    const statusDomeEnabled = resolveDomeEnabledFromStatus(payload);
    if (typeof statusDomeEnabled === "boolean") {
      domeHardwareEnabled = statusDomeEnabled;
    }

    renderSent(resolveSent(payload));
    updateDomeControlsEnabled();
  };

  // ---------------------------------------------------------------------------
  // Where the dome points (#445). The number comes from the one accessor every
  // dome drawing reads (data/dome_bearing.js), with "believed" beside it in its
  // own row; unknown is a word in the number's place, never a number.
  // ---------------------------------------------------------------------------
  const renderBearing = (bearing) => {
    if (!domeBearingValue) return;
    if (bearing?.believed) {
      domeBearingValue.textContent = `${bearing.deg.toFixed(1)}°`;
      domeBearingValue.classList.remove("is-unknown");
      if (domeBearingWord) domeBearingWord.hidden = false;
    } else {
      domeBearingValue.textContent = "Unknown";
      domeBearingValue.classList.add("is-unknown");
      if (domeBearingWord) domeBearingWord.hidden = true;
    }
  };

  // A press goes to the droid, which decides; a refusal comes back as its one
  // clause (include/dome_bearing_act.h) and is shown as it is.
  const pressBearingAct = async (route, done) => {
    if (!window.PAApi) return;
    try {
      await window.PAApi.postForm(route, {}, { timeoutMs: 3000 });
      showFeedback(domeFeedback, done, "success");
    } catch (error) {
      showFeedback(domeFeedback, window.PAApi.messageFor(error), "warning");
    }
  };

  domeFrontButton?.addEventListener("click", () => pressBearingAct("/api/dome/front", "Front set."));
  domeHomeButton?.addEventListener("click", () => pressBearingAct("/api/dome/home", "Turning home."));

  // ---------------------------------------------------------------------------
  // The full turn: three numbers the droid cannot measure. 0 and `unset` are
  // "never recorded", so a box holding 0 is shown empty and the subtitle says
  // so until all three are set.
  // ---------------------------------------------------------------------------
  let turnDir = "unset";

  const paintTurnDir = () => {
    domeTurnDirButtons.forEach((button) => {
      const on = button.dataset.value === turnDir;
      button.classList.toggle("active", on);
      button.setAttribute("aria-pressed", on ? "true" : "false");
      button.setAttribute("aria-checked", on ? "true" : "false");
    });
  };

  const paintTurnState = () => {
    const set = Number(domeTurnMs?.value) > 0 && Number(domeTurnPct?.value) > 0 && turnDir !== "unset";
    setText(domeTurnState, set ? "set" : "not set");
  };

  const renderTurnSnapshot = (domeRotation) => {
    const shown = (value) => (Number(value) > 0 ? String(value) : "");
    if (domeTurnMs && domeRotation.fullTurnMs !== undefined) domeTurnMs.value = shown(domeRotation.fullTurnMs);
    if (domeTurnPct && domeRotation.fullTurnPct !== undefined) domeTurnPct.value = shown(domeRotation.fullTurnPct);
    if (domeRotation.positiveTurn !== undefined) turnDir = String(domeRotation.positiveTurn);
    paintTurnDir();
    paintTurnState();
    renderSent(lastSent);
  };

  // What the builder typed; an empty box is left out rather than sent as 0.
  const turnPayload = (extra = {}) => {
    const payload = { ...extra };
    if (typed(domeTurnMs) !== "") payload.domeRotationFullTurnMs = typed(domeTurnMs);
    if (typed(domeTurnPct) !== "") payload.domeRotationFullTurnPct = typed(domeTurnPct);
    return payload;
  };

  // True when the droid took the save; false when nothing was sent or the
  // droid refused it.
  const saveTurn = async (extra = {}) => {
    if (!window.PAApi || !domeHardwareEnabled) return false;
    const payload = turnPayload(extra);
    if (Object.keys(payload).length === 0) return false;
    showFeedback(turnFeedback, "Saving...");
    try {
      await window.PAApi.postForm("/api/config", payload, { timeoutMs: 3000 });
      paintTurnState();
      showFeedback(turnFeedback, `Saved at ${new Date().toLocaleTimeString()}`, "success");
      return true;
    } catch (error) {
      showFeedback(turnFeedback, `Failed to save the full turn: ${window.PAApi.messageFor(error, payload)}`, "error");
      return false;
    }
  };

  // The press shows at once; a save the droid did not take puts the stored
  // direction back, so the word and the gauge ends never name a direction
  // the droid does not hold.
  const showTurnDir = (value) => {
    turnDir = value;
    paintTurnDir();
    paintTurnState();
    renderSent(lastSent);
  };

  domeTurnDirButtons.forEach((button) => {
    button.addEventListener("click", async () => {
      if (button.dataset.value === turnDir) return;
      const stored = turnDir;
      const pressed = button.dataset.value;
      showTurnDir(pressed);
      const saved = await saveTurn({ domeRotationPositiveTurn: pressed });
      // A later press while this save was out owns the buttons now.
      if (!saved && turnDir === pressed) showTurnDir(stored);
    });
  });

  const renderEscConfigSnapshot = (data) => {
    const domeRotation = data?.domeRotation || {};
    const components = data?.components || {};

    if (domeNeutral && domeRotation.neutralUs !== undefined) domeNeutral.value = domeRotation.neutralUs;
    if (domeMinPulse && domeRotation.minPulseUs !== undefined) domeMinPulse.value = domeRotation.minPulseUs;
    if (domeMaxPulse && domeRotation.maxPulseUs !== undefined) domeMaxPulse.value = domeRotation.maxPulseUs;
    if (domeSpeedLimit && domeRotation.speedLimitPct !== undefined) domeSpeedLimit.value = domeRotation.speedLimitPct;

    if (domeRndEnable && domeRotation.rndEnable !== undefined) domeRndEnable.checked = domeRotation.rndEnable;
    if (domeRndSpeed && domeRotation.rndSpeedPct !== undefined) domeRndSpeed.value = domeRotation.rndSpeedPct;
    if (domeRndPauseMin && domeRotation.rndPauseMin !== undefined) domeRndPauseMin.value = domeRotation.rndPauseMin;
    if (domeRndPauseMax && domeRotation.rndPauseMax !== undefined) domeRndPauseMax.value = domeRotation.rndPauseMax;
    if (domeRndMoveMs && domeRotation.rndMoveMs !== undefined) domeRndMoveMs.value = domeRotation.rndMoveMs;
    renderTurnSnapshot(domeRotation);

    setDomeHardwareEnabled(Boolean(components.domeRotation?.enabled));
  };


  const loadEscConfig = async ({ handle = null } = {}) => {
    if (!window.PAApi) throw new Error("API helper unavailable");
    showFeedback(escFeedback, "Loading motor settings...");

    try {
      const api = handle || window.PAApi;
      const result = await api.get("/api/config");
      renderEscConfigSnapshot(result.data);
      const ts = new Date().toLocaleTimeString();
      showFeedback(escFeedback, `Motor settings loaded at ${ts}`, "success");
      showFeedback(rndFeedback, `Loaded at ${ts}`, "success");
      showFeedback(turnFeedback, `Loaded at ${ts}`, "success");
    } catch (error) {
      showFeedback(escFeedback, `Failed to load motor settings: ${window.PAApi.messageFor(error)}`, "error");
      showFeedback(turnFeedback, "Failed to load the full turn.", "error");
      throw error;
    }
  };

  // What goes out is what the builder typed: the droid holds the ranges and
  // the pulse order, and a value it will not take comes back as a refusal
  // worded by PAApi.messageFor() (ADR 0068, amended 2026-09-26).
  const typed = (input) => String(input?.value ?? "").trim();

  const escPayload = () => ({
    domeRotationNeutralUs: typed(domeNeutral),
    domeRotationMinPulseUs: typed(domeMinPulse),
    domeRotationMaxPulseUs: typed(domeMaxPulse),
    domeRotationSpeedLimitPct: typed(domeSpeedLimit),
  });

  const saveEscConfig = async () => {
    if (!window.PAApi) return;
    if (!domeHardwareEnabled) {
      showFeedback(escFeedback, "Dome ESC settings unavailable: switch Dome Rotation on in Configuration.", "warning");
      return;
    }

    showFeedback(escFeedback, "Saving...");

    try {
      await window.PAApi.postForm(
        "/api/config",
        escPayload(),
        { timeoutMs: 3000 },
      );

      showFeedback(escFeedback, `Saved at ${new Date().toLocaleTimeString()}`, "success");
    } catch (error) {
      showFeedback(escFeedback, `Failed to save motor settings: ${window.PAApi.messageFor(error)}`, "error");
    }
  };

  const rndPayload = () => ({
    domeRotationRndEnable: domeRndEnable?.checked ? "true" : "false",
    domeRotationRndSpeedPct: typed(domeRndSpeed),
    domeRotationRndPauseMin: typed(domeRndPauseMin),
    domeRotationRndPauseMax: typed(domeRndPauseMax),
    domeRotationRndMoveMs: typed(domeRndMoveMs),
  });

  const saveRndDomeConfig = async () => {
    if (!window.PAApi) return;
    if (!domeHardwareEnabled) {
      showFeedback(rndFeedback, "Random movement unavailable: switch Dome Rotation on in Configuration.", "warning");
      return;
    }

    showFeedback(rndFeedback, "Saving...");

    try {
      await window.PAApi.postForm(
        "/api/config",
        rndPayload(),
        { timeoutMs: 3000 },
      );

      showFeedback(rndFeedback, `Saved at ${new Date().toLocaleTimeString()}`, "success");
    } catch (error) {
      showFeedback(rndFeedback, `Failed to save random movement settings: ${window.PAApi.messageFor(error)}`, "error");
    }
  };

  const debouncedSave = window.PAUtils.debounce(saveEscConfig, 500);
  const debouncedRndSave = window.PAUtils.debounce(saveRndDomeConfig, 500);
  const debouncedTurnSave = window.PAUtils.debounce(() => saveTurn(), 500);

  domeNeutral?.addEventListener("input", debouncedSave);
  domeMinPulse?.addEventListener("input", debouncedSave);
  domeMaxPulse?.addEventListener("input", debouncedSave);
  domeSpeedLimit?.addEventListener("input", debouncedSave);
  reloadEscButton?.addEventListener("click", loadEscConfig);

  domeRndEnable?.addEventListener("input", debouncedRndSave);
  domeRndSpeed?.addEventListener("input", debouncedRndSave);
  domeRndPauseMin?.addEventListener("input", debouncedRndSave);
  domeRndPauseMax?.addEventListener("input", debouncedRndSave);
  domeRndMoveMs?.addEventListener("input", debouncedRndSave);
  reloadRndButton?.addEventListener("click", loadEscConfig);

  domeTurnMs?.addEventListener("input", debouncedTurnSave);
  domeTurnPct?.addEventListener("input", debouncedTurnSave);

  // The dome's live state rides the Live Reading, which owns the stream or the
  // one fallback poll for the whole shell (data/live_reading.js).
  window.PALiveReading.subscribe(renderReading);
  window.PADomeBearing?.subscribe(renderBearing);

  // -------------------------------------------------------------------------
  // Boot — load config then start status subscription
  // -------------------------------------------------------------------------

  // Page Recovery: register startup API load as a section so the bootstrap
  // can show recovery state if the config fetch fails.
  // See docs/page-load-recovery-architecture.md and ADR 0019.
  const SECTIONS = [
    ["dome-configuration", loadEscConfig, "dome configuration"],
  ];

  const startPageLoad = () => {
    if (!window.PABootstrap) {
      loadEscConfig().catch(() => {});
      return;
    }
    window.PABootstrap.setResourceLabels?.({
      "/web_api.js": "Body Controller connection",
      "/status_stream.js": "live updates",
      "/live_reading.js": "live updates",
      "/dome_bearing.js": "where the dome points",
      "/shell.js": "page layout",
      "/dome.js": "dome control",
    });
    SECTIONS.forEach(([name, load, label]) =>
      window.PABootstrap.registerSection(name, load, { label })
    );
  };

  renderSent(null);
  updateDomeControlsEnabled();
  startPageLoad();
})();
