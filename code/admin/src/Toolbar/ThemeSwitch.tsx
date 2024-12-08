import Box from "@mui/material/Box";
import {Tooltip, useColorScheme} from "@mui/material";
import IconButton from "@mui/material/IconButton";
import {AutoMode, DarkMode, LightMode} from "@mui/icons-material";

export default function ThemeSwitch() {
  const {mode, setMode} = useColorScheme();

  const updateMode = () => {
    let newMode: 'light' | 'dark' | 'system' | null = null;

    if (mode === 'light') {
      newMode = 'dark';
    } else if (mode === 'dark') {
      newMode = 'system';
    } else if (mode === 'system') {
      newMode = 'light';
    }
    setMode(newMode);
  }

  return (
    <Box>
      <Tooltip title="Switch theme mode">
        <IconButton onClick={updateMode}>
          {mode === 'light' && <DarkMode/>}
          {mode === 'dark' && <AutoMode/>}
          {mode === 'system' && <LightMode/>}
        </IconButton>
      </Tooltip>
    </Box>
  );
}
