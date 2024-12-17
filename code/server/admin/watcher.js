// NOTE: common script for plugins when implementing a widget in Admin/.
//       run `pnpm build:watch` to watch changes, build widget, copy output in
//       debug directory of the plugin. You'll need to hit F5 to reload the
//       widget when running `pnpm start` in `code/server/admin`.
import chokidar from 'chokidar';
import fs from 'fs';
import path from 'path';

const plugin = path.basename(path.dirname(process.cwd()));
const src = path.resolve(path.join(process.cwd(), 'dist'));
const dst = path.resolve(path.join('..', '..', '..', '..', '..', 'build', 'windows', 'x64', 'debug', 'plugins', plugin, 'assets'));

console.log(`watching Admin.${plugin}`);
console.log(`copying from "${src}"`);
console.log(`          to "${dst}"`);

const watcher = chokidar.watch('./dist');

process.once('SIGINT', abort);
process.once('SIGTERM', abort);

watcher.on('all', () => {
    fs.cpSync(src, dst, {recursive: true});
});

function abort() {
    console.log('Stopping watcher...');
    watcher.close();
}
