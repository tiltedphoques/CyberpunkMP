import IconButton from "@mui/material/IconButton";
import {theme} from "../Theme.ts";
import ChevronRightIcon from "@mui/icons-material/ChevronRight";
import ChevronLeftIcon from "@mui/icons-material/ChevronLeft";
import Divider from "@mui/material/Divider";
import List from "@mui/material/List";
import DrawerItem from "./DrawerItem.tsx";
import {
  Dashboard as DashboardIcon,
  Extension as ExtensionIcon,
  Info as InfoIcon,
  Settings as SettingsIcon
} from "@mui/icons-material";
import {CSSObject, styled, Theme} from "@mui/material/styles";
import MuiDrawer from "@mui/material/Drawer";
import {ToolbarProps} from "../App.tsx";
import {createElement, Fragment} from "react";

export const drawerWidth = 240;

const openedMixin = (theme: Theme): CSSObject => ({
  width: drawerWidth,
  transition: theme.transitions.create('width', {
    easing: theme.transitions.easing.sharp,
    duration: theme.transitions.duration.enteringScreen,
  }),
  overflowX: 'hidden',
});

const closedMixin = (theme: Theme): CSSObject => ({
  transition: theme.transitions.create('width', {
    easing: theme.transitions.easing.sharp,
    duration: theme.transitions.duration.leavingScreen,
  }),
  overflowX: 'hidden',
  width: `calc(${theme.spacing(7)} + 1px)`,
  [theme.breakpoints.up('sm')]: {
    width: `calc(${theme.spacing(8)} + 1px)`,
  },
});

const _Drawer = styled(MuiDrawer, {shouldForwardProp: (prop) => prop !== 'open'})(
  ({theme}) => ({
    width: drawerWidth,
    flexShrink: 0,
    whiteSpace: 'nowrap',
    boxSizing: 'border-box',
    variants: [
      {
        props: ({open}) => open,
        style: {
          ...openedMixin(theme),
          '& .MuiDrawer-paper': openedMixin(theme),
        },
      },
      {
        props: ({open}) => !open,
        style: {
          ...closedMixin(theme),
          '& .MuiDrawer-paper': closedMixin(theme),
        },
      },
    ],
  }),
);

const _DrawerHeader = styled('div')(({theme}) => ({
  display: 'flex',
  alignItems: 'center',
  justifyContent: 'flex-end',
  padding: theme.spacing(0, 1),
  // necessary for content to be below app bar
  ...theme.mixins.toolbar,
}));

export default function Drawer({open, onCloseDrawer}: ToolbarProps) {
  const sections = [
    [
      {text: 'Dashboard', icon: DashboardIcon, route: '/'},
      {text: 'Plugins', icon: ExtensionIcon, route: '/plugins'},
    ],
    [
      {text: 'Settings', icon: SettingsIcon, route: '/settings'},
      {text: 'About', icon: InfoIcon, route: '/about'},
    ],
  ];

  return (
    <_Drawer variant="permanent" open={open}>
      <_DrawerHeader>
        <IconButton onClick={onCloseDrawer}>
          {theme.direction === 'rtl' ? <ChevronRightIcon/> : <ChevronLeftIcon/>}
        </IconButton>
      </_DrawerHeader>

      <Divider/>

      {sections.map((section, i) => (
        <Fragment key={i}>
          <List>
            {section.map((item, j) => (
              <DrawerItem key={j}
                          open={open}
                          route={item.route}
                          text={item.text}
                          icon={createElement(item.icon)}
              />
            ))}
          </List>
          {(i + 1) < sections.length && <Divider/>}
        </Fragment>
      ))}
    </_Drawer>
  );
}
