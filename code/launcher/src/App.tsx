import ServerList from './components/servers/ServerList.tsx'
import { Alert, Box, Button, createTheme, CssBaseline, Snackbar, ThemeProvider } from '@mui/material'
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
  const [error, setError] = useState<string | null>(null)
  const [warning, setWarning] = useState<string | null>(null)
  const [success, setSuccess] = useState<string | null>(null)

  const handleSelectFile = async () => {
    try {
      const result = await window.electronAPI.selectGameDialog()
      if (result.canceled) {
        return
      }

      const filePath = result.filePaths[0]

      if (!filePath.endsWith('Cyberpunk2077.exe')) {
        setError('You must select the file "Cyberpunk2077.exe".')
        return
      }
      setSelectedFilePath(filePath)
      setSuccess('Game file detected, you\'re ready.')
      window.electronAPI.setGamePath(filePath)
    } catch (err) {
      console.error('Error selecting file:', err)
      setError(`Error selecting file: ${JSON.stringify(err)}`)
    }
  }

  useEffect(() => {
    // Load the saved file path from electron-store
    window.electronAPI.getGamePath().then((gamePath?: string) => {
      if (gamePath && window.electronAPI.pathExists(gamePath)) {
        setSelectedFilePath(gamePath)
        return
      }
      const knownPaths: string[] = [
        'C:\\Program Files (x86)\\Steam\\steamapps\\common\\Cyberpunk 2077\\bin\\x64\\Cyberpunk2077.exe',
        'C:\\Program Files (x86)\\GOG Galaxy\\Games\\Cyberpunk 2077\\bin\\x64\\Cyberpunk2077.exe',
        'C:\\Program Files\\Epic Games\\Cyberpunk 2077\\bin\\x64\\Cyberpunk2077.exe'
      ]

      gamePath = knownPaths.find((path) => window.electronAPI.pathExists(path))
      if (gamePath) {
        window.electronAPI.setGamePath(gamePath)
        setSelectedFilePath(gamePath)
        setSuccess('Game file auto-detected, you\'re ready.')
      } else {
        setWarning('Could not auto-detect where your game is installed.')
        setTimeout(() => handleSelectFile(), 4000)
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

      <Snackbar open={error != null}
                onClose={() => {
                  setError(null)
                }}
                autoHideDuration={4000}>
        <Alert
          severity="error"
          variant="outlined"
          sx={{ width: '100%' }}
        >
          { error }
        </Alert>
      </Snackbar>

      <Snackbar open={warning != null}
                onClose={() => setWarning(null)}
                autoHideDuration={4000}>
        <Alert
          severity="warning"
          variant="outlined"
          sx={{ width: '100%' }}
        >
          { warning }
        </Alert>
      </Snackbar>

      <Snackbar open={success != null}
                onClose={() => setSuccess(null)}
                autoHideDuration={4000}>
        <Alert
          severity="success"
          variant="outlined"
          sx={{ width: '100%' }}
        >
          { success }
        </Alert>
      </Snackbar>
    </ThemeProvider>
  )
}
