import Box from "@mui/material/Box";
import {createElement, Suspense, useEffect, useState} from "react";
import {ErrorBoundary} from "react-error-boundary";
import {Alert, CircularProgress} from "@mui/material";
import {useToasts} from "../../Toast/ToastReducer.ts";
import {WebApiError} from "../../WebApi/WebApiClient.ts";
import {WidgetData} from "../../WebApi/WebApiData.ts";
import {WebApiService} from "../../WebApi/WebApiService.ts";

export default function Dashboard() {
  const [isLoading, setIsLoading] = useState<boolean>(true);
  const [widgets, setWidgets] = useState<WidgetData[]>([]);
  const showToast = useToasts();

  useEffect(() => {
    WebApiService
      .getPlugins()
      .then(plugins => {
        const widgets: WidgetData[] = plugins
          .filter(plugin => !!plugin.widget && !!plugin.widget!.module)
          .map(plugin => plugin.widget!);

        for (const widget of widgets) {
          widget.element = createElement(widget.module!.default);
        }
        setWidgets(widgets);
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
    <Box className="page">
      {isLoading && <CircularProgress/>}

      {widgets.map((widget, i) => (
        <Box key={i}>
          <ErrorBoundary fallbackRender={
            ({error}) => (
              <Alert severity="error" sx={{width: '500px', m: 'auto'}}>
                Error while rendering widget {widget.name}:
                {error.message}
              </Alert>
            )
          }>
            <Suspense fallback={<CircularProgress/>}>
              {widget.element}
            </Suspense>
          </ErrorBoundary>
        </Box>
      ))}
    </Box>
  );
}
