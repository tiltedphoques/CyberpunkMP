import Box from "@mui/material/Box";
import {createElement, Fragment, ReactElement, Suspense, useEffect, useState} from "react";
import {ErrorBoundary} from "react-error-boundary";
import {Alert} from "@mui/material";
import Module = System.Module;

interface PluginDto {
  readonly Name: string;
}

interface WidgetData {
  readonly url: string;
  readonly element: ReactElement;
}

export default function Dashboard() {
  const [widgets, setWidgets] = useState<WidgetData[]>([]);

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
            element: element,
          });
        } catch (error) {
          console.error(error);
        }
      }

      if (lock) {
        return;
      }
      setWidgets(widgets);
    }

    let lock: boolean = false;

    getWidgets();
    return () => {
      lock = true;
    };
  }, []);

  useEffect(() => {

  }, [widgets]);

  const getPlugins = async (): Promise<PluginDto[]> => {
    const response: Response = await fetch('/api/v1/plugins');

    if (!response.ok) {
      return [];
    }
    return await response.json() as PluginDto[];
  }

  return (
    <Box className="page center">
      <ErrorBoundary fallbackRender={
        ({error}) => {
          return (
            <Alert severity="error" sx={{width: '500px', m: 'auto'}}>
              Error while rendering widgets: {error.message}
            </Alert>
          );
        }
      }>
        {widgets.map((widget, i) => {
          return (
            <Fragment key={i}>
              <Box>
                <Suspense key={i}
                          fallback={
                            <Alert severity="info" sx={{width: '500px', m: 'auto'}}>
                              Loading widgets...
                            </Alert>
                          }>
                  {widget.element}
                </Suspense>
              </Box>
            </Fragment>
          );
        })}
      </ErrorBoundary>
    </Box>
  );
}
