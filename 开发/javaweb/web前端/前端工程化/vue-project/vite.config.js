import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'
import { resolve } from "path";
const pathResolve = (dir) => resolve(__dirname, dir);

export default defineConfig({
  plugins: [
    vue(),
  ],
  resolve: {
    alias: {
      "@": pathResolve("./src"), // 新增
    },
  },

})
