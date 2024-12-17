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
import {loadPlugins, updatePlugin} from "./Stores/PluginStore.ts";
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
  const {isLoaded, plugins} = useAppSelector(state => state.plugin);
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

  useEffect(() => {
    // NOTE: only enable hot-reload of plugins during development. Tree-shaking
    //       compliant.
    if (import.meta.env.DEV) {
      const handle: number = setInterval(reloadPlugins, 1000);

      return () => {
        clearInterval(handle);
      };
    }
  }, [plugins]);

  const getPlugins = async (): Promise<PluginModule[]> => {
    const entries: PluginModule[] = await WebApiClient.getPlugins();
    const plugins: PluginModule[] = [];

    for (const entry of entries) {
      let plugin: PluginModule | undefined = await importPlugin(entry.manifest.name);

      if (plugin) {
        plugin = await resolvePlugin(entry, plugin);
        plugins.push(plugin);
      }
    }
    return plugins;
  };

  const importPlugin = async (name: string): Promise<PluginModule | undefined> => {
    // NOTE: manually fetch widget's script, otherwise SystemJS hits the cache
    //       instead of the latest version of the widget.
    const blob: Blob | undefined = await WebApiClient.getWidget(name);

    if (!blob) {
      return;
    }
    const url: string = URL.createObjectURL(blob);
    let module: PluginModule | undefined;

    try {
      // NOTE: clean previous module, SystemJS doesn't override module by
      //       default.
      delete (window as never)[name];
      module = await System.import(url) as PluginModule;
    } finally {
      URL.revokeObjectURL(url);
    }
    return module;
  };

  const resolvePlugin = async (plugin: PluginModule, module: PluginModule): Promise<PluginModule> => {
    return {
      updatedAt: await WebApiClient.getWidgetETag(plugin.manifest.name),
      manifest: {...plugin.manifest, ...module.manifest},
      //settings: {...plugin.settings},
      widget: module.widget,
      page: module.page
    };
  };

  const reloadPlugins = async (): Promise<void> => {
    for (const plugin of plugins) {
      const updatedAt: string | undefined = await WebApiClient.getWidgetETag(plugin.manifest.name, plugin.updatedAt);

      if (updatedAt && updatedAt !== plugin.updatedAt) {
        let module: PluginModule | undefined = await importPlugin(plugin.manifest.name);

        if (module) {
          module = await resolvePlugin(plugin, module);
          dispatch(updatePlugin(module));
        }
      }
    }
  }

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
