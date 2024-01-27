import { ChangeEventHandler, useCallback, useState } from "react";
import "./App.css";
import { Toggle } from "./components/toggle";
import { WaveGraph } from "./components/wave";

function App() {
  const [duty, setDuty] = useState(50);
  const [hz, setHz] = useState(1);
  const [enable, setEnable] = useState(true);

  const handleChangeDuty = useCallback<ChangeEventHandler<HTMLInputElement>>(
    (ev) => {
      ev.preventDefault();
      enable && setDuty(Number(ev.target.value));
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

  return (
    <div className="content-container">
      <h3>Wriless-pwm controll</h3>
      <WaveGraph disabled={!enable} frequency={hz} duty_clicle={duty} />
      <fieldset>
        <label style={{ marginBottom: 3 }}>Enable</label>
        <Toggle checked={enable} onCheckedChange={setEnable} />
      </fieldset>
      <fieldset>
        <label>
          Duty clicle <span>{duty}%</span>
        </label>
        <input
          type="range"
          name="duty"
          disabled={!enable}
          value={duty}
          onChange={handleChangeDuty}
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
          name="frequency"
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
          fetch(`/update-pwm`, {
            method: "POST",
            headers: {
              // Accept: "application/json",
              "Content-Type": "application/json",
            },
            body: JSON.stringify({
              pin: 3,
              enable,
              frequency: hz * 1e6,
              duty,
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
