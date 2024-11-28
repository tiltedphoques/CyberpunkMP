import ServerData from '../../data/ServerData.ts'
import { MouseEvent } from 'react'
import { Box, TableCell, TableRow, TableSortLabel } from '@mui/material'
import { visuallyHidden } from '@mui/utils'

interface HeadCell {
  disablePadding: boolean;
  id: keyof ServerData;
  label: string;
  head: boolean;
  width?: string;
}

type Order = 'asc' | 'desc';

const headCells: readonly HeadCell[] = [
  {
    id: 'name',
    head: true,
    disablePadding: true,
    label: 'Server Name',
  },
  {
    id: 'players',
    head: false,
    disablePadding: false,
    label: 'Players',
    width: '112px',
  },
  {
    id: 'tags',
    head: false,
    disablePadding: false,
    label: 'Tags'
  },
  {
    id: 'ip',
    head: false,
    disablePadding: false,
    label: 'Address',
    width: '190px',
  },
  {
    id: 'version',
    head: false,
    disablePadding: false,
    label: 'Version',
    width: '112px',
  },
  {
    id: 'favorite',
    head: false,
    disablePadding: false,
    label: 'Favorite',
    width: '112px',
  }
]

interface HeadRowProps {
  onRequestSort: (event: MouseEvent<HTMLSpanElement>, property: keyof ServerData) => void;
  order: Order;
  orderBy: string;
  rowCount: number;
}

export default function HeadRow (props: HeadRowProps) {
  const { order, orderBy, onRequestSort } = props

  return (
    <TableRow>
      {headCells.map((headCell) => (
        <TableCell
          key={headCell.id}
          sx={{ p: 2 }}
          width={headCell.width}
          align={headCell.head ? 'left' : 'right'}
          padding={headCell.disablePadding ? 'none' : 'normal'}
          sortDirection={orderBy === headCell.id ? order : false}
        >
          <TableSortLabel
            active={orderBy === headCell.id}
            direction={orderBy === headCell.id ? order : 'asc'}
            onClick={event => onRequestSort(event, headCell.id)}
          >
            {headCell.label}
            {orderBy === headCell.id
              ? (
                <Box component="span" sx={visuallyHidden}>
                  {order === 'desc' ? 'sorted descending' : 'sorted ascending'}
                </Box>
                )
              : null}
          </TableSortLabel>
        </TableCell>
      ))}
    </TableRow>
  )
}
