import * as Switch from "@radix-ui/react-switch";
import "./styles.css";

export function Toggle(props: Switch.SwitchProps) {
  return (
    <Switch.Root className="SwitchRoot" {...props}>
      <Switch.Thumb className="SwitchThumb" />
    </Switch.Root>
  );
}
