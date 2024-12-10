import Box from "@mui/material/Box";
import {createElement, ReactElement, Suspense, useEffect, useState} from "react";
import {ErrorBoundary} from "react-error-boundary";
import {Alert, CircularProgress} from "@mui/material";
import {useToasts} from "../../Toast/ToastProvider.tsx";
import Module = System.Module;

interface PluginDto {
  readonly Name: string;
}

interface WidgetData {
  readonly url: string;
  readonly name: string;
  readonly element: ReactElement;
}

export default function Dashboard() {
  const [widgets, setWidgets] = useState<WidgetData[]>([]);
  const showToast = useToasts();

  useEffect(() => {
    async function getWidgets() {
      const plugins = await getPlugins();
      const widgets: WidgetData[] = [];

      for (const plugin of plugins) {
        const url: string = `/api/v1/plugins/${plugin.Name}/assets/widget.umd.js`;

        try {
          const module: Module = await System.import(url);
          const element: ReactElement = createElement(module.default);

          widgets.push({
            url: url,
            name: plugin.Name,
            element: element,
          });
        } catch (error) {
          showToast({
            style: 'error',
            message: `Failed to load widget of plugin ${plugin.Name}:\n${error}`,
          });
        }
      }
      setWidgets(widgets);
    }

    getWidgets();
  }, []);

  const getPlugins = async (): Promise<PluginDto[]> => {
    const response: Response = await fetch('/api/v1/plugins');

    if (!response.ok) {
      showToast({
        style: 'error',
        message: 'Failed to request API. Are you sure the server is running?',
        duration: 5000
      });
      return [];
    }
    return await response.json() as PluginDto[];
  }

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
