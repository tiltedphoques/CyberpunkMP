import {
  Box, Button, Paper, Table, TableBody, TableCell, TableContainer, TableFooter,
  TablePagination, TableRow, TextField
} from '@mui/material'
import { ChangeEvent, MouseEvent, useEffect, useMemo, useState } from 'react'
import ServerData from '../../data/ServerData.ts'
import HeadRow from './HeadRow.tsx'
import ServerRow from './ServerRow.tsx'
import ModDownloadPanel from '../ModDownloadPanel.tsx'
import RefreshIcon from '@mui/icons-material/Refresh'

type Order = 'asc' | 'desc';

// const modsList: ModData[] = [
//   {
//     hash: '84a42d208ca22ec3b80e4fd6641ea768',
//     downloadUrl: 'https://edge.forgecdn.net/files/5113/128/ultra_mod_alfa_1.19.2.jar',
//     name: 'Ultra Mod'
//   },
//   {
//     hash: 'd76853d6611e7de040491f2f0cf494ae',
//     downloadUrl: 'https://edge.forgecdn.net/files/2954/474/pitg-1.15.2-1.3.0.jar',
//     name: 'Pitg'
//   },
//   {
//     hash: 'b9177bd5ec008991768d0ba3247bf625',
//     downloadUrl: 'https://edge.forgecdn.net/files/5155/575/100%20days%20in%20a%20Zombie%20Apocalypse.zip',
//     name: '100 days in a Zombie Apocalypse'
//   },
//   {
//     hash: '8e6b21b0d837d03ca7f490fcfb4e75b5',
//     downloadUrl: 'https://edge.forgecdn.net/files/2925/387/Healcraft1.0.0Beta.jar',
//     name: 'Healcraft'
//   }]

function descendingComparator<T> (a: T, b: T, orderBy: keyof T) {
  return a[orderBy] < b[orderBy] ? 1 : -1
}

function getComparator<Key extends keyof any> (
  order: Order,
  orderBy: Key
): (
  a: { [key in Key]: number | string | any },
  b: { [key in Key]: number | string | any }
) => number {
  return order === 'desc'
    ? (a, b) => descendingComparator(a, b, orderBy)
    : (a, b) => -descendingComparator(a, b, orderBy)
}

function stableSort<T> (array: readonly T[], comparator: (a: T, b: T) => number) {
  const stabilizedThis = array.map((el, index) => [el, index] as [T, number])
  stabilizedThis.sort((a, b) => {
    const order = comparator(a[0], b[0])
    if (order !== 0) {
      return order
    }
    return a[1] - b[1]
  })
  return stabilizedThis.map((el) => el[0])
}

const ipv6Regex = /^(?:(?:[a-fA-F\d]{1,4}:){7}(?:[a-fA-F\d]{1,4}|:)|(?:[a-fA-F\d]{1,4}:){6}(?:(?:25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)(?:\\.(?:25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)){3}|:[a-fA-F\d]{1,4}|:)|(?:[a-fA-F\d]{1,4}:){5}(?::(?:25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)(?:\\.(?:25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)){3}|(?::[a-fA-F\d]{1,4}){1,2}|:)|(?:[a-fA-F\d]{1,4}:){4}(?:(?::[a-fA-F\d]{1,4}){0,1}:(?:25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)(?:\\.(?:25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)){3}|(?::[a-fA-F\d]{1,4}){1,3}|:)|(?:[a-fA-F\d]{1,4}:){3}(?:(?::[a-fA-F\d]{1,4}){0,2}:(?:25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)(?:\\.(?:25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)){3}|(?::[a-fA-F\d]{1,4}){1,4}|:)|(?:[a-fA-F\d]{1,4}:){2}(?:(?::[a-fA-F\d]{1,4}){0,3}:(?:25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)(?:\\.(?:25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)){3}|(?::[a-fA-F\d]{1,4}){1,5}|:)|(?:[a-fA-F\d]{1,4}:){1}(?:(?::[a-fA-F\d]{1,4}){0,4}:(?:25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)(?:\\.(?:25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)){3}|(?::[a-fA-F\d]{1,4}){1,6}|:)|(?::(?:(?::[a-fA-F\d]{1,4}){0,5}:(?:25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)(?:\\.(?:25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)){3}|(?::[a-fA-F\d]{1,4}){1,7}|:)))(?:%[0-9a-zA-Z]{1,})?$/gm

