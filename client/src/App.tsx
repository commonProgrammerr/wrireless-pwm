import { ChangeEventHandler, useCallback, useEffect, useState } from "react";
import "./App.css";
// import { Toggle } from "./components/toggle";
import { WaveGraph } from "./components/wave";

function App() {
  const [duty, setDuty] = useState(512);
  const [hz, setHz] = useState(1);
  // const [enable, setEnable] = useState(true);

  useEffect(() => {
    fetch("/load").then(res => res.json()).then(data => {
      data.duty && setDuty(data.duty)
      data.frequency && setHz(data.frequency)
    })
  }, [])

  const handleChangeDuty = useCallback<ChangeEventHandler<HTMLInputElement>>(
    (ev) => {
      ev.preventDefault();
      setDuty(Number(ev.target.value));
    },
    []
  );
  const handleChangeHz = useCallback<ChangeEventHandler<HTMLInputElement>>(
    (ev) => {
      ev.preventDefault();
      setHz(Number(ev.target.value));
    },
    []
  );

  function getFrequency() {
    if (hz < 1)
      return (
        (hz * 1e3).toLocaleString(undefined, {
          minimumFractionDigits: 3,
          maximumFractionDigits: 3,
        }) + "hz"
      );

    return (
      hz.toLocaleString(undefined, {
        minimumFractionDigits: 3,
        maximumFractionDigits: 3,
      }) + "Khz"
    );
  }

  return (
    <div className="content-container">
      <h3>Wriless-pwm controll</h3>
      <WaveGraph frequency={hz} mutiplier={1} duty_clicle={duty/10.24} />
      {/* <fieldset>
        <label style={{ marginBottom: 3 }}>{enable ? 'ON' : 'OFF' }</label>
        <Toggle checked={enable} onCheckedChange={setEnable} />
      </fieldset> */}
      <button
        className="toggle-bt"
        onClick={(ev) => {
          ev.preventDefault()
          fetch(`/enable`, { method: "POST" });
        }}
      >
        ON/OFF
      </button>
      <fieldset>
        <label>
          Duty clicle <span>{(duty / 10.24).toLocaleString(undefined, {
            minimumFractionDigits: 1,
            maximumFractionDigits: 1
          })}%</span>
        </label>
        <input
          type="range"
          name="duty"
          value={duty}
          onChange={handleChangeDuty}
          min="0"
          step="1"
          max="1024"
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
          onChange={handleChangeHz}
          min={1}
          step={1e-3}
          max={10}
        />
      </fieldset>
      <button
        className="apply-bt"
        onClick={(ev) => {
          ev.preventDefault();
          fetch(`http://192.168.4.1/update-pwm`, {
            method: "POST",
            headers: { "Content-Type": "application/json", },
            body: JSON.stringify({
              pin: 4,
              enable: true,
              frequency: hz * 1000,
              duty,
            }),
          });
        }}
      >
        Apply
      </button>
    </div>
  );
}

export default App;
