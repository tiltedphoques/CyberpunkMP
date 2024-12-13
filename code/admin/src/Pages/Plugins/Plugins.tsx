import Box from "@mui/material/Box";
import {Card, Tooltip} from "@mui/material";
import ListItemText from "@mui/material/ListItemText";
import Typography from "@mui/material/Typography";
import {InsertDriveFile, Widgets} from "@mui/icons-material";
import ListItemIcon from "@mui/material/ListItemIcon";
import ListItem from "@mui/material/ListItem";
import {useAppSelector} from "../../Stores/AppStore.ts";

export default function Plugins() {
  const {plugins} = useAppSelector(state => state.plugin);

  return (
    <Box className="page">
      <Card>
        <Typography variant="h4" sx={{mb: '24px'}}>
          Plugins
        </Typography>

        <Typography variant="body1" sx={{textAlign: 'justify', mb: '12px'}}>
          List of all plugins installed on the server:
        </Typography>

        {plugins.map((plugin, i) => (
          <ListItem key={i}
                    component="a"
                    disablePadding
                    sx={{maxHeight: '48px', p: '8px 0'}}>
            <ListItemIcon sx={{mr: '20px', gap: '8px'}}>
              {!!plugin.widget ? <Tooltip title="Widget"><Widgets/></Tooltip> : <Box width={24}/>}
              {!!plugin.page ? <Tooltip title="Page"><InsertDriveFile/></Tooltip> : <Box width={24}/>}
              {/*{!!plugin.settings ? <Tooltip title="Settings"><Settings/></Tooltip> : <Box width={24}/>}*/}
            </ListItemIcon>
            <ListItemText primary={plugin.manifest.name}/>
          </ListItem>
        ))}
      </Card>
    </Box>
  );
}
