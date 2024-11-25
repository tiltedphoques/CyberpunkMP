# How to build

## Prerequisites
- [Node.js](https://nodejs.org/en/)
- [NPM](https://www.npmjs.com/)

1. Clone the repository:
```shell
git clone https://github.com/tiltedphoques/CyberpunkMP.git
```
2. Go to launcher directory:
```shell
cd code/launcher
```
3. Install dependencies:
```shell
npm install
cd electron/
npm install
cd ..
```

## Development
1. Run `npm run start:all` to concurrently run:
  - React with vite
  - react-devtools for debugging
  - electron app with overwolf
2. `CTRL + C` or exit any of these programs to kill them all.

> [!NOTE]
> You can manually start each process in three terminals using `npm run start`, 
> `npm run start:electron` and `npx react-devtools`.

> [!WARNING]
> If `react-devtools` is globally installed on your system, it will likely fail 
> to work when using command `start:all`. You can either uninstall global 
> dependency, or run commands independently. 

## Production
1. Go through `Development` first.
2. Run `npm run build:all` to build:
  - React app
  - Electron app
3. Output will be in the `electron/out` folder.
4. You are ready to run `CyberpunkMP.exe` in `win-unpacked/`!
