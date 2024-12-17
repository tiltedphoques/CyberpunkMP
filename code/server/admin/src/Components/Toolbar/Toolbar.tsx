import MUIToolbar from "@mui/material/Toolbar";
import Box from "@mui/material/Box";
import IconButton from "@mui/material/IconButton";
import MenuIcon from "@mui/icons-material/Menu";
import Typography from "@mui/material/Typography";
import ThemeSwitch from "./ThemeSwitch.tsx";
import {ToolbarProps} from "../../App.tsx";

export default function Toolbar({open, onOpenDrawer}: ToolbarProps) {
  return (
    <MUIToolbar sx={{display: 'flex', justifyContent: 'space-between'}}>
      <Box sx={{display: 'flex', alignItems: 'center'}}>
        <IconButton
          color="inherit"
          aria-label="open drawer"
          onClick={onOpenDrawer}
          edge="start"
          sx={[
            {
              marginRight: 5,
            },
            open && {display: 'none'},
          ]}
        >
          <MenuIcon/>
        </IconButton>

        <Typography variant="h6" noWrap component="div">
          CyberpunkMP &middot; Admin Panel
        </Typography>
      </Box>

      <ThemeSwitch/>
    </MUIToolbar>
  );
}
