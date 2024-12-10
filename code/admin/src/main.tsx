import React, {StrictMode} from 'react';
import * as MUI from '@mui/material';
import {createRoot} from 'react-dom/client';
import './index.css';
import App from './App.tsx';
import {BrowserRouter} from "react-router";
import 'systemjs';
import {useToasts} from "./Toast/ToastProvider.tsx";

// NOTE: declare external dependencies globally for plugins.
window.React = React;
window.MUI = MUI;
window.App = {
  useToasts: useToasts
};

createRoot(document.getElementById('root')!).render(
  <StrictMode>
    <BrowserRouter>
      <App/>
    </BrowserRouter>
  </StrictMode>,
);
