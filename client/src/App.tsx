import { ChangeEventHandler, useCallback, useState } from "react";
import "./App.css";
import { Toggle } from "./components/toggle";

function App() {
  const [value, setValue] = useState(50);
  const [hz, setHz] = useState(1);
  const [enable, setEnable] = useState(true);

  const handleChange = useCallback<ChangeEventHandler<HTMLInputElement>>(
    (ev) => {
      ev.preventDefault();
      enable && setValue(Number(ev.target.value));
    },
    [enable]
  );
  const handleChangeHz = useCallback<ChangeEventHandler<HTMLInputElement>>(
    (ev) => {
      ev.preventDefault();
      enable && setHz(Number(ev.target.value));
    },
    [enable]
  );

  function getFrequency() {
    // if (hz < 1e-6) return hz;
    if (hz < 1)
      return (
        (hz * 1e3).toLocaleString(undefined, {
          minimumFractionDigits: 2,
          maximumFractionDigits: 2,
        }) + "Khz"
      );

    return (
      hz.toLocaleString(undefined, {
        minimumFractionDigits: 2,
        maximumFractionDigits: 2,
      }) + "Mhz"
    );
  }

  function getPath() {
    const path = [];
    const factor = Math.round(hz < 1 ? 1 : hz) * 4;
    const total_width = 985;
    const wave_width = total_width / (factor * 2);
    const duty = value / 100;

    for (let i = 0; i < factor; i++) {
      path.push(
        `h${duty * wave_width}v${30}h${wave_width - duty * wave_width}V10`
      );
    }
    return `m5,${40}V10${path.reduce((p, c) => p + c)}`;
  }

  return (
    <div className="content-container">
      <h3>Wriless-pwm controll</h3>
      <svg
        className="wave-content"
        viewBox="0 0 500 60"
        height={80}
        fill="none"
      >
        <path
          stroke={enable ? "#00F" : "#00000096"}
          strokeWidth="2"
          strokeLinejoin="round"
          strokeLinecap="round"
          d={getPath()}
        />
      </svg>
      <Toggle checked={enable} onCheckedChange={setEnable} />
      <fieldset>
        <label>
          Duty clicle <span id="value1">{value}%</span>
        </label>
        <input
          type="range"
          name="speed"
          disabled={!enable}
          value={value}
          onChange={handleChange}
          min="0"
          step="0.1"
          max="100"
        />
      </fieldset>
      <fieldset>
        <label>
          Frequency <span>{getFrequency()}</span>
        </label>
        <input
          type="range"
          name="speed"
          value={hz}
          disabled={!enable}
          onChange={handleChangeHz}
          min={45 / 1e5}
          step={1e-5}
          max={5}
        />
      </fieldset>
      <button
        className="apply-bt"
        onClick={(ev) => {
          ev.preventDefault();
          fetch(`/update-pwm?pin=${1}`, {
            method: "POST",
            body: JSON.stringify({
              enable,
              frequency: hz * 1e6,
              duty: value,
            }),
          });
        }}
      >
        Aplicar
      </button>
    </div>
  );
}

export default App;
