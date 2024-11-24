import Store from 'electron-store'
import { promises as fs } from 'fs'
import devtools from 'electron-devtools-installer'
import decompress from 'decompress'
import path from 'path'
import { fileURLToPath } from 'url'
import electron from 'electron'

let win = null

const { default: install, REACT_DEVELOPER_TOOLS } = devtools
const __filename = fileURLToPath(import.meta.url)
const __dirname = path.dirname(__filename)
const store = new Store()

const createWindow = () => {
  win = new electron.BrowserWindow({
    width: 1080,
    height: 850,
    frame: false,
    webPreferences: {
      webSecurity: process.env.ELECTRON_DEV !== '1',
      nodeIntegration: true,
      preload: (process.env.ELECTRON_DEV === '1') ? path.join(__dirname, 'preload.mjs') : path.join(process.cwd(), 'resources/preload.mjs')
    }
  })

  if (process.env.ELECTRON_DEV === '1') {
    win.loadURL('http://localhost:5173')
    win.webContents.openDevTools()
  } else {
    win.loadFile('./dist/index.html')
  }

  win.on('ready-to-show', () => {
  })
  win.on('close', function () {
  })
}

electron.app.whenReady().then(async () => {
  electron.ipcMain.handle('get-game-path', () => store.get('game_path'))
  electron.ipcMain.handle('set-game-path', (event, path) => store.set('game_path', path))
  electron.ipcMain.handle('get-user-path', () => electron.app.getPath('userData'))
  electron.ipcMain.handle('minimize-app', () => win.minimize())
  electron.ipcMain.handle('maximize-app', () => win.isMaximized() ? win.unmaximize() : win.maximize())
  electron.ipcMain.handle('close-app', () => electron.app.quit())
  electron.ipcMain.handle('decompress', async (event, src, dest) => {
    return decompress(src, dest)
  })
  electron.ipcMain.handle('select-game-dialog', async () => {
    const result = await electron.dialog.showOpenDialog({
      properties: ['openFile'],
      filters: [{
        name: 'Executable',
        extensions: ['exe']
      }],
      title: 'Select Cyberpunk2077.exe',
      message: 'Please select the Cyberpunk2077.exe file. This file is typically located in bin/x64 of the game\'s installation directory.'
    })
    return result
  })

  electron.ipcMain.handle('create-mod-symlink', async (event, gamePath) => {
    const sourceDir = path.join(path.dirname(path.dirname(electron.app.getAppPath())), 'mod')
    const targetDir = path.join(path.dirname(path.dirname(path.dirname(gamePath))), 'red4ext', 'plugins', 'zzzCyberpunkMP')

    console.log(`Creating symlink from ${sourceDir} to ${targetDir}`)

    try {
      await fs.unlink(targetDir)
      console.log(`Existing symlink at ${targetDir} has been deleted.`)
    } catch (error) {
      if (error.code !== 'ENOENT') {
        throw error
      }
    }

    try {
      await fs.mkdir(path.dirname(targetDir), { recursive: true })
      await fs.symlink(sourceDir, targetDir, 'junction')
      return `Symlink created from ${sourceDir} to ${targetDir}`
    } catch (error) {
      if (error.code === 'EEXIST') {
        return 'Symlink already exists'
      } else {
        throw error
      }
    }
  })

  const devTools = process.env.ELECTRON_DEV === '1'

  if (devTools) {
    try {
      await install(REACT_DEVELOPER_TOOLS, {
        loadExtensionOptions: {
          allowFileAccess: true
        },
        forceDownload: true
      })
    } catch (error) {
      console.log('Failed to install react-devtools: ', error)
    }
  }

  // createMenu()
  createWindow()
})
