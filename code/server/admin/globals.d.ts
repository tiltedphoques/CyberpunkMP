import {App} from "./api";

declare global {
  interface Window {
    MUI: typeof import('@mui/material');
    MDI: typeof import('@mdi/react');
    Redux: typeof import('react-redux');
    App: App;
  }
}

export {};
