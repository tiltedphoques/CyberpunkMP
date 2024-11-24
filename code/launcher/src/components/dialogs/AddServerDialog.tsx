import * as React from 'react'
import Button from '@mui/material/Button'
import TextField from '@mui/material/TextField'
import Dialog from '@mui/material/Dialog'
import DialogActions from '@mui/material/DialogActions'
import DialogContent from '@mui/material/DialogContent'
import DialogTitle from '@mui/material/DialogTitle'
import { useState } from 'react'
import ServerData from '../../data/ServerData.ts'
import { Box } from '@mui/material'

interface Props {
  open: boolean;
  onClose: (serverData?: ServerData) => void
}

export default function AddServerDialog (props: Props) {
  const { open, onClose } = props

  const [formServerName, setServerName] = useState<string>('')
  const [formIpAddress, setIpAddress] = useState<string>('')
  const [formPort, setPort] = useState<number>(11778)
  const [formPassword, setPassword] = useState<string>('')

  const cannotSubmit = () => {
    return formServerName.length === 0 || formIpAddress.length === 0 || formPort < 1024
  }

  const handleClose = () => {
    onClose()
  }

  const handleSubmit = (event: React.FormEvent<HTMLFormElement>) => {
    event.preventDefault()
    if (cannotSubmit()) {
      return
    }
    onClose({
      name: formServerName,
      players: '0/32',
      tags: '',
      ip: `${formIpAddress}:${formPort}`,
      _ip: formIpAddress,
      _port: formPort.toString(),
      version: '',
      favorite: 0,
      modsEndpoint: `https://${formIpAddress}:${formPort}/api/v1/mods/`
    })
  }

  return (
    <React.Fragment>
      <Dialog
        open={open}
        onClose={handleClose}
        PaperProps={{ component: 'form', onSubmit: handleSubmit }}
      >
        <DialogTitle>Add private server</DialogTitle>

        <DialogContent sx={{ flexFlow: 'row' }}>
          <TextField label="Server name"
                     required
                     value={formServerName}
                     onChange={(event) => setServerName(event.target.value)}
          />

          <Box sx={{ display: 'flex' }}>
            <TextField sx={{ flex: 3, mr: '12px' }}
                       label="IP address"
                       required
                       value={formIpAddress}
                       onChange={(event) => setIpAddress(event.target.value)}
            />

            <TextField sx={{ flex: 1, minWidth: '96px' }}
                       label="Port"
                       type="number"
                       required
                       value={formPort}
                       onChange={(event) => setPort(+event.target.value)}
            />
          </Box>

          <TextField label="Password"
                     type="password"
                     value={formPassword}
                     onChange={(event) => setPassword(event.target.value)}
          />
        </DialogContent>

        <DialogActions>
          <Button onClick={handleClose}>Cancel</Button>
          <Button color="success"
                  type="submit"
                  disabled={cannotSubmit()}>
            Add
          </Button>
        </DialogActions>
      </Dialog>
    </React.Fragment>
  )
}
