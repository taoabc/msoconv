const conv = require('../index')

conv('D:/temp/test.pptx', 'D:/temp/out.pdf')
.then(code => {
  console.log('ok', code)
})
.catch(code => {
  console.error('error', code)
})
