import Box from "@mui/material/Box";
import {useEffect, useState} from "react";
import {CircularProgress} from "@mui/material";
import {useToasts} from "../../Toast/ToastReducer.ts";
import {WebApiError} from "../../WebApi/WebApiClient.ts";
import {PluginModule} from "../../WebApi/WebApiData.ts";
import {WebApiService} from "../../WebApi/WebApiService.ts";
import WidgetGrid from "./WidgetGrid.tsx";
import {ShowToastCallback} from "../../Toast/Toast.ts";

export default function Dashboard() {
  const [isLoading, setIsLoading] = useState<boolean>(true);
  const [plugins, setPlugins] = useState<PluginModule[]>([]);
  const showToast: ShowToastCallback = useToasts();

  useEffect(() => {
    WebApiService
      .getPlugins()
      .then(plugins => {
        plugins = plugins.filter(plugin => plugin.widget);
        setPlugins(plugins);
      })
      .catch((e) => {
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
    <Box className="page"
         sx={{height: '100%', padding: 0}}>
      {isLoading && <CircularProgress/>}

      <WidgetGrid plugins={plugins}/>
    </Box>
  );
}