export default function ServerList () {
  const [servers, setServers] = useState<ServerData[]>([
  ])
  const [order, setOrder] = useState<Order>('asc')
  const [orderBy, setOrderBy] = useState<keyof ServerData>('name')
  const [page, setPage] = useState(0)
  const [rowsPerPage, setRowsPerPage] = useState(15)
  const [filter, setFilter] = useState<string>('')
  const [filteredCount, setFilteredCount] = useState<number>(0)

  const [openedServerModal, setOpenedServerModal] = useState<ServerData | null>(null)

  const [shouldQuery, setShouldQuery] = useState(true)

  useEffect(() => {
    if (!shouldQuery) {
      return
    }
    const ipToFavorite = new Map<string, number>()
    const favoriteServers = window.localStorage.getItem('favoriteServers')
    if (favoriteServers) {
      JSON.parse(favoriteServers).forEach((entry: [string, number]) => {
        ipToFavorite.set(entry[0], entry[1])
      })
    }
    fetch('https://cyberpunk.skyrim-together.com/list')
      .then(res => res.json())
      .then(data => {
        const newServers: ServerData[] = data.servers.map((server: any) : ServerData => {
          const fav = ipToFavorite.get(`${server.ip}:${server.port}`)
          const isIpv6 = ipv6Regex.test(server.ip)
          return {
            name: server.name,
            players: `${server.player_count}/${server.max_player_count}`,
            tags: server.tags,
            ip: `${server.ip}:${server.port}`,
            _ip: isIpv6 ? `[${server.ip}]` : server.ip,
            _port: server.port,
            version: server.version,
            modsEndpoint: isIpv6
              ? `http://[${server.ip}]:${server.port}/api/v1/mods/`
              : `http://${server.ip}:${server.port}/api/v1/mods/`,
            favorite: fav ?? 0
          }
        })
        setServers(newServers)
        setShouldQuery(false)
      })
  }, [shouldQuery])

  const handleRequestSort = (
    _: MouseEvent,
    property: keyof ServerData
  ) => {
    const isAsc = orderBy === property && order === 'asc'
    setOrder(isAsc ? 'desc' : 'asc')
    setOrderBy(property)
  }

  const handleChangePage = (_: unknown, newPage: number) => {
    setPage(newPage)
  }

  const handleChangeRowsPerPage = (event: ChangeEvent<HTMLInputElement>) => {
    setRowsPerPage(parseInt(event.target.value, 10))
    setPage(0)
  }

  // Avoid a layout jump when reaching the last page with empty rows.
  const emptyRows =
      page > 0 ? Math.max(0, (1 + page) * rowsPerPage - servers.length) : 0

  useEffect(() => {
    setPage(0)
    setFilteredCount(servers.filter(x => x.name.toLowerCase().includes(filter)).length)
  }, [filter, servers])

  const visibleRows = useMemo(
    () =>
      stableSort(servers.filter(x => x.name.toLowerCase().includes(filter)), getComparator(order, orderBy)).slice(
        page * rowsPerPage,
        page * rowsPerPage + rowsPerPage
      ),
    [order, orderBy, page, rowsPerPage, servers, filter]
  )

  useEffect(() => {
    const ipToFavorite = new Map<string, number>()
    servers.forEach(server => {
      ipToFavorite.set(server.ip, server.favorite)
    })

    window.localStorage.setItem('favoriteServers', JSON.stringify(Array.from(ipToFavorite.entries())))
  }, [servers])

  return (
    <div>
      <Box sx={{ width: 'calc(100% - 20px)', m: '10px' }}>
        <Paper sx={{ width: '100%', mb: 2 }}>
          <TableContainer sx={{ height: '80%', overflow: 'hidden' }}>
            <Table
              sx={{ minWidth: 750, tableLayout: 'fixed' }}
              aria-labelledby="tableTitle"
              size='medium'
            >
              <HeadRow
                order={order}
                orderBy={orderBy}
                onRequestSort={handleRequestSort}
                rowCount={servers.length}
              />
              <TableBody>
                {visibleRows
                  .map((row, index) =>
                    <ServerRow
                      key={index}
                      row={row}
                      index={index}
                      setServers={setServers}
                      onStart={(_: ServerData) => {
                        setOpenedServerModal(row)
                      }}
                    />
                  )}
                {emptyRows > 0 && (
                  <TableRow
                    style={{
                      height: (53) * emptyRows
                    }}
                  >
                    <TableCell colSpan={6} />
                  </TableRow>
                )}
              </TableBody>
            </Table>
          </TableContainer>
          <TableFooter sx={{ display: 'flex', justifyContent: 'space-between', pl: 2 }}>
            <Box sx={{ display: 'flex', justifyContent: 'center', alignItems: 'center' }}>
              <TextField
                id="standard-basic"
                label="Filter"
                variant="standard"
                onChange={(event: ChangeEvent<HTMLInputElement>) => {
                  setFilter(event.target.value.toLowerCase())
                }}
              />
              <Button onClick={() => setShouldQuery(true)}>
                <RefreshIcon/>
              </Button>
            </Box>
            <TablePagination
              rowsPerPageOptions={[rowsPerPage]}
              component="div"
              count={filteredCount}
              rowsPerPage={rowsPerPage}
              page={page}
              onPageChange={handleChangePage}
              onRowsPerPageChange={handleChangeRowsPerPage}
            />
          </TableFooter>
        </Paper>
      </Box>
      <ModDownloadPanel
        onClose={() => {
          setOpenedServerModal(null)
        }}
        serverData={openedServerModal}
      />
    </div>
  )
}
