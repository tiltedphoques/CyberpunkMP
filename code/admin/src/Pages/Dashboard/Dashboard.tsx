import Box from "@mui/material/Box";
import {ReactElement, Suspense, useEffect, useState} from "react";
import {ErrorBoundary} from "react-error-boundary";
import {Alert, CircularProgress} from "@mui/material";
import {useWebApi, WebApiError, WebApiService} from "../../Services/WebApiService.ts";
import {useToasts} from "../../Toast/ToastProvider.tsx";

interface WidgetData {
  readonly url: string;
  readonly name: string;
  readonly element: ReactElement;
}

export default function Dashboard() {
  const webApi: WebApiService = useWebApi();

  const [widgets, setWidgets] = useState<WidgetData[]>([]);
  const showToast = useToasts();

  useEffect(() => {
    webApi.getPlugins().then(plugins => {
      const widgets = plugins
        .map(plugin => plugin.widget)
        .filter(widget => !!widget);

      setWidgets(widgets);
    }).catch((e) => {
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
    <Box className="page center">
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
