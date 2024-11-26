import { useState } from 'react'
import Modal from '@mui/material/Modal'
import Backdrop from '@mui/material/Backdrop'
import Fade from '@mui/material/Fade'
import Box from '@mui/material/Box'
import Typography from '@mui/material/Typography'
import { Button, CircularProgress, Alert } from '@mui/material'
import ServerData from '../data/ServerData.ts'

const style = {
  position: 'absolute' as 'absolute',
  top: '50%',
  left: '50%',
  transform: 'translate(-50%, -50%)',
  width: 600,
  bgcolor: 'background.paper',
  border: '2px solid #000',
  boxShadow: 24,
  p: 4
}

interface Props {
  serverData: ServerData | null,
  onClose: () => void
}

export default function ModDownloadPanel (props: Props) {
  const { serverData, onClose } = props
  const [isGameStarted, setIsGameStarted] = useState(false)
  const [error, setError] = useState<string | null>(null)

  const createSymlink = async (gamePath: string) => {
    try {
      const result = await window.electronAPI.createModSymlink(gamePath)
      console.log(result)
    } catch (err: any) {
      throw new Error(`Failed to create symlink: ${err.message}`)
    }
  }

  const launchGame = async () => {
    if (serverData == null) return

    setIsGameStarted(true)
    setError(null)

    const params = ['--online', '--ip=' + serverData._ip, '--port=' + serverData._port]

    try {
      const gamePath = await window.electronAPI.getGamePath()

      await createSymlink(gamePath)

      await window.electronAPI.executeExe(gamePath, params, './', () => {
        setIsGameStarted(false)
        onClose()
      })
    } catch (err: any) {
      console.error('Error launching game:', err)
      setError(err.message || 'An error occurred while launching the game.')
      setIsGameStarted(false)
    }
  }

  return (
    <Modal
      aria-labelledby="transition-modal-title"
      open={serverData != null}
      onClose={() => {
        if (!isGameStarted) onClose()
      }}
      closeAfterTransition
      slots={{ backdrop: Backdrop }}
      slotProps={{
        backdrop: {
          timeout: 500
        }
      }}
    >
      <Fade in={serverData != null}>
        <Box sx={style}>
          <Typography id="transition-modal-title" variant="h6" component="h4">
            {isGameStarted ? 'Launching game...' : `Launch game for ${serverData?.name}`}
          </Typography>
          <Box sx={{ p: 2, display: 'flex', flexDirection: 'column' }} alignItems="center" justifyContent="center">
            {isGameStarted
              ? (
              <CircularProgress />
                )
              : (
              <>
                {error && <Alert severity="error" sx={{ mb: 2, width: '100%' }}>{error}</Alert>}
                <Box sx={{ display: 'flex', flexDirection: 'row' }}>
                  <Button onClick={launchGame} variant="contained" color="primary" sx={{ mr: 1 }}>Launch Game</Button>
                  <Button onClick={onClose} variant="outlined">Cancel</Button>
                </Box>
              </>
                )}
          </Box>
        </Box>
      </Fade>
    </Modal>
  )
}
