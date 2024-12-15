# Admin Panel

This webapp provides an admin panel to manage the server. It is made to allow
extensions using widgets provided by plugins themselves.

## Features with WebApi
- secure access using basic authentication (credentials through environment variables).
  - always allow Server List System routes.
- dashboard with widgets from plugins (move/resize widgets).
- list of plugins (only implementing widgets for now).
- automatically load widget when implemented by a plugin.

## Stack
- [react](https://react.dev/learn) (library)
- [react-redux](https://react-redux.js.org/introduction/getting-started) (state management)
- [@mui](https://mui.com/material-ui/getting-started/) (UI/UX)
- [@mdi](https://pictogrammers.com/docs/library/mdi/getting-started/react/) (additional icons)
- [systemjs](https://github.com/systemjs/systemjs) (dynamically import UMD widgets)

## ROADMAP

- [ ] replace basic authentication using Discord OAuth2.
- [ ] add a Flecs page, using client Api to show stuff.
- [ ] let user hide/show widgets in Dashboard page.
- [ ] dynamically add plugin page, when implemented by plugin. It should be
      listed in Drawer component.

## Development

### Prerequisites
- [Node.js](https://nodejs.org/en/)
- [PNPM](https://pnpm.io/)
- server is built in `build/<os>/<arch>/debug/`

> [!TIP]
> **pnpm** is used instead of **npm** to save disk space on your system. This
> is particularly useful as multiple projects in this repository are using the
> same dependencies.

1. Clone the repository:
```shell
git clone https://github.com/tiltedphoques/CyberpunkMP.git
```
2. Move to directory `code/admin`
3. Run commands:
```shell
pnpm install
pnpm start
```
4. Start the backend server:
```shell
..\..\build\windows\x64\debug\Server.Loader.exe
```
5. Open your browser in `http://localhost:5173`. It is configured to proxy API 
request (`/api`) to the backend on `http://localhost:11778`.

### Create a widget for a plugin

TBD, see `code\server\scripting\EmoteSystem\Admin`.

## Production
You can manually build the webapp using `pnpm build`. It will output the build
in `dist/`. The content of this directory must be put in 
`distrib/launcher/server/assets`.

These steps are already implemented for you when running
`xmake install -o distrib`.

You'll need to define two environment variables, otherwise the server will 
abort when starting in release mode:
```shell
# PowerShell
$env:CYBERPUNKMP_ADMIN_USERNAME="<username>"
$env:CYBERPUNKMP_ADMIN_PASSWORD="<password>"

# Bash
export CYBERPUNKMP_ADMIN_USERNAME "<username>"
export CYBERPUNKMP_ADMIN_PASSWORD "<password>"
```
Now, your browser will show a popup asking you for the username/password 
credentials. Failing this step will prevent access to any routes of the backend
server. Except for `/api/v1/mods` and `/api/v1/statistics`, which must be 
available to anyone.
