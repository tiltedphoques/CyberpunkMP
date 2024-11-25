import { defineConfig } from 'vite'
import { createHtmlPlugin } from 'vite-plugin-html'

export default defineConfig({
  build: {
    outDir: './electron/dist',
    rollupOptions: {
      // Hides MUI warning
      onwarn (warning, warn) {
        if (warning.code === 'MODULE_LEVEL_DIRECTIVE') {
          return
        }
        warn(warning)
      }
    }
  },
  base: './',
  plugins: [
    createHtmlPlugin({
      minify: true,
      entry: 'src/main.tsx',
      inject: {
        data: {
          devtools: process.env.NODE_ENV === 'development' ? '<script src="http://localhost:8097"></script>' : ''
        }
      }
    })
  ]
})
