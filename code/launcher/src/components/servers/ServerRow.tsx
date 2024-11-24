import { Box, IconButton, TableCell, TableRow } from '@mui/material'
import { produce } from 'immer'
import FavoriteIcon from '@mui/icons-material/Favorite'
import FavoriteIconEmpty from '@mui/icons-material/FavoriteBorder'
import ServerData from '../../data/ServerData.ts'
import { SetStateAction } from 'react'

interface Props {
  row: ServerData,
  index: number,
  setServers: (value: SetStateAction<ServerData[]>) => void,
  onStart: (server: ServerData) => void
}

export default function ServerRow (props: Props) {
  const { row, index, setServers, onStart } = props
  const labelId = `enhanced-table-checkbox-${index}`
  return (
    <TableRow
      hover
      tabIndex={-1}
      key={row.ip}
    >
      <TableCell onClick={() => onStart(row)} sx={{ p: 2, cursor: 'pointer' }} component="th" id={labelId} scope="row" padding="none" width='25%'><Box className="textContainer">{row.name}</Box></TableCell>
      <TableCell onClick={() => onStart(row)} sx={{ p: 2, cursor: 'pointer' }} align="right" width='15%'><Box className="textContainer">{row.players}</Box></TableCell>
      <TableCell onClick={() => onStart(row)} sx={{ p: 2, cursor: 'pointer' }} align="right" width='15%'><Box className="textContainer">{row.tags}</Box></TableCell>
      <TableCell onClick={() => onStart(row)} sx={{ p: 2, cursor: 'pointer' }} align="right" width='15%'><Box className="textContainer">{row.ip}</Box></TableCell>
      <TableCell onClick={() => onStart(row)} sx={{ p: 2, cursor: 'pointer' }} align="right" width='15%'><Box className="textContainer">{row.version}</Box></TableCell>
      <TableCell sx={{ p: 2 }} align="right" width='15%'>
        <Box className="textContainer">
          <IconButton sx={{ zIndex: 10 }}
                      color="primary"
                      onClick={() => {
                        setServers(produce(lastServers => {
                          lastServers.forEach((lastServer) => {
                            if (row.ip === lastServer.ip) {
                              lastServer.favorite = +!lastServer.favorite
                            }
                          })
                        }))
                      }}>
            {row.favorite ? <FavoriteIcon/> : <FavoriteIconEmpty/>}
          </IconButton>
        </Box>
      </TableCell>
    </TableRow>
  )
}
