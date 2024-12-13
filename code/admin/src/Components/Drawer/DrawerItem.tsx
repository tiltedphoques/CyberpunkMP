import ListItem from "@mui/material/ListItem";
import ListItemButton from "@mui/material/ListItemButton";
import ListItemIcon from "@mui/material/ListItemIcon";
import ListItemText from "@mui/material/ListItemText";
import {ReactElement} from "react";
import {Link} from "react-router";

export interface DrawerItemProps {
  readonly open: boolean;
  readonly route: string;
  readonly text: string;
  readonly icon: ReactElement;
}

export default function DrawerItem({open, route, text, icon}: DrawerItemProps) {
  return (
    <ListItem key={text}
              disablePadding
              sx={{display: 'block'}}
    >
      <ListItemButton
        sx={[
          {
            minHeight: 48,
            px: 2.5,
          },
          open ? {justifyContent: 'initial'} : {justifyContent: 'center'},
        ]}
        component={Link}
        to={route}
      >
        <ListItemIcon
          sx={[
            {
              minWidth: 0,
              justifyContent: 'center'
            },
            open ? {mr: 3} : {mr: 'auto'}
          ]}>
          {icon}
        </ListItemIcon>

        <ListItemText
          primary={text}
          sx={[open ? {opacity: 1} : {opacity: 0}]}
        />
      </ListItemButton>
    </ListItem>
  );
}
