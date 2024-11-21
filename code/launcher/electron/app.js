const electron = require('electron');
const path = require('path')
const decompress = require('decompress')
const Store = require('electron-store')
const fs = require('fs').promises;

let win = null

const store = new Store();

const createWindow = () => {
  win = new electron.BrowserWindow({
    width: 1080,
    height: 850,
    frame: false,
    webPreferences: {
      nodeIntegration: true,
     // preload: path.join(__dirname, 'preload.js') // for development
      preload: path.join(process.cwd(), 'resources/preload.js') // for production
    }
  })

  win.loadFile('./dist/index.html')

  win.on('ready-to-show', () => {
  })
  win.on('close', function () {
  })
}

electron.app.whenReady().then(() => {
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
      filters: [{ name: 'Executable', extensions: ['exe'] }],
      title: 'Select Cyberpunk2077.exe',
      message: 'Please select the Cyberpunk2077.exe file. This file is typically located in bin/x64 of the game\'s installation directory.'
    });
    return result;
  });

  electron.ipcMain.handle('create-mod-symlink', async (event, gamePath) => {
    const sourceDir = path.join(path.dirname(path.dirname(electron.app.getAppPath())), 'mod');
    const targetDir = path.join(path.dirname(path.dirname(path.dirname(gamePath))), 'red4ext', 'plugins', 'zzzCyberpunkMP');

    console.log(`Creating symlink from ${sourceDir} to ${targetDir}`);
  
    try {
      await fs.unlink(targetDir);
      console.log(`Existing symlink at ${targetDir} has been deleted.`);
    } catch (error) {
      if (error.code !== 'ENOENT') {
        throw error;
      }
    }

    try {
      await fs.mkdir(path.dirname(targetDir), { recursive: true });
      await fs.symlink(sourceDir, targetDir, 'junction');
      return `Symlink created from ${sourceDir} to ${targetDir}`;
    } catch (error) {
      if (error.code === 'EEXIST') {
        return 'Symlink already exists';
      } else {
        throw error;
      }
    }
  });

  const devTools = false
  if (devTools) {
    // install([REACT_DEVELOPER_TOOLS], { allowFileAccess: true })
    //   .then((name) => console.log(`Added Extension:  ${name}`))
    //   .catch((err) => console.log('An error occurred: ', err))
  }

  // createMenu()
  createWindow()
})
