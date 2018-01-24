const path = require('path')
const conv = require('../index')

conv(path.resolve(__dirname, 'pp/test.pptx'), path.resolve(__dirname, 'out/pp-test.pdf'))
.then(code => {
  console.log('ok', code)
})
.catch(code => {
  console.error('error', code)
})
