import RGL, {Layout, ReactGridLayoutProps, WidthProvider, WidthProviderProps} from "react-grid-layout";
import {PluginModule, PluginWidget} from "../../WebApi/WebApiData.ts";
import {Component, createElement, Suspense, useEffect, useRef, useState} from "react";
import {Alert, Card, CircularProgress} from "@mui/material";
import {ErrorBoundary} from "react-error-boundary";

const GridLayout = WidthProvider(RGL);
type GridLayoutComponent = Component<ReactGridLayoutProps & WidthProviderProps, any, any>;

interface WidgetGridProps {
  readonly widgets: PluginModule[];
}

interface WidgetLayout {
  readonly key: string;
  readonly component: any;
  readonly layout: Layout;
}

export default function WidgetGrid({widgets: plugins}: WidgetGridProps) {
  const MARGIN: number = 24;
  const CELL_SIZE: number = 64;

  const root = useRef<GridLayoutComponent | null>(null);
  const [columns, setColumns] = useState<number>(28);
  const [grid, setGrid] = useState<WidgetLayout[]>([]);

  useEffect(() => {
    if (!root.current) {
      return;
    }
    // @ts-ignore
    const rect: DOMRect = root.current.elementRef.current.getBoundingClientRect();
    const columns: number = Math.round(rect.width / CELL_SIZE);
    const width: number = rect.width - MARGIN * columns;

    setColumns(Math.ceil(width / CELL_SIZE));
  }, [root]);

  useEffect(() => {
    const layouts: WidgetLayout[] = plugins.map((plugin, i) => {
      const widget: PluginWidget = plugin.widget!;
      const layout: Layout = {
        ...widget.layout,
        i: `${plugin.manifest.author}/${plugin.manifest.name}`,
        x: 0,
        y: i,
        w: widget.layout.minW ?? 4,
        h: widget.layout.minH ?? 4,
      };

      return {
        key: layout.i,
        component: createElement(widget.component),
        layout: layout
      } as WidgetLayout;
    });

    setGrid(layouts);
  }, [plugins]);

  const layout: Layout[] = grid.map(item => item.layout);

  return (
    <GridLayout
      ref={root}
      className="layout"
      layout={layout}
      isDraggable
      isResizable
      margin={[MARGIN, MARGIN]}
      containerPadding={[0, 0]}
      cols={columns}
      rowHeight={CELL_SIZE}
    >
      {grid.map(widget => (
        <Card key={widget.key}>
          <ErrorBoundary fallbackRender={
            ({error}) => (
              <Alert severity="error" sx={{width: '500px', m: 'auto'}}>
                Error while rendering widget: {error.message}
              </Alert>
            )
          }>
            <Suspense fallback={<CircularProgress/>}>
              {widget.component}
            </Suspense>
          </ErrorBoundary>
        </Card>
      ))}
    </GridLayout>
  );
}
