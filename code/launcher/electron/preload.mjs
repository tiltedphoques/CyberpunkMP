import { contextBridge, ipcRenderer } from 'electron'

import fs from 'fs'
import path from 'path'
import { execFile as exec } from 'child_process'

export function getRemoteFile (file, url, onFinish) {
  fetch(url).then(async response => {
    const tempPath = file + '.tmp'
    const hash = path.parse(file).name
    const localFile = fs.createWriteStream(tempPath)
    response.arrayBuffer().then(arrayBuffer => {
      localFile.write(Buffer.from(arrayBuffer))
    }).finally(() => {
      localFile.close(() => {
        const dirPath = path.join(path.dirname(tempPath), hash)
        if (!fs.existsSync(dirPath)) fs.mkdirSync(dirPath)
        ipcRenderer.invoke('decompress', tempPath, dirPath).then(() => {
          fs.unlink(tempPath, () => {})
          onFinish()
        }).catch(err => {
          if (err) onFinish(err)
        })
      })
    })
  })
}

export async function selectGameDialog () {
  return await ipcRenderer.invoke('select-game-dialog')
}

export function getGamePath () {
  return ipcRenderer.invoke('get-game-path')
}

export function setGamePath (path) {
  ipcRenderer.invoke('set-game-path', path)
}

export async function _getModPath () {
  const userPath = await ipcRenderer.invoke('get-user-path')
  return path.join(userPath, 'downloads', 'mods')
}

export async function isModInstalled (hash) {
  const modPath = await _getModPath()
  const filePath = path.join(modPath, hash)
  return fs.existsSync(filePath)
}

contextBridge.exposeInMainWorld('electronAPI', {
  writeFile: async (path, content) => {
    await fs.writeFile(path, content, err => {
      if (err) console.error(err)
    })
  },
  pathExists: (path) => {
    return fs.existsSync(path)
  },
  readFile: async (path) => {
    let out = ''
    await fs.readFile(path, 'utf8', (err, data) => {
      if (err) console.error(err)
      out = data
    })
    return out
  },
  deleteFile: (path) => {
    fs.unlink(path, () => {})
  },
  downloadFile: async (url, hash, onFinish) => {
    const modPath = await _getModPath()
    const filePath = path.join(modPath, hash)
    const dirPath = path.dirname(filePath)
    if (!fs.existsSync(dirPath)) {
      fs.mkdirSync(dirPath, { recursive: true })
    }
    getRemoteFile(filePath, url, onFinish)
    // const file = fs.createWriteStream(filePath)
    // console.log('after create write stream')
    // const request = https.get(url, (response) => {
    //   response.pipe(file)
    //   file.on('finish', function () {
    //     file.close(onFinish)
    //   })
    //   response.on('data', (chunk) => {
    //     console.log('downloading chunk of size: ' + chunk.length)
    //   })
    // })
    //
    // request.on('error', (err) => {
    //   fs.unlink(filePath, () => {})
    //   onFinish(err)
    // })
  },
  createModSymlink: (gamePath) => ipcRenderer.invoke('create-mod-symlink', gamePath),
  checkModList: async (list) => {
    const out = new Map()
    for (const mod of list) {
      out.set(mod, await isModInstalled(mod))
    }
    return out
  },
  selectGameDialog: selectGameDialog,
  getGamePath: getGamePath,
  setGamePath: setGamePath,
  closeApp: () => {
    ipcRenderer.invoke('close-app')
  },
  minimizeApp: () => {
    ipcRenderer.invoke('minimize-app')
  },
  maximizeApp: () => {
    ipcRenderer.invoke('maximize-app')
  },
  executeExe: (fileName, params, path, onExit) => {
    return new Promise((resolve, reject) => {
      const child = exec(fileName, params, { cwd: path }, (err, data) => {
        if (err) reject(err)
        else resolve(data)
      })
      child.on('exit', onExit)
    })
  },
  getModsFiles: async (modsHashes) => {
    const modPath = await _getModPath()
    const out = []
    for (const hash of modsHashes) {
      const filePath = path.join(modPath, hash)
      out.push(
        fs.readdirSync(filePath, { withFileTypes: true, recursive: true })
          .filter(item => item.isFile())
          .map(dirent => dirent.path)
      )
    }
    return Array.from(new Set(out.flat()))
  },
  isProcessRunning: async (processName) => {
    const cmd = (() => {
      switch (process.platform) {
        case 'win32': return 'tasklist'
        case 'darwin': return `ps -ax | grep ${processName}`
        case 'linux': return 'ps -A'
        default: return false
      }
    })()

    if (!cmd) {
      return false
    }

    return new Promise((resolve, reject) => {
      require('child_process').exec(cmd, (err, stdout) => {
        if (err) reject(err)

        resolve(stdout.toLowerCase().indexOf(processName.toLowerCase()) > -1)
      })
    })
  }
})
