import Owadview from './components/Owadview.tsx'
import ServerList from './components/servers/ServerList.tsx'
import { Box, createTheme, CssBaseline, ThemeProvider } from '@mui/material'
import WindowFrame from './components/WindowFrame.tsx'
import { useState, useEffect } from 'react';

const darkTheme = createTheme({
  palette: {
    mode: 'dark'
  }
})

export default function App () {
  const [_selectedFilePath, setSelectedFilePath] = useState<string | null>(null);
  const [_error, setError] = useState<string>('');

  const handleSelectFile = async () => {
    try {
      const result = await window.electronAPI.selectGameDialog();
      if (result.canceled) {
        return;
      }

      const filePath = result.filePaths[0];

      if (filePath.endsWith('Cyberpunk2077.exe')) {
        setSelectedFilePath(filePath);
        setError('');
        window.electronAPI.setGamePath(filePath);
      } else {
        setError('Please select a file named "Cyberpunk2077.exe"');
      }
    } catch (err) {
      console.error('Error selecting file:', err);
      setError('Exception: "' + err + '"');
    }
  };

  useEffect(() => {
    
    // Load the saved file path from electron-store
    window.electronAPI.getGamePath().then((storedPath: any) => {
      if (storedPath && window.electronAPI.pathExists(storedPath)) {
        setSelectedFilePath(storedPath);
      } else {
        handleSelectFile();
      }
    });
  }, []);

  return (
    <ThemeProvider theme={darkTheme}>
      <CssBaseline/>
      <WindowFrame/>
      <Box sx={{ height: 'calc(100vh - 50px)', display: 'flex', flexDirection: 'column', pt: '50px' }}>
        <Box
          sx={{
            width: '100%',
            overflow: 'auto',
            flex: 1
          }}
        >
          <ServerList/>
        </Box>
        <button onClick={handleSelectFile}>Reselect Cyberpunk2077.exe</button>
        <Owadview style={{ width: '100%', height: '120px' }}/>
      </Box>
    </ThemeProvider>
  )
}
