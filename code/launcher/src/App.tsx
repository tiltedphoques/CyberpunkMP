import ServerList from './components/servers/ServerList.tsx'
import { Box, Button, createTheme, CssBaseline, ThemeProvider } from '@mui/material'
import WindowFrame from './components/WindowFrame.tsx'
import { useEffect, useState } from 'react'
import { FindInPage } from '@mui/icons-material'

const darkTheme = createTheme({
  palette: {
    mode: 'dark'
  },
  components: {
    // Actions & Inputs
    MuiButton: {
      styleOverrides: {
        root: {
          lineHeight: 'normal'
        }
      }
    },
    MuiTextField: {
      styleOverrides: {
        root: {
          width: '100%',
          margin: '12px 0'
        }
      }
    },
    // Display data
    MuiDialogContent: {
      styleOverrides: {
        root: {
          paddingTop: '12px!important'
        }
      }
    }
  }
})

export default function App () {
  const [_selectedFilePath, setSelectedFilePath] = useState<string | null>(null)
  const [_error, setError] = useState<string>('')

  const handleSelectFile = async () => {
    try {
      const result = await window.electronAPI.selectGameDialog()
      if (result.canceled) {
        return
      }

      const filePath = result.filePaths[0]

      if (filePath.endsWith('Cyberpunk2077.exe')) {
        setSelectedFilePath(filePath)
        setError('')
        window.electronAPI.setGamePath(filePath)
      } else {
        setError('Please select a file named "Cyberpunk2077.exe"')
      }
    } catch (err) {
      console.error('Error selecting file:', err)
      setError('Exception: "' + err + '"')
    }
  }

  useEffect(() => {
    // Load the saved file path from electron-store
    window.electronAPI.getGamePath().then((storedPath: any) => {
      if (storedPath && window.electronAPI.pathExists(storedPath)) {
        setSelectedFilePath(storedPath)
      } else {
        handleSelectFile()
      }
    })
  }, [])

  return (
    <ThemeProvider theme={darkTheme}>
      <CssBaseline/>
      <WindowFrame/>
      <Box sx={{ display: 'flex', flexDirection: 'column', p: '12px' }}>
        <Box sx={{ display: 'flex', pt: '50px', mb: '12px' }}>
          <Button variant="outlined"
                  startIcon={<FindInPage />}
                  onClick={handleSelectFile}
          >
            Select game executable
          </Button>
        </Box>

        <Box sx={{ width: '100%', overflow: 'auto', flex: 1 }}>
          <ServerList/>
        </Box>

        { /* <Owadview style={{ width: '100%', height: '120px' }}/> */ }
      </Box>
    </ThemeProvider>
  )
}
