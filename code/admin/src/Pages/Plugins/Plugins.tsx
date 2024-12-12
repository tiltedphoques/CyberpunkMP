import Box from "@mui/material/Box";
import {Card, Tooltip} from "@mui/material";
import ListItemText from "@mui/material/ListItemText";
import Typography from "@mui/material/Typography";
import {PluginData, useWebApi, WebApiError, WebApiService} from "../../Services/WebApiService.ts";
import {useEffect, useState} from "react";
import {useToasts} from "../../Toast/ToastProvider.tsx";
import {ShowToastCallback} from "../../Toast/ToastReducer.ts";
import {Settings, Widgets} from "@mui/icons-material";
import ListItemIcon from "@mui/material/ListItemIcon";
import ListItem from "@mui/material/ListItem";

export default function Plugins() {
  const webApi: WebApiService = useWebApi();
  const showToast: ShowToastCallback = useToasts();

  const [plugins, setPlugins] = useState<PluginData[]>([]);

  useEffect(() => {
    webApi.getPlugins().then(setPlugins).catch((e) => {
      const error: WebApiError = e as WebApiError;

      if (error.type === 'plugins') {
        showToast({
          style: 'error',
          message: 'Failed to request API. Are you sure the server is running?',
          duration: 5000
        });
      } else {
        showToast({
          style: 'error',
          message: `Unknown error: ${error.message}.`,
          duration: 5000
        });
      }
    });
  }, []);

  return (
    <Box className="page">
      <Card>
        <Typography variant="h4" sx={{mb: '24px'}}>
          Plugins
        </Typography>

        <Typography variant="body1" sx={{textAlign: 'justify', mb: '12px'}}>
          List of all plugins installed on the server:
        </Typography>

        {plugins.map((plugin, i) => (
          <ListItem key={i}
                    component="a"
                    disablePadding
                    sx={{maxHeight: '48px', p: '8px 0'}}>
            <ListItemIcon sx={{mr: '20px', gap: '8px'}}>
              {!!plugin.widget ? <Tooltip title="Widget"><Widgets/></Tooltip> : <Box sx={{width: '24px'}}/>}
              {!!plugin.settings ? <Tooltip title="Settings"><Settings/></Tooltip> : <Box sx={{width: '24px'}}/>}
            </ListItemIcon>
            <ListItemText primary={plugin.name}/>
          </ListItem>
        ))}
      </Card>
    </Box>
  );
}
