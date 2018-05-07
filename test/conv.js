const path = require('path')
const conv = require('../index')

const ppSource = path.resolve(__dirname, 'pp/test.pptx')

// async function syncTest () {
//   await conv(ppSource, path.resolve(__dirname, 'out/pp-test.pdf'))
//   console.log('pdf ok')
//   await conv(ppSource, path.resolve(__dirname, 'out/pp-test-png'), 'PNG')
//   console.log('png ok')
//   await conv(ppSource, path.resolve(__dirname, 'out/pp-test-jpg'), 'jpg')
//   console.log('jpg ok')
//   await conv(ppSource, path.resolve(__dirname, 'out/pp-test-xps.xps'), 'xPs')
//   console.log('xps ok')
//   await conv(ppSource, path.resolve(__dirname, 'out/pp-test-bmp.bmp'))
//   console.log('bmp ok')
//   await conv(ppSource, path.resolve(__dirname, 'out/pp-test-gif'), 'Gif')
//   console.log('gif ok')
// }

async function asyncTest () {
  const promises = [
    conv(ppSource, path.resolve(__dirname, 'out/pp-test.pdf')),
    conv(ppSource, path.resolve(__dirname, 'out/pp-test-png'), 'PNG'),
    conv(ppSource, path.resolve(__dirname, 'out/pp-test-jpg'), 'jpg'),
    conv(ppSource, path.resolve(__dirname, 'out/pp-test-xps.xps'), 'xPs'),
    conv(ppSource, path.resolve(__dirname, 'out/pp-test-bmp.bmp')),
    conv(ppSource, path.resolve(__dirname, 'out/pp-test-gif'), 'Gif')
  ]
  return Promise.all(promises)
}

asyncTest().then(() => console.log('all ok')).catch(err => console.error(err))

// syncTest()
//   .then(() => {
//     console.log('all ok')
//   })
//   .catch(err => {
//     console.error(err)
//   })
