import * as React from 'react';
import {styled} from '@mui/material/styles';
import Box from '@mui/material/Box';
import MuiAppBar, {AppBarProps as MuiAppBarProps} from '@mui/material/AppBar';
import CssBaseline from '@mui/material/CssBaseline';
import {ThemeProvider} from "@mui/material";
import {theme} from "./Theme.ts";
import Toolbar from "./Toolbar/Toolbar.tsx";
import Drawer, {drawerWidth} from "./Drawer/Drawer.tsx";
import {Route, Routes} from "react-router";
import Dashboard from "./Pages/Dashboard/Dashboard.tsx";
import Plugins from "./Pages/Plugins/Plugins.tsx";
import Settings from "./Pages/Settings/Settings.tsx";
import About from "./Pages/About/About.tsx";

interface AppBarProps extends MuiAppBarProps {
  open?: boolean;
}

const AppBar = styled(MuiAppBar, {
  shouldForwardProp: (prop) => prop !== 'open',
})<AppBarProps>(({theme}) => ({
  zIndex: theme.zIndex.drawer + 1,
  transition: theme.transitions.create(['width', 'margin'], {
    easing: theme.transitions.easing.sharp,
    duration: theme.transitions.duration.leavingScreen,
  }),
  variants: [
    {
      props: ({open}) => open,
      style: {
        marginLeft: drawerWidth,
        width: `calc(100% - ${drawerWidth}px)`,
        transition: theme.transitions.create(['width', 'margin'], {
          easing: theme.transitions.easing.sharp,
          duration: theme.transitions.duration.enteringScreen,
        }),
      },
    },
  ],
}));

export interface ToolbarProps {
  readonly open: boolean;
  readonly onOpenDrawer?: () => void;
  readonly onCloseDrawer?: () => void;
}

export default function App() {
  const [openDrawer, setOpenDrawer] = React.useState<boolean>(false);

  const handleDrawerOpen = () => {
    setOpenDrawer(true);
  };

  const handleDrawerClose = () => {
    setOpenDrawer(false);
  };

  return (
    <ThemeProvider theme={theme}
                   defaultMode="dark"
                   noSsr>
      <Box sx={{height: '100%', display: 'flex'}}>
        <CssBaseline/>

        <AppBar position="fixed" color="primary" enableColorOnDark open={openDrawer}>
          <Toolbar open={openDrawer} onOpenDrawer={handleDrawerOpen}/>
        </AppBar>

        <Drawer open={openDrawer} onCloseDrawer={handleDrawerClose}/>

        <Box component="main"
             sx={{
               overflowY: 'auto',
               flexGrow: 1,
               p: 3,
               mt: '64px'
             }}>
          <Routes>
            <Route index element={<Dashboard/>}/>
            <Route path="plugins" element={<Plugins/>}/>
            <Route path="settings" element={<Settings/>}/>
            <Route path="about" element={<About/>}/>
          </Routes>
        </Box>
      </Box>
    </ThemeProvider>
  );
}
