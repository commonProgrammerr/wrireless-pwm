import { useCallback } from "react";

interface WaveGraphProps {
  disabled?: boolean;
  mutiplier?: number;
  frequency: number;
  duty_clicle: number;
}

const total_width = 985;
// const total_width = 900;
export function WaveGraph({
  disabled,
  frequency,
  duty_clicle,
  mutiplier = 3,
}: WaveGraphProps) {
  const getPath = useCallback(() => {
    const path = [];
    const factor = Math.round(
      frequency * mutiplier < 1 ? 1 : frequency * mutiplier
    );
    const wave_width = (total_width / (factor * 2)) * 0.8;
    const min = (total_width / (factor * 2)) * 0.1;
    const duty = duty_clicle / 100;

    const high_width = wave_width * duty + min;
    const low_width = min + wave_width - duty * wave_width;

    for (let i = 0; i < factor; i++) {
      path.push(`h${high_width}v${32}h${low_width}V1.5`);
    }
    return `m5,${32}V1.5${path.reduce((p, c) => p + c)}`;
  }, [duty_clicle, frequency]);

  return (
    <svg className="wave-content" viewBox="0 0 500 35" fill="none">
      <path
        stroke={!disabled ? "#00F" : "#00000096"}
        strokeWidth="2"
        strokeLinejoin="round"
        strokeLinecap="round"
        d={getPath()}
      />
    </svg>
  );
}
