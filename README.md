# msoconv
document convert package for microsoft automation

we use exe to convert fist!

# next
write C++ addon code to convert

# usage

```javascript
const msoconv = require('msoconv')
msoconv('C:/test.pptx', 'C:/test.pdf')
.then(() => {
  console.log('convert ok')
})
.catch(err => {
  // 1 unknown error
  // 2 parameter error
  // 3 powerpoint error
  // 4 presentations error
  // 5 open file error
  // 6 unsupport type
  // 7 save as error
})
```