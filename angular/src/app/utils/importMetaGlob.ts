// // Polyfill for Vite's import.meta.glob in Angular/TypeScript
// // Usage: const modules = importMetaGlob('./**/*.ts');

// export function importMetaGlob(glob: string | string[]): Record<string, () => Promise<any>> {
//   // 支持字符串或字符串数组
//   const globs = Array.isArray(glob) ? glob : [glob];
//   const modules: Record<string, () => Promise<any>> = {};
//   globs.forEach((g) => {
//     try {
//       const context = (require as any).context(
//         g.replace(/\/\*\*\//, './').replace(/\*/g, ''),
//         false,
//         new RegExp(g.split('.').pop() ? `\\.${g.split('.').pop()}$` : '')
//       );
//       const keys = context.keys();
//       keys.forEach((key: string) => {
//         modules[key] = () => Promise.resolve(context(key));
//       });
//     } catch (e) {
//       // ignore unsupported glob
//     }
//   });
//   return modules;
// }

// // Example usage:
// // const modules = importMetaGlob('./utils/*.ts');
// // Object.entries(modules).forEach(([path, loader]) => loader().then(mod => ...));
