import Box from "@mui/material/Box";
import {Fab, Tooltip} from "@mui/material";
import {useMemo, useState} from "react";
import Icon from "@mdi/react";
import {mdiViewDashboard, mdiViewDashboardEdit} from "@mdi/js";
import WidgetGrid from "./WidgetGrid.tsx";
import {useAppSelector} from "../../Stores/AppStore.ts";
import {PluginModule} from "../../Data/PluginData.ts";

export type GridMode = 'view' | 'edit';

export default function Dashboard() {
  const {plugins} = useAppSelector(state => state.plugin);

  const [gridMode, setGridMode] = useState<GridMode>('view');

  const onToggleGridMode = () => {
    setGridMode(gridMode === 'view' ? 'edit' : 'view');
  };

  const filteredPlugins: PluginModule[] = useMemo(() => {
    return plugins.filter(plugin => plugin.widget);
  }, [plugins]);

  const isEditing: boolean = gridMode === 'edit';

  return (
    <Box className="page"
         sx={{position: 'relative', height: '100%', padding: 0}}>
      <WidgetGrid plugins={filteredPlugins} gridMode={gridMode}/>

      <Tooltip title={isEditing ? "View widgets" : "Edit widgets"}>
        <Fab aria-label="toggle grid mode"
             color="secondary"
             sx={{position: 'absolute', right: 0, bottom: 0}}
             onClick={onToggleGridMode}>
          <Icon size="28px" path={isEditing ? mdiViewDashboard : mdiViewDashboardEdit}/>
        </Fab>
      </Tooltip>
    </Box>
  );
}
