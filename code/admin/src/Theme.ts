import {createTheme} from "@mui/material";
import {blue, orange} from "@mui/material/colors";

export const theme = createTheme({
  cssVariables: {
    colorSchemeSelector: 'class'
  },
  colorSchemes: {
    light: {
      palette: {
        primary: blue,
        secondary: orange,
      },
    },
    dark: {
      palette: {
        primary: blue,
        secondary: orange,
      },
    },
  },
  components: {
    MuiCard: {
      styleOverrides: {
        root: {
          padding: '24px'
        }
      },
    },
  },
});
