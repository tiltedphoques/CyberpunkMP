# How to build

## Prerequisites
- [Node.js](https://nodejs.org/en/)
- [PNPM](https://pnpm.io/)

> [!TIP]
> **pnpm** is used instead of **npm** to save disk space on your system. This 
> is particularly useful as multiple projects in this repository are using the 
> same dependencies.

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
pnpm install
cd electron/
pnpm install
cd ..
```

> [!WARNING]
> On Windows, depending on where you installed this repository, long path might
> be an issue when trying to build electron app. You can add `electron/.npmrc`
> with this line `virtual-store-dir-max-length=50` to fix it. You might need to
> change this value based on your environment. See official [documentation] for 
> more.

[documentation]: https://pnpm.io/npmrc#virtual-store-dir

## Development
1. Run `pnpm run start:all` to concurrently run:
  - React with vite
  - react-devtools for debugging
  - electron app with overwolf
2. `CTRL + C` or exit any of these programs to kill them all.

> [!NOTE]
> You can manually start each process in three terminals using `pnpm run start`, 
> `pnpm run start:electron` and `npx react-devtools`.

> [!WARNING]
> If `react-devtools` is globally installed on your system, it will likely fail 
> to work when using command `start:all`. You can either uninstall global 
> dependency, or run commands independently. 

## Production
1. Go through `Development` first.
2. Run `pnpm run build:all` to build:
  - React app
  - Electron app
3. Output will be in the `electron/out` folder.
4. You are ready to run `CyberpunkMP.exe` in `win-unpacked/`!
