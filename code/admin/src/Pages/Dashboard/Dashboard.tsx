import Box from "@mui/material/Box";
import WidgetGrid from "./WidgetGrid.tsx";
import {useAppSelector} from "../../Stores/AppStore.ts";
import {PluginModule} from "../../Data/PluginData.ts";

export default function Dashboard() {
  const {plugins} = useAppSelector(state => state.plugin);

  const filteredPlugins: PluginModule[] = plugins.filter(plugin => plugin.widget);

  return (
    <Box className="page"
         sx={{height: '100%', padding: 0}}>
      <WidgetGrid plugins={filteredPlugins}/>
    </Box>
  );
}
