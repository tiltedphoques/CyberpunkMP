import RGL, {Layout, ReactGridLayoutProps, WidthProvider, WidthProviderProps} from "react-grid-layout";
import {Component, createElement, Suspense, useEffect, useRef, useState} from "react";
import {Alert, Card, CircularProgress} from "@mui/material";
import {ErrorBoundary} from "react-error-boundary";
import {PluginManifest, PluginModule, PluginWidget} from "../../Data/PluginData.ts";
import {useAppDispatch, useAppSelector} from "../../Stores/AppStore.ts";
import {saveLayouts} from "../../Stores/StorageStore.ts";

const GridLayout = WidthProvider(RGL);
type GridLayoutComponent = Component<ReactGridLayoutProps & WidthProviderProps, any, any>;

interface WidgetGridProps {
  readonly plugins: PluginModule[];
}

interface WidgetLayout {
  readonly key: string;
  readonly component: any;
  readonly layout: Layout;
}

export default function WidgetGrid({plugins}: WidgetGridProps) {
  const MARGIN: number = 24;
  const CELL_SIZE: number = 64;

  const $root = useRef<GridLayoutComponent | null>(null);

  const {layouts} = useAppSelector(state => state.storage);
  const dispatch = useAppDispatch();

  const [columns, setColumns] = useState<number>(28);
  const [grid, setGrid] = useState<WidgetLayout[]>([]);

  useEffect(() => {
    const grid: WidgetLayout[] = plugins.map((plugin, i) => {
      const key: string = getLayoutKey(plugin.manifest);
      const layout: Layout | undefined = layouts.find(cache => cache.i === key);

      return createLayout(plugin, layout, i);
    });

    setGrid(grid);
  }, [plugins]);

  useEffect(() => {
    if (!$root.current) {
      return;
    }
    // @ts-ignore
    const rect: DOMRect = $root.current.elementRef.current.getBoundingClientRect();
    const columns: number = Math.round(rect.width / CELL_SIZE);
    const width: number = rect.width - MARGIN * columns;

    setColumns(Math.ceil(width / CELL_SIZE));
  }, [$root]);

  const createLayout = (plugin: PluginModule, cache: Layout | undefined, i: number): WidgetLayout => {
    const widget: PluginWidget = plugin.widget!;
    const layout: Layout = {
      ...widget.layout,
      i: getLayoutKey(plugin.manifest),
      x: 0,
      y: i,
      w: widget.layout.minW ?? 4,
      h: widget.layout.minH ?? 4,
      ...cache
    };

    return {
      key: layout.i,
      component: createElement(widget.component),
      layout: layout
    } as WidgetLayout;
  }

  const getLayoutKey = (manifest: PluginManifest): string => `${manifest.author}/${manifest.name}`;

  const onLayoutChange = (layouts: Layout[]) => {
    dispatch(saveLayouts(layouts));
  };

  const layout: Layout[] = grid.map(item => item.layout);

  return (
    <GridLayout
      ref={$root}
      className="layout"
      margin={[MARGIN, MARGIN]}
      containerPadding={[0, 0]}
      cols={columns}
      rowHeight={CELL_SIZE}
      layout={layout}
      isDraggable
      isResizable
      onLayoutChange={onLayoutChange}
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
