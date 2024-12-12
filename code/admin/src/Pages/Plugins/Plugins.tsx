import Box from "@mui/material/Box";
import {Card, Skeleton, Tooltip} from "@mui/material";
import ListItemText from "@mui/material/ListItemText";
import Typography from "@mui/material/Typography";
import {Fragment, useEffect, useState} from "react";
import {ShowToastCallback} from "../../Toast/Toast.ts";
import {InsertDriveFile, Settings, Widgets} from "@mui/icons-material";
import ListItemIcon from "@mui/material/ListItemIcon";
import ListItem from "@mui/material/ListItem";
import {PluginModule} from "../../WebApi/WebApiData.ts";
import {WebApiError} from "../../WebApi/WebApiClient.ts";
import {useToasts} from "../../Toast/ToastReducer.ts";
import {WebApiService} from "../../WebApi/WebApiService.ts";

export default function Plugins() {
  const showToast: ShowToastCallback = useToasts();

  const [isLoading, setIsLoading] = useState<boolean>(true);
  const [plugins, setPlugins] = useState<PluginModule[]>([]);

  useEffect(() => {
    WebApiService.getPlugins().then(setPlugins).catch((e) => {
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
    }).finally(() => setIsLoading(false));
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

        {isLoading && <Fragment>
            <Skeleton variant="rectangular" height={48} sx={{m: '8px 0'}}/>
            <Skeleton variant="rectangular" height={48} sx={{m: '8px 0'}}/>
            <Skeleton variant="rectangular" height={48} sx={{m: '8px 0'}}/>
            <Skeleton variant="rectangular" height={48} sx={{m: '8px 0'}}/>
        </Fragment>}

        {plugins.map((plugin, i) => (
          <ListItem key={i}
                    component="a"
                    disablePadding
                    sx={{maxHeight: '48px', p: '8px 0'}}>
            <ListItemIcon sx={{mr: '20px', gap: '8px'}}>
              {!!plugin.widget ? <Tooltip title="Widget"><Widgets/></Tooltip> : <Box width={24}/>}
              {!!plugin.page ? <Tooltip title="Page"><InsertDriveFile/></Tooltip> : <Box width={24}/>}
              {!!plugin.settings ? <Tooltip title="Settings"><Settings/></Tooltip> : <Box width={24}/>}
            </ListItemIcon>
            <ListItemText primary={plugin.manifest.name}/>
          </ListItem>
        ))}
      </Card>
    </Box>
  );
}
