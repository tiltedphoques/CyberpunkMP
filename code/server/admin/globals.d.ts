import {useToasts} from "./src/Stores/ToastStore.ts";

declare global {
  interface Window {
    Redux: typeof import('react-redux');
    MUI: typeof import('@mui/material');
    MUIIcons: typeof import('@mui/icons-material');
    MDI: typeof import('@mdi/react');
    MDIIcons: typeof import('@mdi/js');
    StatusCodes: typeof import('http-status-codes');
    App: {
      useToasts: typeof useToasts;
    }
  }
}

export {};
