import {useEffect, useState} from 'react';
import {Route, Routes} from 'react-router';
import {styled} from '@mui/material/styles';
import Box from '@mui/material/Box';
import MuiAppBar, {AppBarProps as MuiAppBarProps} from '@mui/material/AppBar';
import CssBaseline from '@mui/material/CssBaseline';
import {ThemeProvider} from "@mui/material";
import {theme} from "./Theme.ts";
import Toolbar from "./Components/Toolbar/Toolbar.tsx";
import Drawer, {drawerWidth} from "./Components/Drawer/Drawer.tsx";
import ToastProvider from "./Components/Toast/ToastProvider.tsx";
import Dashboard from "./Pages/Dashboard/Dashboard.tsx";
import Plugins from "./Pages/Plugins/Plugins.tsx";
import Settings from "./Pages/Settings/Settings.tsx";
import About from "./Pages/About/About.tsx";
import {useAppDispatch, useAppSelector} from "./Stores/AppStore.ts";
import {PluginModule} from "./Data/PluginData.ts";
import {WebApiClient, WebApiError} from "./Clients/WebApiClient.ts";
import {loadPlugins} from "./Stores/PluginStore.ts";
import {loadLayouts} from './Stores/StorageStore.ts';
import {showToast} from './Stores/ToastStore.ts';

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
  const {isLoaded} = useAppSelector(state => state.plugin);
  const dispatch = useAppDispatch();

  const [openDrawer, setOpenDrawer] = useState<boolean>(false);

  useEffect(() => {
    if (isLoaded) {
      return;
    }
    dispatch(loadLayouts());
    getPlugins().then(plugins => {
      dispatch(loadPlugins(plugins));
    }).catch(e => {
      const error = e as WebApiError;

      if (error.type === 'plugins') {
        dispatch(showToast({
          style: 'error',
          message: 'Failed to request API. Are you sure the server is running?',
          duration: 5000
        }));
      }
    });
  }, []);

  const getPlugins = async (): Promise<PluginModule[]> => {
    const plugins: PluginModule[] = await WebApiClient.getPlugins();

    for (const plugin of plugins) {
      const module: PluginModule | undefined = await WebApiClient.getWidget(plugin.manifest.name);

      if (module) {
        plugin.manifest = {...plugin.manifest, ...module.manifest};
        //plugin.settings = module.settings;
        plugin.widget = module.widget;
        plugin.page = module.page;
      }
    }
    return plugins;
  };

  const onOpenDrawer = () => {
    setOpenDrawer(true);
  };

  const onCloseDrawer = () => {
    setOpenDrawer(false);
  };

  return (
    <ThemeProvider theme={theme}
                   defaultMode="system"
                   noSsr>
      <ToastProvider/>

      <Box sx={{height: '100%', display: 'flex'}}>
        <CssBaseline/>

        <AppBar position="fixed"
                color="primary"
                enableColorOnDark
                open={openDrawer}>
          <Toolbar open={openDrawer} onOpenDrawer={onOpenDrawer}/>
        </AppBar>

        <Drawer open={openDrawer} onCloseDrawer={onCloseDrawer}/>

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
